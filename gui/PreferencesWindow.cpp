//  PreferencesWindow.cpp
//  AmiKo-wx
//
//  Created by b123400 on 19 Nov 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include "PreferencesWindow.hpp"
#include "../sync/GoogleSyncManager.hpp"
#include "wx/checkbox.h"
#include "wx/sizer.h"
#include "wx/frame.h"
#include <wx/preferences.h>

PreferencesWindow::PreferencesWindow(wxWindow *parent)
: SyncPreferencesBase(parent, -1, _("Preferences"))
, googleAuthSheet(nullptr)
{
    GoogleSyncManager *g = GoogleSyncManager::Instance();
    syncCheckbox->SetValue(g->isGoogleLoggedIn());
}

void PreferencesWindow::OnCheckboxClick(wxCommandEvent& event)
{
    GoogleSyncManager *g = GoogleSyncManager::Instance();
    if (!g->isGoogleLoggedIn()) {
#ifdef AMIKO_USE_COPY_OAUTH
            if (!googleAuthCopySheet) {
                googleAuthCopySheet = new GoogleAuthCopySheet(this);
            }
            googleAuthCopySheet->ShowModal();
#else
            if (!googleAuthSheet) {
                googleAuthSheet = new GoogleAuthSheet(this);
            } else {
                googleAuthSheet->LoadAuthURL();
            }
            googleAuthSheet->ShowModal();
#endif
        syncCheckbox->SetValue(g->isGoogleLoggedIn());
    } else {
        g->logout();
    }
}

void PreferencesWindow::OnCloseClicked( wxCommandEvent& event ) {
    EndModal(wxID_OK);
}
