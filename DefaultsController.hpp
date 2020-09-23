//  DefaultsController.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 7 Sep 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <wx/config.h>
#include "config.h"

class DefaultsController : public wxConfig {
public:
    static DefaultsController * Instance();
    virtual ~DefaultsController() {}
    
    void setBool(bool value, const wxString & key);
    bool getBool(const wxString & key, bool defVal);

    void setInt(int value, const wxString & key);
    int getInt(const wxString & key, int defVal);

    void setString(const wxString& value, const wxString& key);
    wxString getString(const wxString& key, const char* defVal);

private:
    // private constructor so that it can not be called

    // INI file is called:
    //  macos: "~/Library/Preferences/$APP_NAME"
    //  linux: "~/.$APP_NAME"
    DefaultsController() : wxConfig(PROJECT_NAME, "ywesee", APP_NAME) {}

    static DefaultsController* m_pInstance;
};
