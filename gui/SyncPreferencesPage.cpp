//  SyncPreferencesPage.cpp
//  AmiKo-wx
//
//  Created by b123400 on 19 Nov 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include "SyncPreferencesPage.hpp"
#include "../sync/GoogleSyncManager.hpp"
#include "wx/checkbox.h"
#include "wx/sizer.h"

SyncPreferencesPage::SyncPreferencesPage() : wxStockPreferencesPage(Kind_General) {

}

wxWindow *SyncPreferencesPage::CreateWindow(wxWindow *parent)
{
    return new SyncPreferencesPagePanel(parent);
}

SyncPreferencesPagePanel::SyncPreferencesPagePanel(wxWindow *parent) : wxPanel(parent)
, googleAuthSheet(nullptr)
{
    wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    syncCheckbox = new wxCheckBox(this, wxID_ANY, _("Sync with Google Drive"));

    sizer->Add(200, 15);
    sizer->Add(syncCheckbox, wxSizerFlags().Border());
    sizer->Add(200, 15);    SetSizer(sizer);

    GoogleSyncManager *g = GoogleSyncManager::Instance();
    syncCheckbox->SetValue(g->isGoogleLoggedIn());
}

BEGIN_EVENT_TABLE(SyncPreferencesPagePanel, wxPanel)
    EVT_CHECKBOX(wxID_ANY, SyncPreferencesPagePanel::OnCheckboxClick)
END_EVENT_TABLE()

void SyncPreferencesPagePanel::OnCheckboxClick(wxCommandEvent& event)
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