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
#include <wx/dir.h>
#include "wx/fs_zip.h"
#include <wx/log.h>
#include <wx/zipstrm.h>

#include "main.hpp"
#include "DefaultsController.hpp"
#include "gui/PreferencesWindow.hpp"

// IMPLEMENT_APP(MyApp)
IMPLEMENT_APP_NO_MAIN(MyApp);
IMPLEMENT_WX_THEME_SUPPORT;

int main(int argc, char *argv[])
{
    // Suppress the init error message about web-extensions #134
    // https://github.com/wxWidgets/Phoenix/issues/1640
    wxLogNull x;
    wxEntryStart( argc, argv );
    wxTheApp->CallOnInit();
    wxTheApp->OnRun();

    return 0;
}

enum
{ 
    Splash_Reset = wxID_HIGHEST,
    Splash_Timer,
};

BEGIN_EVENT_TABLE(MyApp, wxApp)
EVT_MENU(wxID_PREFERENCES, MyApp::OnPrefs)
EVT_TIMER(Splash_Timer, MyApp::OnTimer)
END_EVENT_TABLE()

wxConfigBase *MyAppTraits::CreateConfig () {
    return new DefaultsController();
}

bool MyApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;
    
    wxInitAllImageHandlers();

#if 1
    wxLanguage initialLanguage;

    if (wxString(APP_NAME) == "CoMed")
        initialLanguage = wxLANGUAGE_FRENCH_SWISS;
    else
        initialLanguage = wxLANGUAGE_GERMAN_SWISS;

    DefaultsController* defaults = DefaultsController::Instance();
    m_lang = (wxLanguage)defaults->getInt("language", initialLanguage);

    if (!m_locale.Init(m_lang, wxLOCALE_DONT_LOAD_DEFAULT) )
    {
#ifndef NDEBUG
        wxLogWarning(_("This language is not supported by the system."));
        // continue nevertheless
#endif
    }

    const wxLanguageInfo* pInfo = wxLocale::GetLanguageInfo(m_lang);
    wxLocale::AddCatalogLookupPathPrefix(wxStandardPaths::Get().GetLocalizedResourcesDir(pInfo->CanonicalName, wxStandardPathsBase::ResourceCat_Messages));
    // Initialize the catalogs we'll be using
    if (m_lang != wxLANGUAGE_ENGLISH &&
        !m_locale.AddCatalog("amiko"))
    {
        wxLogError(_("Couldn't find/load the 'amiko' catalog for locale '%s'."),
                   pInfo ? pInfo->GetLocaleName() : _("unknown"));
    }

#if 0 // issue #42
    // Now try to add wxstd.mo so that loading "NOTEXIST.ING" file will produce
    // a localized error message:
    m_locale.AddCatalog("wxstd");
    // NOTE: it's not an error if we couldn't find it!
#endif

    // This catalog is installed in standard location on Linux systems and
    // shows that you may make use of the standard message catalogs as well
    //
    // if it's not installed on your system, it is just silently ignored
#ifdef __linux__
    {
        wxLogNull noLog;
        m_locale.AddCatalog("fileutils");
    }
#endif
#endif

    SplashWindow* splash = new SplashWindow(nullptr);
    m_splash = splash;
    splash->Show();
    SetTopWindow( splash );

    wxString dir = wxStandardPaths::Get().GetUserDataDir();
    if (!wxDir::Exists(dir))
        wxFileName::Mkdir(dir);
    
    wxFileSystem::AddHandler(new wxZipFSHandler);

    static const int INTERVAL = 1000; // milliseconds
    m_timer = new wxTimer(this, Splash_Timer);
    // m_timer->Start(INTERVAL);
    m_timer->StartOnce(INTERVAL);

    return true;
}

void MyApp::OnTimer(wxTimerEvent& event) {
    MainWindow* frame = new MainWindow(nullptr);
    this->m_window = frame;
    frame->Show();
    this->SetTopWindow( frame );
    this->m_splash->Close(true);
    this->m_splash->Destroy();
    this->m_splash = nullptr;
}

void MyApp::OnPrefs(wxCommandEvent& evt)
{
    auto *p = new PreferencesWindow(this->m_window);
    p->ShowWindowModal();
}

int MyApp::OnExit()
{
#if 0
    delete wxConfig::Set(nullptr);
#endif
    return EXIT_SUCCESS;
}
