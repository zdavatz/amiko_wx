//  SyncPreferencesPage.hpp
//  AmiKo-wx
//
//  Created by b123400 on 19 Nov 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <wx/wx.h>
#include "wx/preferences.h"

class SyncPreferencesPage : public wxStockPreferencesPage
{
public:
    SyncPreferencesPage();
    virtual wxWindow *CreateWindow(wxWindow *parent);
    virtual ~SyncPreferencesPage() {}
};

class SyncPreferencesPagePanel : public wxPanel
{
public:
    SyncPreferencesPagePanel(wxWindow *parent);
    ~SyncPreferencesPagePanel() {};
};