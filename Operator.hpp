//  Operator.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 17 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <map>
#include <string>

#include <wx/wx.h>

#define DOC_JSON_TITLE      "title"
#define DOC_JSON_NAME       "name"
#define DOC_JSON_SURNAME    "surname"
#define DOC_JSON_ADDRESS    "street"
#define DOC_JSON_CITY       "city"
#define DOC_JSON_ZIP        "zip"
#define DOC_JSON_PHONE      "phone"
#define DOC_JSON_EMAIL      "email"
#define DOC_JSON_COUNTRY    "country"

// 36
#define KEY_AMK_DOC_TITLE       "title"
#define KEY_AMK_DOC_NAME        "given_name"
#define KEY_AMK_DOC_SURNAME     "family_name"
#define KEY_AMK_DOC_ADDRESS     "postal_address"
#define KEY_AMK_DOC_CITY        "city"
#define KEY_AMK_DOC_ZIP         "zip_code"
#define KEY_AMK_DOC_PHONE       "phone_number"
#define KEY_AMK_DOC_EMAIL       "email_address"

// 45
#define KEY_AMK_DOC_SIGNATURE   "signature"
#define DOC_JSON_FILENAME       "doctor.json"
#define DOC_SIGNATURE_FILENAME  "op_signature.png"

#define OPER_DICT            std::map<std::string, std::string>

class Operator
{
public:
    Operator();
    virtual ~Operator() {}
    
    void importFromDict(OPER_DICT &dict);
    wxString retrieveOperatorAsString();
    
    // 50
    wxString title;
    wxString familyName;
    wxString givenName;
    wxString postalAddress;
    wxString zipCode;
    wxString city;
    wxString country;
    wxString phoneNumber;
    wxString emailAddress;
};
