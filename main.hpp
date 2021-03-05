//
//  main.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 12 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once
#include <wx/wx.h>
#include <wx/apptrait.h>
#include <wx/taskbar.h>
#include "MainWindow.h"

class MyAppTraits : public wxGUIAppTraits
{
public:
    wxConfigBase *CreateConfig ();
};

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
    virtual int OnExit();

    DECLARE_EVENT_TABLE()
    void OnPrefs(wxCommandEvent& evt);
    
protected:
    wxLanguage m_lang;  // language specified by user
    wxLocale m_locale;  // locale we'll be using
    MainWindow *m_window;

    virtual wxAppTraits *CreateTraits() { return new MyAppTraits; }
};

DECLARE_APP(MyApp)
