//  GoogleSyncManager.cpp
//  AmiKo-wx
//
//  Created by b123400 on 20 Nov 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include "GoogleSyncManager.hpp"
#include "GoogleConfig.h"

GoogleSyncManager* GoogleSyncManager::m_pInstance;

// Singleton
GoogleSyncManager* GoogleSyncManager::Instance()
{
    if (!m_pInstance) {
        m_pInstance = new GoogleSyncManager();
    }

    return m_pInstance;
}

GoogleSyncManager::GoogleSyncManager() {

}

std::string GoogleSyncManager::authURL() {
    return "https://accounts.google.com/o/oauth2/v2/auth?scope=email%20profile%20https://www.googleapis.com/auth/drive.appdata&response_type=code&state=1&redirect_uri=urn%3Aietf%3Awg%3Aoauth%3A2.0%3Aoob%3Aauto&client_id=" GOOGLE_CLIENT_ID;
}

void GoogleSyncManager::receivedAuthCode(std::string code) {
    // TODO
}