//  main.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 12 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <stdio.h>
#include <iostream>

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#include <wx/stdpaths.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/intl.h>

#include "main.hpp"
#include "MainWindow.h"

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;
    
#if 1
    if (APP_NAME == "CoMed")
        m_lang = wxLANGUAGE_FRENCH;
    else
        m_lang = wxLANGUAGE_GERMAN;

    if ( !m_locale.Init(m_lang, wxLOCALE_DONT_LOAD_DEFAULT) ) {
        wxLogWarning(_("This language is not supported by the system."));
        // continue nevertheless
    }
    // normally this wouldn't be necessary as the catalog files would be found
    // in the default locations, but when the program is not installed the
    // catalogs are in the build directory where we wouldn't find them by
    // default
    wxLocale::AddCatalogLookupPathPrefix("./lang");
    // Initialize the catalogs we'll be using
    const wxLanguageInfo* pInfo = wxLocale::GetLanguageInfo(m_lang);
    if (!m_locale.AddCatalog("amiko"))
    {
        wxLogError(_("Couldn't find/load the 'amiko' catalog for locale '%s'."),
                   pInfo ? pInfo->GetLocaleName() : _("unknown"));
    }

    // Now try to add wxstd.mo so that loading "NOTEXIST.ING" file will produce
    // a localized error message:
    m_locale.AddCatalog("wxstd");
        // NOTE: it's not an error if we couldn't find it!

    // This catalog is installed in standard location on Linux systems and
    // shows that you may make use of the standard message catalogs as well
    //
    // if it's not installed on your system, it is just silently ignored
#ifdef __LINUX__
    {
        wxLogNull noLog;
        m_locale.AddCatalog("fileutils");
    }
#endif
#endif

    MainWindow* frame = new MainWindow(nullptr);

    frame->Show();
    SetTopWindow( frame );
    return true;
}

int MyApp::OnExit()
{
#if 0
    delete wxConfig::Set(NULL);
#endif
    return EXIT_SUCCESS;
}



