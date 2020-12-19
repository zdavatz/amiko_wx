//  GoogleSyncManager.hpp
//  AmiKo-wx
//
//  Created by b123400 on 20 Nov 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <set>
#include <chrono>
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
        std::string size;
        std::map<std::string, std::string> properties;
    };
    void to_json(nlohmann::json& j, const RemoteFile& f);

    void from_json(const nlohmann::json& j, RemoteFile& f);
};

wxDECLARE_EVENT(SYNC_MANAGER_UPDATED_PATIENT, wxCommandEvent);

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
    
    void deleteFile(std::string fileId);
    GoogleAPITypes::RemoteFile createFolder(std::string name, std::vector<std::string> parents);

    GoogleAPITypes::RemoteFile uploadFile(std::string name, std::string filePath, std::string mimeType, std::vector<std::string> parents);
    GoogleAPITypes::RemoteFile updateFile(std::string fileId, std::string filePath);
    void downloadFile(std::string fileId, std::string path);

    std::vector<GoogleAPITypes::RemoteFile> listRemoteFilesAndFolders(std::string pageToken = "");
    GoogleAPITypes::RemoteFile createFileWithMetadata(
        std::string name,
        std::map<std::string, std::string> appProperties,
        std::chrono::time_point<std::chrono::system_clock> modifiedTime,
        std::vector<std::string> parents
    );
    GoogleAPITypes::RemoteFile updateFileWithMetadata(
        std::string fileId, 
        std::map<std::string, std::string> properties,
        std::chrono::time_point<std::chrono::system_clock> modifiedTime,
        std::vector<std::string> parents
    );

    wxEvtHandler *patientUpdatedHandler = nullptr;

private:
    static GoogleSyncManager* m_pInstance;
    std::string getAccessToken();
    
    std::set<std::string> listLocalFilesAndFolders(wxString path = UTI::documentsDirectory());

    bool shouldSyncLocalFile(wxFileName path);
};
