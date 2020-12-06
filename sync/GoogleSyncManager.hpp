//  GoogleSyncManager.hpp
//  AmiKo-wx
//
//  Created by b123400 on 20 Nov 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <set>
#include <wx/wx.h>
#include <wx/string.h>
#include "wx/preferences.h"
#include <nlohmann/json.hpp>
#include "Utilities.hpp"
#include <wx/filename.h>

namespace GoogleAPITypes {
    struct RemoteFile {
        std::string id;
        std::string name;
        std::string mimeType;
        std::vector<std::string> parents;
        std::string modifiedTime;
        std::string version;
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
    
    void sync();

private:
    static GoogleSyncManager* m_pInstance;
    std::string getAccessToken();
    std::vector<GoogleAPITypes::RemoteFile> listRemoteFilesAndFolders(std::string pageToken = "");
    std::set<std::string> listLocalFilesAndFolders(wxString path = UTI::documentsDirectory());

    std::string uploadFile(std::string name, std::string filePath, std::string mimeType, std::vector<std::string> parents);
    void downloadFile(std::string fileId, std::string path);
    void deleteFile(std::string fileId);
    bool shouldSyncLocalFile(wxFileName path);
};
