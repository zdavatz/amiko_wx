//  GoogleSyncManager.cpp
//  AmiKo-wx
//
//  Created by b123400 on 20 Nov 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include "GoogleSyncManager.hpp"
#include "GoogleConfig.h"
#include <curl/curl.h>

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

    std::clog << s << std::endl;
    curl_easy_cleanup(curl);

    // Example response
    // {
    //   "access_token": "...",
    //   "expires_in": 3599,
    //   "refresh_token": "...",
    //   "scope": "...",
    //   "token_type": "Bearer",
    //   "id_token": "..."
    // }
}