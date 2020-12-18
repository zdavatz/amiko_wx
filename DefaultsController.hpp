//  DefaultsController.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 7 Sep 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <wx/config.h>
#include "config.h"

#ifdef __linux__
#define PROJECT_BUNDLE_ID   "com.ywesee." APP_NAME "linux"
#else
#define PROJECT_BUNDLE_ID   "com.ywesee." APP_NAME "osx"
#endif

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
    DefaultsController()
    : wxConfig(APP_NAME, PROJECT_BUNDLE_ID
#ifdef __linux__
               , "." APP_NAME "/" APP_NAME ".conf"
#endif
               )
    { }

    static DefaultsController* m_pInstance;
};
