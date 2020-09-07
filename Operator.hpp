//  Operator.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 17 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <map>
#include <string>

#include <wx/wx.h>

// 26
#define DEFAULTS_DOC_TITLE      "title"
#define DEFAULTS_DOC_NAME       "givenname"
#define DEFAULTS_DOC_SURNAME    "familyname"
#define DEFAULTS_DOC_ADDRESS    "postaladdress"
#define DEFAULTS_DOC_CITY       "city"
#define DEFAULTS_DOC_ZIP        "zipcode"
#define DEFAULTS_DOC_PHONE      "phonenumber"
#define DEFAULTS_DOC_EMAIL      "emailaddress"
#define DEFAULTS_DOC_COUNTRY    "country"

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
