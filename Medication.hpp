//  Medication.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 18 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <map>
#include <wx/wx.h>

class Medication
{
public:
    Medication();
    virtual ~Medication() {}
    
    wxArrayString listOfSectionIds();
    wxArrayString listOfSectionTitles();
    const wxString &shortTitle(wxString &longTitle);
    std::map<wxString, wxString> indexToTitlesDict();

    long medId; // pk in DB file
    char *title;
    char *auth;
    char *atccode;
    char *substances;
    char *regnrs;
    char *atcClass;
    char *therapy;
    char *application;
    char *indications;
    int customerId; // sqlite 3 returns type SQLITE_TEXT if the cell is empty
    char *packInfo;
    char *addInfo;
    char *sectionIds;
    char *sectionTitles;
    char *styleStr;
    char *contentStr;
    char *packages;
};
