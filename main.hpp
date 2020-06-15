//
//  main.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 12 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once
#include <wx/wx.h>
#include <wx/taskbar.h>

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
    virtual int OnExit();
    
protected:
    wxLanguage m_lang;  // language specified by user
    wxLocale m_locale;  // locale we'll be using
};

DECLARE_APP(MyApp)
