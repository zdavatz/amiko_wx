//  GoogleSyncManager.cpp
//  AmiKo-wx
//
//  Created by b123400 on 20 Nov 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include "GoogleSyncManager.hpp"
#include "GoogleConfig.h"
#include "../DefaultsController.hpp"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <wx/filename.h>
#include <wx/dir.h>
#include "../Operator.hpp"

#define FILE_FIELDS "id,name,version,parents,mimeType,modifiedTime,size,properties"

namespace GoogleAPITypes {
    void to_json(nlohmann::json& j, const RemoteFile& f) {
        j = nlohmann::json{{"id", f.id}, {"name", f.name}, {"mimeType", f.mimeType}, {"parents", f.parents}, {"modifiedTime", f.modifiedTime}};
    }

    void from_json(const nlohmann::json& j, RemoteFile& f) {
        j.at("id").get_to(f.id);
        j.at("name").get_to(f.name);
        j.at("mimeType").get_to(f.mimeType);
        j.at("parents").get_to(f.parents);
        j.at("modifiedTime").get_to(f.modifiedTime);
    }
}

GoogleSyncManager* GoogleSyncManager::m_pInstance;

// Singleton
GoogleSyncManager* GoogleSyncManager::Instance()
{
    if (!m_pInstance) {
        m_pInstance = new GoogleSyncManager();
    }

    return m_pInstance;
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, std::string *s) 
{
  s->append(static_cast<char *>(ptr), size*nmemb);
  return size*nmemb;
}

