//  GoogleSyncManager.hpp
//  AmiKo-wx
//
//  Created by b123400 on 20 Nov 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <wx/wx.h>
#include <wx/string.h>
#include "wx/preferences.h"

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
private:
    static GoogleSyncManager* m_pInstance;
};
