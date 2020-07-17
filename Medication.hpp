//  Medication.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 18 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <map>
#include <wx/wx.h>

#define KEY_AMK_MED_TITLE           "title"
#define KEY_AMK_MED_OWNER           "owner"
#define KEY_AMK_MED_REG_N           "regnrs"
#define KEY_AMK_MED_ATC             "atccode"

// Used in MLPrescriptionItem
#define KEY_AMK_MED_PROD_NAME       "product_name"
#define KEY_AMK_MED_PACKAGE         "package"
#define KEY_AMK_MED_EAN             "eancode"
#define KEY_AMK_MED_COMMENT         "comment"

#define MED_DICT            std::map<std::string, std::string>

class Medication
{
public:
    Medication();
    virtual ~Medication() {}
    
    wxArrayString listOfSectionIds();
    wxArrayString listOfSectionTitles();
    const wxString &shortTitle(wxString &longTitle);
    std::map<wxString, wxString> indexToTitlesDict();
    void importFromDict(MED_DICT &dict);

    long medId; // pk in DB file
    std::string title;
    std::string auth;
    std::string atccode;
    char *substances;
    std::string regnrs;
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
