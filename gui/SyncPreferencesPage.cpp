//  SyncPreferencesPage.cpp
//  AmiKo-wx
//
//  Created by b123400 on 19 Nov 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include "SyncPreferencesPage.hpp"
#include "wx/checkbox.h"
#include "wx/sizer.h"

SyncPreferencesPage::SyncPreferencesPage() : wxStockPreferencesPage(Kind_General) {

}

wxWindow *SyncPreferencesPage::CreateWindow(wxWindow *parent)
{
    return new SyncPreferencesPagePanel(parent);
}

SyncPreferencesPagePanel::SyncPreferencesPagePanel(wxWindow *parent) : wxPanel(parent)
{
    wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    wxCheckBox *syncCheckbox = new wxCheckBox(this, wxID_ANY, _("Sync with Google Drive"));

    sizer->Add(200, 15);
    sizer->Add(syncCheckbox, wxSizerFlags().Border());
    sizer->Add(200, 15);    SetSizer(sizer);
}