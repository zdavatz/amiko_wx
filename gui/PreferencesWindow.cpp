//  PreferencesWindow.cpp
//  AmiKo-wx
//
//  Created by b123400 on 19 Nov 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include "PreferencesWindow.hpp"
#include "../sync/GoogleSyncManager.hpp"
#include <wx/app.h>
#include "wx/checkbox.h"
#include "wx/sizer.h"
#include "wx/frame.h"
#include <wx/preferences.h>
#include "config.h"
#include <curl/curl.h>
#include "main.hpp"

size_t writefuncPref(void *ptr, size_t size, size_t nmemb, std::string *s) 
{
  s->append(static_cast<char *>(ptr), size*nmemb);
  return size*nmemb;
}

PreferencesWindow::PreferencesWindow(wxWindow *parent)
: SyncPreferencesBase(parent, -1, _("Preferences"))
, googleAuthSheet(nullptr)
{
    GoogleSyncManager *g = GoogleSyncManager::Instance();
    syncCheckbox->SetValue(g->isGoogleLoggedIn());

    mUpdateButton->Enable(false);
    mUpdateButton->SetLabel(_("Checking for update"));
    
    wxGetApp().CallAfter([=]{
        CheckForUpdate();
    });
}

void PreferencesWindow::OnCheckboxClick(wxCommandEvent& event)
{
    GoogleSyncManager *g = GoogleSyncManager::Instance();
    if (!g->isGoogleLoggedIn()) {
        if (!googleAuthSheet) {
            googleAuthSheet = new GoogleAuthSheet(this);
        } else {
            googleAuthSheet->LoadAuthURL();
        }
        googleAuthSheet->ShowModal();
        syncCheckbox->SetValue(g->isGoogleLoggedIn());
    } else {
        g->logout();
    }
}

void PreferencesWindow::OnCloseClicked( wxCommandEvent& event ) {
    EndModal(wxID_OK);
}

void PreferencesWindow::CheckForUpdate() {
    CURL *curl = curl_easy_init();
    std::string s;
    curl_easy_setopt(curl, CURLOPT_URL, "https://github.com/zdavatz/amiko_wx/releases/latest");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefuncPref);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    wxArrayString parts = wxSplit(wxString(s), '"');

    if (parts.GetCount() < 2) return;
    auto url = parts[1];

    wxArrayString urlParts = wxSplit(url, '/');
    if (urlParts.GetCount() < 1) return;
    auto last = urlParts.Last();

    if (last != PROJECT_VER) {
        mUpdateButton->Enable(true);
        mUpdateButton->SetLabel(_("Update"));
    } else {
        mUpdateButton->Enable(false);
        mUpdateButton->SetLabel(_("Already up-to-date"));
    }
}

void PreferencesWindow::OnUpdateClick(wxCommandEvent& event) {
    wxLaunchDefaultBrowser("https://github.com/zdavatz/amiko_wx/releases/latest");
}
