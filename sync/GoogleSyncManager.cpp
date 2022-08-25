//  GoogleSyncManager.cpp
//  AmiKo-wx
//
//  Created by b123400 on 20 Nov 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include "GoogleSyncManager.hpp"
#include "PatientDBAdapter.hpp"
#include "../DefaultsController.hpp"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include <wx/filename.h>
#include <wx/dir.h>
#include "../Patient.hpp"
#include "../Operator.hpp"

#define FILE_FIELDS "id,name,version,parents,mimeType,modifiedTime,size,properties"

static const std::string KEY_TIMESTAMP = "time_stamp";
static const std::string KEY_UID = "uid";
static const std::string KEY_FAMILYNAME = "family_name";
static const std::string KEY_GIVENNAME = "given_name";
static const std::string KEY_BIRTHDATE = "birthdate";
static const std::string KEY_GENDER  = "gender";
static const std::string KEY_WEIGHT_KG = "weight_kg";
static const std::string KEY_HEIGHT_CM = "height_cm";
static const std::string KEY_ZIPCODE = "zip";
static const std::string KEY_CITY = "city";
static const std::string KEY_COUNTRY = "country";
static const std::string KEY_ADDRESS = "address";
static const std::string KEY_PHONE = "phone";
static const std::string KEY_EMAIL = "email";

namespace GoogleAPITypes {
    void to_json(nlohmann::json& j, const RemoteFile& f) {
        j = nlohmann::json{
            {"id", f.id}, 
            {"name", f.name}, 
            {"mimeType", f.mimeType}, 
            {"parents", f.parents}, 
            {"modifiedTime", f.modifiedTime},
            {"version", f.version},
            {"size", f.size},
            {"properties", f.properties}
        };
    }

    void from_json(const nlohmann::json& j, RemoteFile& f) {
        j.at("id").get_to(f.id);
        j.at("name").get_to(f.name);
        j.at("mimeType").get_to(f.mimeType);
        j.at("parents").get_to(f.parents);
        j.at("modifiedTime").get_to(f.modifiedTime);
        j.at("version").get_to(f.version);
        if (j.contains("size")) {
            j.at("size").get_to(f.size);
        }
        if (j.contains("properties")) {
            j.at("properties").get_to(f.properties);
        }
    }
}

GoogleSyncManager* GoogleSyncManager::m_pInstance;

wxDEFINE_EVENT(SYNC_MANAGER_UPDATED_PATIENT, wxCommandEvent);
wxDEFINE_EVENT(SYNC_MANAGER_UPDATED_DOCTOR, wxCommandEvent);
wxDEFINE_EVENT(SYNC_MANAGER_UPDATED_AMK, wxCommandEvent);

// Singleton
GoogleSyncManager* GoogleSyncManager::Instance()
{
    if (!m_pInstance) {
        m_pInstance = new GoogleSyncManager();
    }

    return m_pInstance;
}

void GoogleSyncManager::syncLoop() {
    while (true) {
        auto now = std::chrono::system_clock::now();
        auto nextSync = lastSynced + std::chrono::seconds(3 * 60); // 3 minutes
        if (wantToStartSyncing || now >= nextSync ) {
            syncMutex.lock();
            isSyningNow = true;
            wantToStartSyncing = false;
            lastSynced = std::chrono::system_clock::now();
            try {
                sync();
            } catch (std::exception& e) {
                std::cout << "Sync error:" << std::endl;
                std::cout << e.what() << std::endl;
            }
            isSyningNow = false;
            syncMutex.unlock();
        }
        if (wantToStopSyncing || !this->isGoogleLoggedIn()) {
            break;
        }
        sleep(1);
    }
}

void GoogleSyncManager::startBackgroundSync() {
    if (!startedSyncing) {
        startedSyncing = true;
        std::thread background(&GoogleSyncManager::syncLoop, this);
        background.detach();
    }
}

void GoogleSyncManager::stopBackgroundSync() {
    startedSyncing = false;
    wantToStopSyncing = true;
}

