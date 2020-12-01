//  GoogleSyncManager.hpp
//  AmiKo-wx
//
//  Created by b123400 on 20 Nov 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <wx/wx.h>
#include <wx/string.h>
#include "wx/preferences.h"
#include <nlohmann/json.hpp>

namespace GoogleAPITypes {
    struct RemoteFile {
        std::string id;
        std::string name;
        std::string mimeType;
        std::vector<std::string> parents;
        std::string modifiedTime;
    };
    void to_json(nlohmann::json& j, const RemoteFile& f);

    void from_json(const nlohmann::json& j, RemoteFile& f);
};

class GoogleSyncManager
{
public:
    static GoogleSyncManager * Instance();

protected:
    GoogleSyncManager();
    ~GoogleSyncManager();

public:
    std::string authURL();

    void receivedAuthCode(std::string code);
    bool isGoogleLoggedIn();
    void logout();
    std::vector<GoogleAPITypes::RemoteFile> fetchFileList(std::string pageToken = "");

    std::string uploadFile(std::string name, std::string filePath, std::string mimeType, std::vector<std::string> parents);
    void deleteFile(std::string fileId);

private:
    static GoogleSyncManager* m_pInstance;
    std::string getAccessToken();
};