static size_t writeFileData(void *ptr, size_t size, size_t nmemb, void *stream)
{
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

void debugLogRemoteMap(std::map<std::string, GoogleAPITypes::RemoteFile> map) {
    for (auto const& x : map)
    {
        std::clog << x.first  // string (key)
                  << ": RemoteFile("
                  << x.second.name // string's value 
                  << ")"
                  << std::endl ;
    }
}

void debugLogStringSet(std::set<std::string> stringSet) {
    for (auto const& x : stringSet)
    {
        std::clog << x << std::endl;
    }
}

void debugLogStringMap(std::map<std::string, std::string> stringMap) {
    for (auto const& x : stringMap)
    {
        std::clog << x.first << ": " << x.second << std::endl;
    }
}

GoogleSyncManager::GoogleSyncManager() {
    curl_global_init(CURL_GLOBAL_ALL);
}

GoogleSyncManager::~GoogleSyncManager() {
    curl_global_cleanup();
}

std::string GoogleSyncManager::authURL() {
    return "https://accounts.google.com/o/oauth2/v2/auth?scope=email%20profile%20https://www.googleapis.com/auth/drive.appdata&response_type=code&state=1&redirect_uri=urn%3Aietf%3Awg%3Aoauth%3A2.0%3Aoob%3Aauto&client_id=" GOOGLE_CLIENT_ID;
}

bool GoogleSyncManager::isGoogleLoggedIn() {
    DefaultsController *defaults = DefaultsController::Instance();
    return defaults->Exists("google-access-token");
}

void GoogleSyncManager::receivedAuthCode(std::string code) {
    // Get Access token from code
    // https://developers.google.com/identity/protocols/oauth2/native-app
    CURL *curl = curl_easy_init();

    std::string s;

    std::string postString = \
        "code=" + code + \
        "&client_id=" + GOOGLE_CLIENT_ID + \
        "&client_secret=" GOOGLE_CLIENT_SECRET + \
        "&redirect_uri=urn%3Aietf%3Awg%3Aoauth%3A2.0%3Aoob%3Aauto" + \
        "&grant_type=authorization_code";

    curl_easy_setopt(curl, CURLOPT_URL, "https://oauth2.googleapis.com/token");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postString.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    auto json = nlohmann::json::parse(s);
    // Example response
    // {
    //   "access_token": "...",
    //   "expires_in": 3599,
    //   "refresh_token": "...",
    //   "scope": "...",
    //   "token_type": "Bearer",
    //   "id_token": "..."
    // }

    std::string accessToken = json["access_token"].get<std::string>();
    std::string refreshToken = json["refresh_token"].get<std::string>();
    int expires = json["expires_in"].get<int>();

    auto now = std::chrono::system_clock::now();
    auto expire = now + std::chrono::seconds(expires);
    auto expireIsoString = UTI::timeToString(expire);

#ifndef NDEBUG
    std::clog << "Access token: " << accessToken << std::endl;
    std::clog << "Refresh token: " << refreshToken << std::endl;
    std::clog << "expire: " << expireIsoString << std::endl;
#endif

    DefaultsController *defaults = DefaultsController::Instance();
    defaults->setString(wxString(accessToken), "google-access-token");
    defaults->setString(wxString(refreshToken), "google-refresh-token");
    defaults->setString(wxString(expireIsoString), "google-access-token-expire");
    defaults->Flush();
}

void GoogleSyncManager::logout() {
    DefaultsController *defaults = DefaultsController::Instance();
    defaults->DeleteEntry("google-access-token");
    defaults->DeleteEntry("google-refresh-token");
    defaults->DeleteEntry("google-access-token-expire");
    defaults->Flush();
}

std::string GoogleSyncManager::getAccessToken() {
    DefaultsController *defaults = DefaultsController::Instance();
    auto expireStr = defaults->getString("google-access-token-expire", "");
    if (expireStr.IsEmpty()) {
        return "";
    }
    auto expire = UTI::stringToTime(expireStr.ToStdString());
    auto now = std::chrono::system_clock::now();

    if (now < expire) {
        wxString at = defaults->getString("google-access-token", "");
        if (at.IsEmpty()) {
            return "";
        }
        return at.ToStdString();
    }

#ifndef NDEBUG
    std::clog << "Access Token expired, renewing" << std::endl;
#endif
    wxString rt = defaults->getString("google-refresh-token", "");
    if (rt.IsEmpty()) {
        return "";
    }
    auto refreshToken = rt.ToStdString();

    CURL *curl = curl_easy_init();

    std::string s;

    std::string postString = \
        "client_id=" GOOGLE_CLIENT_ID \
        "&client_secret=" GOOGLE_CLIENT_SECRET \
        "&refresh_token=" + refreshToken + \
        "&grant_type=refresh_token";

    curl_easy_setopt(curl, CURLOPT_URL, "https://oauth2.googleapis.com/token");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postString.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    std::clog << s << std::endl;
    auto json = nlohmann::json::parse(s);
    // Example response
    // {
    //   "access_token": "...",
    //   "expires_in": 3599,
    //   "scope": "...",
    //   "token_type": "Bearer",
    // }

    std::string accessToken = json["access_token"].get<std::string>();
    int expires = json["expires_in"].get<int>();

    now = std::chrono::system_clock::now();
    expire = now + std::chrono::seconds(expires);
    std::time_t expireT = std::chrono::system_clock::to_time_t(expire);

    std::ostringstream ss2;
    ss2 << std::put_time(gmtime(&expireT), "%FT%TZ");
    std::string expireIsoString = ss2.str();

#ifndef NDEBUG
    std::clog << "Access token: " << accessToken << std::endl;
    std::clog << "Expire: " << expireIsoString << std::endl;
#endif

    defaults->setString(wxString(accessToken), "google-access-token");
    defaults->setString(wxString(expireIsoString), "google-access-token-expire");
    defaults->Flush();
    return accessToken;
}

GoogleAPITypes::RemoteFile GoogleSyncManager::createFolder(std::string name, std::vector<std::string> parents) {
    auto accessToken = this->getAccessToken();
    if (accessToken.empty()) {
        return {};
    }

    CURL *curl = curl_easy_init();
    std::string s;
    curl_easy_setopt(curl, CURLOPT_URL, "https://www.googleapis.com/drive/v3/files?fields=" FILE_FIELDS);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

    struct curl_slist *chunk = NULL;
    chunk = curl_slist_append(chunk, ("Authorization: Bearer " + accessToken).c_str());
    chunk = curl_slist_append(chunk, "Content-Type: application/json; charset: utf-8");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

    nlohmann::json metadata;
    metadata["name"] = name;
    metadata["mimeType"] = "application/vnd.google-apps.folder";
    if (!parents.empty()) {
        metadata["parents"] = parents;
    } else {
        metadata["parents"] = {"appDataFolder"};
    }
    auto postData = metadata.dump();
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    std::clog << s << std::endl;
    // Example response
    // {
    //  "kind": "drive#file",
    //  "id": "1NvrC5PUAH_-AUmE6R_YrkS_M8KFNWLofWaFOeqIznFOq6iyFDA",
    //  "name": "testFolder",
    //  "mimeType": "application/vnd.google-apps.folder"
    // }
    auto json = nlohmann::json::parse(s);
    GoogleAPITypes::RemoteFile file = json;
    return file;
}

GoogleAPITypes::RemoteFile GoogleSyncManager::uploadFile(std::string name, std::string filePath, std::string mimeType, std::vector<std::string> parents) {
    auto accessToken = this->getAccessToken();
    if (accessToken.empty()) {
        return {};
    }

    CURL *curl = curl_easy_init();
    std::string s;
    curl_easy_setopt(curl, CURLOPT_URL, "https://www.googleapis.com/upload/drive/v3/files?uploadType=multipart&fields=" FILE_FIELDS);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

    struct curl_slist *chunk = NULL;
    chunk = curl_slist_append(chunk, ("Authorization: Bearer " + accessToken).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

    curl_mime *form = curl_mime_init(curl);
    curl_mimepart *field = NULL;

    field = curl_mime_addpart(form);
    curl_mime_name(field, "metadata");
    nlohmann::json metadata;
    metadata["name"] = name;
    metadata["parents"] = parents;
    curl_mime_data(field, metadata.dump().c_str(), CURL_ZERO_TERMINATED);
    curl_mime_type(field, "application/json; charset=UTF-8");

    /* Fill in the file upload field */ 
    field = curl_mime_addpart(form);
    curl_mime_name(field, "file");
    curl_mime_filedata(field, filePath.c_str());
    curl_mime_type(field, mimeType.c_str());

    curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    std::clog << s << std::endl;
    // Example response
    // {
    //  "kind": "drive#file",
    //  "id": "1NvrC5PUAH_-AUmE6R_YrkS_M8KFNWLofWaFOeqIznFOq6iyFDA",
    //  "name": "hk.png",
    //  "mimeType": "image/png"
    // }
    auto json = nlohmann::json::parse(s);
    GoogleAPITypes::RemoteFile file = json;
    return file;
}

GoogleAPITypes::RemoteFile GoogleSyncManager::updateFile(std::string fileId, std::string filePath) {
    auto accessToken = this->getAccessToken();
    if (accessToken.empty()) {
        return {};
    }

    CURL *curl = curl_easy_init();
    std::string s;
    curl_easy_setopt(curl, CURLOPT_URL, ("https://www.googleapis.com/upload/drive/v3/files/" + fileId + "?uploadType=multipart&fields=" + FILE_FIELDS).c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

    struct curl_slist *chunk = NULL;
    chunk = curl_slist_append(chunk, ("Authorization: Bearer " + accessToken).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

    curl_mime *form = curl_mime_init(curl);
    curl_mimepart *field = NULL;

    field = curl_mime_addpart(form);
    curl_mime_name(field, "metadata");
    nlohmann::json metadata;
    curl_mime_data(field, metadata.dump().c_str(), CURL_ZERO_TERMINATED);
    curl_mime_type(field, "application/json; charset=UTF-8");

    /* Fill in the file upload field */ 
    field = curl_mime_addpart(form);
    curl_mime_name(field, "file");
    curl_mime_filedata(field, filePath.c_str());
    // curl_mime_type(field, mimeType.c_str());
    curl_mime_type(field, "application/octet-stream");

    curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    std::clog << s << std::endl;
    // Example response
    // {
    //  "kind": "drive#file",
    //  "id": "1NvrC5PUAH_-AUmE6R_YrkS_M8KFNWLofWaFOeqIznFOq6iyFDA",
    //  "name": "hk.png",
    //  "mimeType": "image/png"
    // }
    auto json = nlohmann::json::parse(s);
    GoogleAPITypes::RemoteFile file = json;
    return file;
}

void GoogleSyncManager::deleteFile(std::string fileId) {
    auto accessToken = this->getAccessToken();
    if (accessToken.empty()) {
        return;
    }

    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, ("https://www.googleapis.com/drive/v3/files/" + fileId).c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

    struct curl_slist *chunk = NULL;
    chunk = curl_slist_append(chunk, ("Authorization: Bearer " + accessToken).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
}

void GoogleSyncManager::downloadFile(std::string fileId, std::string path) {
    auto accessToken = this->getAccessToken();
    if (accessToken.empty()) {
        return;
    }

    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, ("https://www.googleapis.com/drive/v3/files/" + fileId + "?alt=media").c_str());

    struct curl_slist *chunk = NULL;
    chunk = curl_slist_append(chunk, ("Authorization: Bearer " + accessToken).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFileData);

    FILE *file = fopen(path.c_str(), "wb");
    if(file) {
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
        CURLcode res = curl_easy_perform(curl);
    }

    curl_easy_cleanup(curl);
}

std::vector<GoogleAPITypes::RemoteFile> GoogleSyncManager::listRemoteFilesAndFolders(std::string pageToken) {
    auto accessToken = this->getAccessToken();
    if (accessToken.empty()) {
        return {};
    }

    std::string queryString = \
        "?pageSize=1000" \
        "&spaces=appDataFolder" \
        "&fields=nextPageToken,files(" FILE_FIELDS ")";

    if (!pageToken.empty()) {
        queryString += "&pageToken=" + pageToken;
    }

    CURL *curl = curl_easy_init();
    std::string s;
    curl_easy_setopt(curl, CURLOPT_URL, ("https://www.googleapis.com/drive/v3/files" + queryString).c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

    struct curl_slist *chunk = NULL;
    chunk = curl_slist_append(chunk, ("Authorization: Bearer " + accessToken).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    std::clog << s << std::endl;
    auto json = nlohmann::json::parse(s);
    // Example response
    // {
    //  "kind": "drive#fileList",
    //  "incompleteSearch": false,
    //  "files": [
    //   {
    //    "kind": "drive#file",
    //    "id": "1NvrC5PUAH_-AUmE6R_YrkS_M8KFNWLofWaFOeqIznFOq6iyFDA",
    //    "name": "hk.png",
    //    "mimeType": "image/png"
    //   }
    //  ]
    // }
    auto remoteFiles = json["files"].get<std::vector<GoogleAPITypes::RemoteFile>>();
    nlohmann::json j = remoteFiles;
    std::cout << "json: " << j << std::endl;

    if (json.contains("nextPageToken")) {
        auto rest = listRemoteFilesAndFolders(json["nextPageToken"].get<std::string>());
        remoteFiles.insert(remoteFiles.end(), rest.begin(), rest.end());
    }
    return remoteFiles;
}

std::map<std::string, GoogleAPITypes::RemoteFile> remoteFilesToMap(std::vector<GoogleAPITypes::RemoteFile> remoteFiles) {
    std::map<std::string, GoogleAPITypes::RemoteFile> idMap;
    for (auto remoteFile : remoteFiles) {
        idMap[remoteFile.id] = remoteFile;
    }

    std::map<std::string, GoogleAPITypes::RemoteFile> fileMap;
    for (auto remoteFile : remoteFiles) {
        wxFileName filename;
        filename.SetFullName(remoteFile.name);
        if (!remoteFile.parents.empty()) {
            std::string fileId = remoteFile.parents[0];
            do {
                if (idMap.find(fileId) != idMap.end()) {
                    GoogleAPITypes::RemoteFile parent = idMap[fileId];
                    filename.PrependDir(wxString(parent.name));
                    if (!parent.parents.empty()) {
                        fileId = parent.parents[0];
                    } else {
                        break;
                    }
                } else {
                    fileId = "";
                }
            } while (!fileId.empty());
        }

        fileMap[filename.GetFullPath().ToStdString()] = remoteFile;
    }
    return fileMap;
}

std::set<std::string> GoogleSyncManager::listLocalFilesAndFolders(wxString path) {
    wxArrayString filenameStrs;
    std::set<std::string> localFiles;
    wxDir::GetAllFiles(path, &filenameStrs);
    for (auto filenameStr : filenameStrs) {
        wxFileName filename(filenameStr);
        filename.MakeRelativeTo(path);
        if (shouldSyncLocalFile(filename)) {
            localFiles.insert(filename.GetFullPath().ToStdString());
        }
    }
    return localFiles;
}

// Remove patient files from remote file map
// Return a <patient-id : remote file> map
std::map<std::string, GoogleAPITypes::RemoteFile> extractPatientsFromRemoteFiles(std::map<std::string, GoogleAPITypes::RemoteFile> *filesPtr) {
    std::map<std::string, GoogleAPITypes::RemoteFile> patients;
    auto files = *filesPtr;
    for (auto entry : files) {
        auto filename = wxFileName(wxString(entry.first));
        auto dirs = filename.GetDirs();
        if (!dirs.IsEmpty() && dirs[0].ToStdString() == "patients") {
            patients[filename.GetFullName().ToStdString()] = entry.second;
        }
    }
    for (auto entry : patients) {
        std::string filename = "patients" + wxString(wxFILE_SEP_PATH).ToStdString() + entry.first;
        size_t deleted = files.erase(filename);
    }
    *filesPtr = files;
    return patients;
}

std::map<std::string, long> localFileVersionMap() {
    std::map<std::string, long> map;
    try {
        std::string path = (UTI::documentsDirectory() + wxFILE_SEP_PATH + "googleSync" + wxFILE_SEP_PATH + "versions.json").ToStdString();
        std::ifstream i(path);
        nlohmann::json json;
        i >> json;

        for (auto& el : json.items()) {
            map[el.key()] = el.value().get<long>();
        }
    } catch (...) {}
    return map;
}

bool GoogleSyncManager::shouldSyncLocalFile(wxFileName path) {
    wxString fullPath = path.GetFullPath();
    if (path.GetFullName() == ".DS_Store") {
        return false;
    }
    if (fullPath.ToStdString() == DOC_JSON_FILENAME || fullPath.ToStdString() == DOC_SIGNATURE_FILENAME) {
        return true;
    }
    auto dirs = path.GetDirs();
    if (!dirs.IsEmpty() && dirs[0].ToStdString() == "amk") {
        return true;
    }
    return false;
}

void GoogleSyncManager::sync() {
    std::cout << "Start syncing" << std::endl;

    if (!this->isGoogleLoggedIn()) {
        return;
    }

    std::vector<GoogleAPITypes::RemoteFile> remoteFiles = this->listRemoteFilesAndFolders();
    auto remoteFilesMap = remoteFilesToMap(remoteFiles);
    std::map<std::string, GoogleAPITypes::RemoteFile> remotePatientFilesMap = extractPatientsFromRemoteFiles(&remoteFilesMap);
    std::map<std::string, long> localVersionMap = localFileVersionMap();

    std::clog << "remote files:" << std::endl;
    debugLogRemoteMap(remoteFilesMap);
    std::clog << "remote patient:" << std::endl;
    debugLogRemoteMap(remotePatientFilesMap);

    std::set<std::string> localFiles = this->listLocalFilesAndFolders();
    std::clog << "local files:" << std::endl;
    debugLogStringSet(localFiles);

    std::set<std::string> pathsToCreate;
    std::map<std::string, std::string> pathsToUpdate; // { path: remote file id }
    std::map<std::string, std::string> pathsToDownload; // { path: remote file id }
    std::set<std::string> localFilesToDelete;
    std::set<std::string> remoteFilesToDelete;

    {
        pathsToCreate.insert(localFiles.begin(), localFiles.end());
        for (auto const& entry : localVersionMap) {
            pathsToCreate.erase(entry.first);
        }
        for (auto const& entry : remoteFilesMap) {
            pathsToCreate.erase(entry.first);
        }
    }
    for (auto const& entry : localVersionMap) {
        auto path = entry.first;
        bool localHasFile = localFiles.find(path) != localFiles.end();
        bool remoteHasFile = remoteFilesMap.find(path) != remoteFilesMap.end();

        if (localHasFile && remoteHasFile) {
            long localVersion = localVersionMap[path];
            wxFileName filename = wxFileName(UTI::documentsDirectory() + wxFILE_SEP_PATH + path);
            GoogleAPITypes::RemoteFile remoteFile = remoteFilesMap[path];

            long remoteVersion = std::stol(remoteFile.version);

            if (filename.IsDir() || remoteFile.mimeType == "application/vnd.google-apps.folder") {
                continue;
            }
            if (remoteVersion == localVersion) {
                wxDateTime localModified = filename.GetModificationTime();
                wxDateTime remoteModified = wxDateTime(std::chrono::system_clock::to_time_t(UTI::stringToTime(remoteFile.modifiedTime)));
                if (localModified.IsLaterThan(remoteModified)) {
                    pathsToUpdate[path] = remoteFile.id;
                }
            } else if (remoteVersion > localVersion) {
                pathsToDownload[path] = remoteFile.id;
            }
        }
        if (!localHasFile && remoteHasFile) {
            GoogleAPITypes::RemoteFile remoteFile = remoteFilesMap[path];
            if (remoteFile.mimeType != "application/vnd.google-apps.folder") {
                remoteFilesToDelete.insert(path);
            }
        }
        if (localHasFile && !remoteHasFile) {
            localFilesToDelete.insert(path);
        }
    }

    for (const auto entry : remoteFilesMap) {
        std::string path = entry.first;
        GoogleAPITypes::RemoteFile remoteFile = remoteFilesMap[path];
        if (remoteFile.mimeType == "application/vnd.google-apps.folder") {
            continue;
        }
        if (localVersionMap.find(path) != localVersionMap.end()) {
            // We already handled this in the above loop
            continue;
        }
        if (localFiles.find(path) != localFiles.end()) {
            // File exist both on server and local, but has no local version
            wxFileName filename = wxFileName(UTI::documentsDirectory() + wxFILE_SEP_PATH + path);
            wxDateTime localModified = filename.GetModificationTime();
            auto remoteModified = wxDateTime(std::chrono::system_clock::to_time_t(UTI::stringToTime(remoteFile.modifiedTime)));
            if (localModified.IsLaterThan(remoteModified)) {
                pathsToUpdate[path] = remoteFile.id;
            } else if (localModified.IsEarlierThan(remoteModified)) {
                pathsToDownload[path] = remoteFile.id;
            }
        } else {
            pathsToDownload[path] = remoteFile.id;
        }
    }

    std::clog << "pathsToCreate: " << std::endl;
    debugLogStringSet(pathsToCreate);
    std::clog << "pathsToUpdate: " << std::endl;
    debugLogStringMap(pathsToUpdate);
    std::clog << "pathsToDownload: " << std::endl;
    debugLogStringMap(pathsToDownload);
    std::clog << "localFilesToDelete: " << std::endl;
    debugLogStringSet(localFilesToDelete);
    std::clog << "remoteFilesToDelete: " << std::endl;
    debugLogStringSet(remoteFilesToDelete);

    // Create folders on Google drive
    {
        std::set<std::string> allFoldersToCreate;
        for (const auto& path : localFiles) {
            wxFileName filename = wxFileName(path);
            std::string parentPath = filename.GetPath().ToStdString();
            if (!parentPath.empty()) {
                if (remoteFilesMap.find(parentPath) == remoteFilesMap.end()) {
                    // not exist in remote
                    allFoldersToCreate.insert(parentPath);
                }
            }
        }
        while (true) {
            if (allFoldersToCreate.empty()) {
                break;
            }
            std::set<std::string> foldersCreated;
            for (std::string folderToCreate : allFoldersToCreate) {
                wxFileName folderName = wxFileName(folderToCreate);
                wxFileName absolutePath = wxFileName(UTI::documentsDirectory() + wxFILE_SEP_PATH + folderToCreate);

                bool atRoot = folderName.GetPath().ToStdString() == UTI::documentsDirectory().ToStdString();
                std::string parentRelativeToFilesDir = wxFileName(folderToCreate).GetPath().ToStdString(); // Parent of this folder
                if (!atRoot && remoteFilesMap.find(parentRelativeToFilesDir) == remoteFilesMap.end()) {
                    // parent is not created yet, skip this
                    continue;
                }
                GoogleAPITypes::RemoteFile remoteParent = remoteFilesMap[parentRelativeToFilesDir];
                GoogleAPITypes::RemoteFile createdFolder;
                if (atRoot) {
                    createdFolder = createFolder(folderName.GetFullName().ToStdString(), {"appDataFolder"});
                } else {
                    createdFolder = createFolder(folderName.GetFullName().ToStdString(), {remoteParent.id});
                }
                remoteFilesMap[folderToCreate] = createdFolder;
                foldersCreated.insert(folderToCreate);
            }
            allFoldersToCreate.erase(foldersCreated.begin(), foldersCreated.end());
        }
    }

    // create files
    {
        std::set<std::string> toRemove;
        for (std::string pathToCreate : pathsToCreate) {
            wxFileName filename = wxFileName(UTI::documentsDirectory() + wxFILE_SEP_PATH + pathToCreate);
            wxString parentFilename = filename.GetPath();
            bool atRoot = parentFilename.IsSameAs(UTI::documentsDirectory());
            std::string parentRelativeToFilesDir = wxFileName(pathToCreate).GetPath().ToStdString();
            if (remoteFilesMap.find(parentRelativeToFilesDir) == remoteFilesMap.end()) {
                std::cerr << "Cannot find remote parent for: " << parentRelativeToFilesDir << std::endl;
                continue;
            }
            GoogleAPITypes::RemoteFile remoteParent = remoteFilesMap[parentRelativeToFilesDir];

            if (!atRoot && remoteParent.mimeType != "application/vnd.google-apps.folder") {
                std::cerr << "remoteParent is not a folder? " << remoteParent.mimeType << std::endl;
            }

            std::clog << "Uploading new files: " << pathToCreate << std::endl;

            // try {
                auto uploadedFile = uploadFile(
                    filename.GetFullName().ToStdString(), // file name
                    filename.GetFullPath().ToStdString(), // path of the file content
                    "application/octet-stream",
                    {atRoot ? "appDataFolder" : remoteParent.id} // parent folder
                );

                std::clog << "Uploaded: " << uploadedFile.id << std::endl;

                toRemove.insert(pathToCreate);
                remoteFilesMap[pathToCreate] = uploadedFile;
            // } catch (...) {}
        }
        for (auto r : toRemove) {
            pathsToCreate.erase(r);
        }
    }

    // update files
    {
        std::set<std::string> toRemove;
        for (const auto& entry : pathsToUpdate) {
            std::string path = entry.first;
            std::string fileId = pathsToUpdate[path];
            // try {
                std::clog << "Updating: " << path << std::endl;
                GoogleAPITypes::RemoteFile remoteFile = updateFile(fileId, path);
                toRemove.insert(path);
                remoteFilesMap[path] = remoteFile;
                std::clog << "Updated: " << remoteFile.id << std::endl;
            // } catch (...) {}
        }
        for (auto r : toRemove) {
            pathsToUpdate.erase(r);
        }
    }

    // delete files
    {
        std::set<std::string> toRemove;
        for (std::string path : remoteFilesToDelete) {
            GoogleAPITypes::RemoteFile remoteFile = remoteFilesMap[path];
            deleteFile(remoteFile.id);

            std::clog << "Deleting: " << remoteFile.id << " " << path << std::endl;
            toRemove.insert(path);
            remoteFilesMap.erase(path);
            std::clog << "Deleted" << std::endl;
        }
        remoteFilesToDelete.erase(toRemove.begin(), toRemove.end());
    }

    // download remote files
    {
        for (const auto& entry : pathsToDownload) {
            std::string path = entry.first;
            wxFileName filename = wxFileName(UTI::documentsDirectory() + wxFILE_SEP_PATH + path);
            wxFileName parent = wxFileName(filename.GetPath());
            std::string fileId = entry.second;
            GoogleAPITypes::RemoteFile remoteFile = remoteFilesMap[path];
            UTI::ensureDirectory(parent);

            if (remoteFile.size == "0") {
                continue;
            }
            std::clog << "Downloading " << fileId << " " << path << std::endl;
            downloadFile(fileId, path);
            std::clog << "Downloaded" << std::endl;
        }
    }

    // delete local files
    {
        for (std::string path : localFilesToDelete) {
            std::clog << "Deleting " << path << std::endl;
            wxFileName filename = wxFileName(UTI::documentsDirectory() + wxFILE_SEP_PATH + path);
            if (filename.IsDir()) {
                filename.Rmdir();
            } else {
                remove(filename.GetFullPath().ToStdString().c_str());
            }
        }
    }
}
