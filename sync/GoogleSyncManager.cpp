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
#include <sstream>

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
    std::time_t expireT = std::chrono::system_clock::to_time_t(expire);

    std::ostringstream ss;
    ss << std::put_time(gmtime(&expireT), "%FT%TZ");
    std::string expireIsoString = ss.str();

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
    std::string expireStdStr = expireStr.ToStdString();
    std::tm tm = {};
    std::stringstream ss(expireStdStr);
    ss >> std::get_time(&tm, "%FT%TZ");

    const time_t expiredInUTC =
    #if defined(_WIN32)
        _mkgmtime(&tm);
    #else // Assume POSIX
        timegm(&tm);
    #endif
    auto expire = std::chrono::system_clock::from_time_t(expiredInUTC);
    auto now = std::chrono::system_clock::now();

    std::time_t tempExpireT = std::chrono::system_clock::to_time_t(expire);
    std::ostringstream ssEx;
    ssEx << std::put_time(gmtime(&tempExpireT), "%FT%TZ");
    std::cout << "expire: " << ssEx.str() << std::endl;

    std::time_t nowT = std::chrono::system_clock::to_time_t(now);
    std::ostringstream ssNow;
    ssNow << std::put_time(gmtime(&nowT), "%FT%TZ");
    std::cout << "now: " << ssNow.str() << std::endl;
    

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

void GoogleSyncManager::uploadFile() {
    auto accessToken = this->getAccessToken();
    if (accessToken.empty()) {
        return;
    }

    CURL *curl = curl_easy_init();
    std::string s;
    curl_easy_setopt(curl, CURLOPT_URL, "https://www.googleapis.com/upload/drive/v3/files?uploadType=multipart");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

    struct curl_slist *chunk = NULL;
    chunk = curl_slist_append(chunk, ("Authorization: Bearer " + accessToken).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

    curl_mime *form = curl_mime_init(curl);
    curl_mimepart *field = NULL;

    field = curl_mime_addpart(form);
    curl_mime_name(field, "metadata");
    curl_mime_data(field, "{\"appProperties\":{\"foo\":\"bar\"}, \"name\":\"hk.png\",\"parents\":[\"appDataFolder\"]}", CURL_ZERO_TERMINATED);
    curl_mime_type(field, "application/json; charset=UTF-8");

    /* Fill in the file upload field */ 
    field = curl_mime_addpart(form);
    curl_mime_name(field, "file");
    curl_mime_filedata(field, "/Users/b123400/Downloads/Slice.png");
    curl_mime_type(field, "image/png");

    curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    std::clog << s << std::endl;
    // auto json = nlohmann::json::parse(s);
    // Example response
    // {
    //  "kind": "drive#file",
    //  "id": "1NvrC5PUAH_-AUmE6R_YrkS_M8KFNWLofWaFOeqIznFOq6iyFDA",
    //  "name": "hk.png",
    //  "mimeType": "image/png"
    // }
}

void GoogleSyncManager::fetchFileList(std::string pageToken) {
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

    auto accessToken = this->getAccessToken();
    if (accessToken.empty()) {
        return {};
    }

    std::string queryString = \
        "?pageSize=1000" \
        "&spaces=appDataFolder";

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
    // auto json = nlohmann::json::parse(s);
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
}
