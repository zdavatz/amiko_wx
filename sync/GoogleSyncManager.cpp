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

    std::clog << s << std::endl;
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

    std::clog << "a: " << accessToken << std::endl;
    std::clog << "r: " << refreshToken << std::endl;

    auto now = std::chrono::system_clock::now();
    auto expire = now + std::chrono::seconds(expires);
    std::time_t expireT = std::chrono::system_clock::to_time_t(expire);

    std::ostringstream ss;
    ss << std::put_time(gmtime(&expireT), "%FT%TZ");
    std::string expireIsoString = ss.str();

    std::clog << "expire: " << expireIsoString << std::endl;

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