void GoogleSyncManager::requestSync() {
    if (startedSyncing && !isSyningNow) {
        wantToStartSyncing = true;
    }
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
                  << x.second.name
                  << ","
                  << x.second.version
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

std::string getGoogleClientId() {
    if (wxString(APP_NAME) == "CoMed")
        return COMED_GOOGLE_CLIENT_ID;
    return AMIKO_GOOGLE_CLIENT_ID;
}

std::string getGoogleClientSecret() {
    if (wxString(APP_NAME) == "CoMed")
        return COMED_GOOGLE_CLIENT_SECRET;
    return AMIKO_GOOGLE_CLIENT_SECRET;
}

std::string GoogleSyncManager::authURL(int port) {
    return "https://accounts.google.com/o/oauth2/v2/auth?scope=email%20profile%20https://www.googleapis.com/auth/drive.appdata&response_type=code&state=1&redirect_uri=http%3A//127.0.0.1:" + std::to_string(port) + "&client_id=" + getGoogleClientId();
}

bool GoogleSyncManager::isGoogleLoggedIn() {
    DefaultsController *defaults = DefaultsController::Instance();
    return !defaults->getString("google-access-token", "").IsEmpty();
}

void GoogleSyncManager::receivedAuthCode(std::string code, int port) {
    // Get Access token from code
    // https://developers.google.com/identity/protocols/oauth2/native-app
    CURL *curl = curl_easy_init();

    std::string redirectUri = "http%3A//127.0.0.1:" + std::to_string(port);

    std::string s;

    std::string postString = \
        "code=" + code + \
        "&client_id=" + getGoogleClientId() + \
        "&client_secret=" + getGoogleClientSecret() + \
        "&redirect_uri=" + redirectUri + \
        "&grant_type=authorization_code";

    curl_easy_setopt(curl, CURLOPT_URL, "https://oauth2.googleapis.com/token");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postString.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    std::clog << "Response: " << s << std::endl;

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

    std::clog << "Access token: " << accessToken << std::endl;
    std::clog << "Refresh token: " << refreshToken << std::endl;
    std::clog << "expire: " << expireIsoString << std::endl;

    DefaultsController *defaults = DefaultsController::Instance();
    defaults->setString(wxString(accessToken), "google-access-token");
    defaults->setString(wxString(refreshToken), "google-refresh-token");
    defaults->setString(wxString(expireIsoString), "google-access-token-expire");
    defaults->Flush();
}

void GoogleSyncManager::logout() {
    DefaultsController *defaults = DefaultsController::Instance();
    defaults->setString("", "google-access-token");
    defaults->setString("", "google-refresh-token");
    defaults->setString("", "google-access-token-expire");
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
        "client_id=" + getGoogleClientId() + \
        "&client_secret=" + getGoogleClientSecret() + \
        "&refresh_token=" + refreshToken + \
        "&grant_type=refresh_token";

    curl_easy_setopt(curl, CURLOPT_URL, "https://oauth2.googleapis.com/token");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postString.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    // std::clog << s << std::endl;
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

    if (parents.empty()) {
        parents = {"appDataFolder"};
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
    metadata["modifiedTime"] = UTI::timeToString(std::chrono::system_clock::from_time_t(wxFileName(filePath).GetModificationTime().GetTicks()));
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

GoogleAPITypes::RemoteFile GoogleSyncManager::createFileWithMetadata(
    std::string name, 
    std::map<std::string, std::string> properties,
    std::chrono::time_point<std::chrono::system_clock> modifiedTime,
    std::vector<std::string> parents
) {
    auto accessToken = this->getAccessToken();
    if (accessToken.empty()) {
        return {};
    }

    if (parents.empty()) {
        parents = {"appDataFolder"};
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
    metadata["parents"] = parents;
    metadata["modifiedTime"] = UTI::timeToString(modifiedTime);
    metadata["mimeType"] = "application/octet-stream";
    metadata["properties"] = properties;

    auto postData = metadata.dump();
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

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

GoogleAPITypes::RemoteFile GoogleSyncManager::updateFileWithMetadata(
    std::string fileId, 
    std::map<std::string, std::string> properties,
    std::chrono::time_point<std::chrono::system_clock> modifiedTime,
    std::vector<std::string> parents
) {
    auto accessToken = this->getAccessToken();
    if (accessToken.empty()) {
        return {};
    }

    CURL *curl = curl_easy_init();
    std::string s;
    curl_easy_setopt(curl, CURLOPT_URL, ("https://www.googleapis.com/drive/v3/files/" + fileId + "?fields=" + FILE_FIELDS).c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

    struct curl_slist *chunk = NULL;
    chunk = curl_slist_append(chunk, ("Authorization: Bearer " + accessToken).c_str());
    chunk = curl_slist_append(chunk, "Content-Type: application/json; charset: utf-8");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

    nlohmann::json metadata;
    if (!parents.empty()) {
        metadata["parents"] = parents;
    }
    metadata["modifiedTime"] = UTI::timeToString(modifiedTime);
    metadata["mimeType"] = "application/octet-stream";
    metadata["properties"] = properties;

    auto postData = metadata.dump();
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

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
        fclose(file);
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

    // std::clog << s << std::endl;
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

    if (json.contains("nextPageToken")) {
        auto rest = listRemoteFilesAndFolders(json["nextPageToken"].get<std::string>());
        remoteFiles.insert(remoteFiles.end(), rest.begin(), rest.end());
    }
    return remoteFiles;
}

// {Path : RemoteFile}
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
template <typename T>
std::map<std::string, T> extractPatientsFromFilesMap(std::map<std::string, T> *filesPtr) {
    std::map<std::string, T> patients;
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
    } catch (std::exception& e) {
        std::clog << "file version: " << e.what() << std::endl;
    }
    return map;
}

void saveLocalFileVersionMap(std::map<std::string, long> map) {
    try {
        wxString syncDir = UTI::documentsDirectory() + wxFILE_SEP_PATH + "googleSync";
        std::string path = (syncDir + wxFILE_SEP_PATH + "versions.json").ToStdString();
        UTI::ensureDirectory(wxFileName(syncDir.ToStdString()));
        std::ofstream o(path);
        nlohmann::json json = map;
        o << json;
    } catch (std::exception &e) {
        std::clog << "save file version: " << e.what() << std::endl;
    }
}

bool GoogleSyncManager::shouldSyncLocalFile(wxFileName path) {
    wxString fullPath = path.GetFullPath();
    if (path.GetFullName() == ".DS_Store") {
        return false;
    }
    if (fullPath.ToStdString() == DOC_JSON_FILENAME 
        || fullPath.ToStdString() == DOC_SIGNATURE_FILENAME
        || fullPath.ToStdString() == "favorites.json"
        || fullPath.ToStdString() == "favorites-full-text.json"
    ) {
        return true;
    }
    auto dirs = path.GetDirs();
    if (!dirs.IsEmpty() && dirs[0].ToStdString() == "amk") {
        return true;
    }
    return false;
}

std::map<std::string, long> remoteFilesToVersionMap(std::map<std::string, GoogleAPITypes::RemoteFile> remoteFilesMap) {
    std::map<std::string, long> versionMap;
    for (const auto& remoteFile : remoteFilesMap) {
        std::clog << remoteFile.first << ":" << remoteFile.second.version << std::endl;
        try {
            versionMap[remoteFile.first] = std::stol(remoteFile.second.version);
        } catch (std::exception & e) {
            std::clog << e.what() << std::endl;
        }
    }
    return versionMap;
}

std::map<std::string, time_t> getLocalPatientTimestamps() {
    std::map<std::string, std::string> strMap = PatientDBAdapter::sharedInstance()->getAllTimestamps();
    std::map<std::string, time_t> map;
    for (const auto& entry : strMap) {
        auto uid = entry.first;
        auto timeStr = entry.second;
        map[uid] = std::chrono::system_clock::to_time_t(UTI::patientStringToTime(timeStr));
    }
    return map;
}

std::map<std::string, std::string> patientToStringMap(Patient *patient) {
    std::map<std::string, std::string> map;
    map[KEY_TIMESTAMP] = patient->timestamp.ToStdString();
    map[KEY_UID] = patient->uniqueId.ToStdString();
    map[KEY_FAMILYNAME] = patient->familyName.ToStdString();
    map[KEY_GIVENNAME] = patient->givenName.ToStdString();
    map[KEY_BIRTHDATE] = patient->birthDate.ToStdString();
    map[KEY_GENDER] = patient->gender.ToStdString();
    map[KEY_WEIGHT_KG] = std::to_string(patient->weightKg);
map[KEY_HEIGHT_CM] = std::to_string(patient->heightCm);
    map[KEY_ZIPCODE] = patient->zipCode.ToStdString();
    map[KEY_CITY] = patient->city.ToStdString();
    map[KEY_COUNTRY] = patient->country.ToStdString();
    map[KEY_ADDRESS] = patient->postalAddress.ToStdString();
    map[KEY_PHONE] = patient->phoneNumber.ToStdString();
    map[KEY_EMAIL] = patient->emailAddress.ToStdString();
    return map;
}

Patient* stringMapToPatient(std::map<std::string, std::string> map) {
    Patient* patient = new Patient();
    patient->timestamp = wxString(map[KEY_TIMESTAMP]);
    patient->uniqueId = wxString(map[KEY_UID]);
    patient->familyName = wxString(map[KEY_FAMILYNAME]);
    patient->givenName = wxString(map[KEY_GIVENNAME]);
    patient->birthDate = wxString(map[KEY_BIRTHDATE]);
    patient->gender = wxString(map[KEY_GENDER]);
    patient->weightKg = std::stoi(map[KEY_WEIGHT_KG]);
    patient->heightCm = std::stoi(map[KEY_HEIGHT_CM]);
    patient->zipCode = wxString(map[KEY_ZIPCODE]);
    patient->city = wxString(map[KEY_CITY]);
    patient->country = wxString(map[KEY_COUNTRY]);
    patient->postalAddress = wxString(map[KEY_ADDRESS]);
    patient->phoneNumber = wxString(map[KEY_PHONE]);
    patient->emailAddress = wxString(map[KEY_EMAIL]);
    return patient;
}

void GoogleSyncManager::sync() {
    std::clog << "Start syncing" << std::endl;

    if (!this->isGoogleLoggedIn()) {
        return;
    }

    std::vector<GoogleAPITypes::RemoteFile> remoteFiles = this->listRemoteFilesAndFolders();
    auto remoteFilesMap = remoteFilesToMap(remoteFiles);
    std::map<std::string, GoogleAPITypes::RemoteFile> remotePatientsMap = extractPatientsFromFilesMap(&remoteFilesMap);
    std::map<std::string, long> localVersionMap = localFileVersionMap();
    std::map<std::string, long> patientVersions = extractPatientsFromFilesMap(&localVersionMap);

    std::clog << "remote files:" << std::endl;
    debugLogRemoteMap(remoteFilesMap);
    std::clog << "remote patient:" << std::endl;
    debugLogRemoteMap(remotePatientsMap);

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

    bool willUpdateDoctor = pathsToDownload.find(DOC_SIGNATURE_FILENAME) != pathsToDownload.end()
        || pathsToDownload.find(DOC_JSON_FILENAME) != pathsToDownload.end();
    bool willUpdateAMK = false;
    for (const auto& entry : pathsToDownload) {
        auto path = entry.first;
        auto dirs = wxFileName(path).GetDirs();
        if (!dirs.IsEmpty() && dirs[0] == "amk") {
            willUpdateAMK = true;
        }
    }
    for (const auto& path : localFilesToDelete) {
        auto dirs = wxFileName(path).GetDirs();
        if (!dirs.IsEmpty() && dirs[0] == "amk") {
            willUpdateAMK = true;
        }
    }


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
            for (auto a : foldersCreated) {
                allFoldersToCreate.erase(a);
            }
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
            if (!atRoot && remoteFilesMap.find(parentRelativeToFilesDir) == remoteFilesMap.end()) {
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
            try {
                std::clog << "Updating: " << path << std::endl;
                GoogleAPITypes::RemoteFile remoteFile = updateFile(fileId, wxFileName(UTI::documentsDirectory() + wxFILE_SEP_PATH + path).GetFullPath().ToStdString());
                toRemove.insert(path);
                remoteFilesMap[path] = remoteFile;
                std::clog << "Updated: " << remoteFile.id << std::endl;
            } catch (const std::exception& e) {
                std::clog << e.what() << std::endl;
            }
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
        for (auto a : toRemove) {
            remoteFilesToDelete.erase(a);
        }
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
            std::clog << "Downloading " << fileId << " " << filename.GetFullPath().ToStdString() << std::endl;
            downloadFile(fileId, filename.GetFullPath().ToStdString());
            std::clog << "Downloaded" << std::endl;
            UTI::setFileModifiedTime(filename.GetFullPath().ToStdString(), UTI::stringToTime(remoteFile.modifiedTime));
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

    {
        wxCommandEvent* event = new wxCommandEvent(SYNC_MANAGER_UPDATED_DOCTOR);
        if (willUpdateDoctor && doctorUpdatedHandler != nullptr) {
            doctorUpdatedHandler->QueueEvent(event);
        }
    }

    {
        wxCommandEvent* event = new wxCommandEvent(SYNC_MANAGER_UPDATED_AMK);
        if (willUpdateAMK && amkUpdatedHandler != nullptr) {
            amkUpdatedHandler->QueueEvent(event);
        }
    }

    // Sync patients
    std::set<std::string> patientsToCreate; // uid
    std::map<std::string, GoogleAPITypes::RemoteFile> patientsToUpdate; // uid : remote file
    std::map<std::string, GoogleAPITypes::RemoteFile> patientsToDownload; // uid : remote file
    std::set<std::string> localPatientsToDelete; // uid
    std::map<std::string, GoogleAPITypes::RemoteFile> remotePatientsToDelete; // uid : remote

    bool willUpdatePatients = !patientsToDownload.empty() || !localPatientsToDelete.empty();

    std::map<std::string, time_t> localPatientTimestamps = getLocalPatientTimestamps();
    {
        for (const auto entry : localPatientTimestamps) {
            patientsToCreate.insert(entry.first);
        }
        for (auto const& entry : patientVersions) {
            patientsToCreate.erase(entry.first);
        }
        for (auto const& entry : remotePatientsMap) {
            patientsToCreate.erase(entry.first);
        }
    }

    for (const auto& entry : patientVersions) {
        std::string uid = entry.first;
        bool localHasPatient = localPatientTimestamps.find(uid) != localPatientTimestamps.end();
        bool remoteHasPatient = remotePatientsMap.find(uid) != remotePatientsMap.end();

        if (localHasPatient && remoteHasPatient) {
            long localVersion = patientVersions[uid];
            time_t localTimestamp = localPatientTimestamps[uid];
            GoogleAPITypes::RemoteFile remoteFile = remotePatientsMap[uid];
            long remoteVersion = 0;
            try {
                remoteVersion = std::stol(remoteFile.version);
            } catch(...){}

            if (remoteVersion == localVersion) {
                auto localModified = std::chrono::system_clock::from_time_t(localTimestamp);
                auto remoteModified = UTI::stringToTime(remoteFile.modifiedTime);

                if (localModified > remoteModified) {
                    patientsToUpdate[uid] = remoteFile;
                }
            } else if (remoteVersion > localVersion) {
                patientsToDownload[uid] = remoteFile;
            }
        }
        if (!localHasPatient && remoteHasPatient) {
            remotePatientsToDelete[uid] = remotePatientsMap[uid];
        }
        if (localHasPatient && !remoteHasPatient) {
            localPatientsToDelete.insert(uid);
        }
    }

    for (const auto entry : remotePatientsMap) {
        std::string uid = entry.first;
        if (patientVersions.find(uid) != patientVersions.end()) {
            // We already handled this in the above loop
            continue;
        }
        if (localPatientTimestamps.find(uid) != localPatientTimestamps.end()) {
            // File exist both on server and local, but has no local version
            time_t timestamp = localPatientTimestamps[uid];
            GoogleAPITypes::RemoteFile remoteFile = remotePatientsMap[uid];
            auto localModified =  std::chrono::system_clock::from_time_t(timestamp);
            auto localModifiedStr = UTI::timeToString(localModified);
            auto remoteModified = UTI::stringToTime(remoteFile.modifiedTime);
            if (localModified > remoteModified) {
                patientsToUpdate[uid] = remoteFile;
            } else if (remoteModified > localModified) {
                patientsToDownload[uid] = remoteFile;
            }
        } else {
            GoogleAPITypes::RemoteFile remoteFile = remotePatientsMap[uid];
            patientsToDownload[uid] = remoteFile;
        }
    }

    std::clog << "patientsToCreate: " << std::endl;
    debugLogStringSet(patientsToCreate);
    std::clog << "localPatientsToDelete: " << std::endl;
    debugLogStringSet(localPatientsToDelete);
    std::clog << "patientsToUpdate: " << std::endl;
    debugLogRemoteMap(patientsToUpdate);
    std::clog << "patientsToDownload: " << std::endl;
    debugLogRemoteMap(patientsToDownload);
    std::clog << "remotePatientsToDelete: " << std::endl;
    debugLogRemoteMap(remotePatientsToDelete);

    // createPatientFolder
    {
        if (remoteFilesMap.find("patients") == remoteFilesMap.end()) {
            auto remoteFile = createFolder("patients", {});
            remoteFilesMap["patients"] = remoteFile;
        }
    }

    // createPatients
    {
        // Drive driveService = getDriveService();
        // if (driveService == null) return;
        auto patientRemoteFolder = remoteFilesMap["patients"];
        if (remoteFilesMap.find("patients") == remoteFilesMap.end()) {
            std::cout << "Cannot find patients folder" << std::endl;
        } else {
            GoogleAPITypes::RemoteFile patientFolder = remoteFilesMap["patients"];
        }
        
        std::vector<Patient*> patients = PatientDBAdapter::sharedInstance()->getPatientsWithUniqueIDs(patientsToCreate);
        for (Patient* patient : patients) {
            std::clog << "Creating patient: " << patient->uniqueId.ToStdString() << std::endl;
            GoogleAPITypes::RemoteFile remoteFile = createFileWithMetadata(
                patient->uniqueId.ToStdString(), // filename
                patientToStringMap(patient),
                UTI::patientStringToTime(patient->timestamp.ToStdString()),
                {patientRemoteFolder.id}
            );
            std::clog << "Created" << std::endl;
            patientVersions[patient->uniqueId.ToStdString()] = std::stol(remoteFile.version);
        }
    }

    // updatePatients
    {
        std::set<std::string> patientsIDToUpdate;
        for (auto entry : patientsToUpdate) {
            patientsIDToUpdate.insert(entry.first);
        }

        std::vector<Patient*> patients = PatientDBAdapter::sharedInstance()->getPatientsWithUniqueIDs(patientsIDToUpdate);
        for (Patient *patient : patients) {
            GoogleAPITypes::RemoteFile remoteFile = patientsToUpdate[patient->uniqueId.ToStdString()];

            std::clog << "Updating patient " << patient->uniqueId.ToStdString() << std::endl;
            GoogleAPITypes::RemoteFile updatedFile = updateFileWithMetadata(
                remoteFile.id,
                patientToStringMap(patient),
                UTI::patientStringToTime(patient->timestamp.ToStdString()),
                {}
            );
            std::clog << "Updated" << std::endl;
            patientVersions[patient->uniqueId.ToStdString()] = std::stol(updatedFile.version);
        }
    }

    // deletePatients
    {
        for (auto entry : remotePatientsToDelete) {
            std::string uid = entry.first;
            GoogleAPITypes::RemoteFile remoteFile = entry.second;

            std::clog << "Deleting patient: " << uid << std::endl;
            deleteFile(remoteFile.id);
            std::clog << "Deleted" << std::endl;
        }
    }

    // download remote patients
    {
        for (auto entry : patientsToDownload) {
            std::string uid = entry.first;
            GoogleAPITypes::RemoteFile remoteFile = entry.second;
            Patient *patient = stringMapToPatient(remoteFile.properties);
            std::clog << "Downloading patient " << uid << std::endl;
            PatientDBAdapter::sharedInstance()->upsertEntry(patient);
            patientVersions[uid] = std::stol(remoteFile.version);
        }
    }

    // deleteLocalPatients
    {
        for (std::string uid : localPatientsToDelete) {
            Patient *existing = PatientDBAdapter::sharedInstance()->getPatientWithUniqueID(uid);
            PatientDBAdapter::sharedInstance()->deleteEntry(existing);
            patientVersions.erase(uid);
        }
    }

    auto versionMap = remoteFilesToVersionMap(remoteFilesMap);
    for (auto entry : patientVersions) {
        std::string patientUid = entry.first;
        long version = entry.second;
        std::string key = wxFileName("patient", patientUid).GetFullPath().ToStdString();
        versionMap[key] = version;
    }

    {
        wxCommandEvent* event = new wxCommandEvent(SYNC_MANAGER_UPDATED_PATIENT);
        if (willUpdatePatients && patientUpdatedHandler != nullptr) {
            patientUpdatedHandler->QueueEvent(event);
        }
    }

    try {
        std::clog << "Saving versions" << std::endl;
        saveLocalFileVersionMap(versionMap);
        std::clog << "Saved versions, sync complete" << std::endl;
    } catch (const std::exception& e) {
        std::clog << "Error: " << e.what() << std::endl;
    }
}
