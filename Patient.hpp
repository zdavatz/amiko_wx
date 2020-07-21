//  Patient.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 24 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <map>
#include <string>

#include <wx/wx.h>

#define KEY_AMK_PAT_ID          "patient_id"
#define KEY_AMK_PAT_NAME        "given_name"
#define KEY_AMK_PAT_SURNAME     "family_name"
#define KEY_AMK_PAT_BIRTHDATE   "birth_date"
#define KEY_AMK_PAT_WEIGHT      "weight_kg"
#define KEY_AMK_PAT_HEIGHT      "height_cm"
#define KEY_AMK_PAT_GENDER      "gender"
#define KEY_AMK_PAT_ADDRESS     "postal_address"
#define KEY_AMK_PAT_ZIP         "zip_code"
#define KEY_AMK_PAT_CITY        "city"
#define KEY_AMK_PAT_COUNTRY     "country"
#define KEY_AMK_PAT_PHONE       "phone_number"
#define KEY_AMK_PAT_EMAIL       "email_address"

#define PAT_DICT            std::map<std::string, std::string>

class Patient
{
public:
    Patient();
    virtual ~Patient() {}
    
    void importFromDict(PAT_DICT &dict);
    
    // 46
    long rowId;
    wxString uniqueId;
    wxString familyName;
    wxString givenName;
    wxString birthDate;
    wxString gender;
    int weightKg;
    int heightCm;
    wxString zipCode;
    wxString city;
    wxString country;
    wxString postalAddress;
    wxString phoneNumber;
    wxString emailAddress;
};
