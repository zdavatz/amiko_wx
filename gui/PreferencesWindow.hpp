//  PreferencesWindow.hpp
//  AmiKo-wx
//
//  Created by b123400 on 19 Nov 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <wx/wx.h>
#include <wx/event.h>
#include "GoogleAuthSheet.h"

class PreferencesWindow : public SyncPreferencesBase
{
public:
    PreferencesWindow(wxWindow *parent);
    ~PreferencesWindow() {};
private:
    void OnCheckboxClick(wxCommandEvent& event);
    void OnCloseClicked( wxCommandEvent& event );
    void OnUpdateClick( wxCommandEvent& event );
    void CheckForUpdate();

    GoogleAuthSheet *googleAuthSheet;
};

