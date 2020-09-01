//  Patient.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 24 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <iostream>

#include "Patient.hpp"
#include "Utilities.hpp"

Patient::Patient()
{
    
}

// 44
void Patient::importFromDict(PAT_DICT &dict)
{
    uniqueId   = dict[ KEY_AMK_PAT_ID];
    familyName = dict[ KEY_AMK_PAT_SURNAME];
    givenName  = dict[ KEY_AMK_PAT_NAME];
    birthDate  = dict[ KEY_AMK_PAT_BIRTHDATE];

    weightKg   = wxAtoi(dict[KEY_AMK_PAT_WEIGHT]);
    heightCm   = wxAtoi(dict[KEY_AMK_PAT_HEIGHT]);

    gender       = dict [ KEY_AMK_PAT_GENDER];
    postalAddress = dict [ KEY_AMK_PAT_ADDRESS];
    zipCode      = dict [ KEY_AMK_PAT_ZIP];
    city         = dict [ KEY_AMK_PAT_CITY];
    country      = dict [ KEY_AMK_PAT_COUNTRY];
    phoneNumber  = dict [ KEY_AMK_PAT_PHONE];
    emailAddress =  dict [ KEY_AMK_PAT_EMAIL];
    
    wxString newUniqueID = generateUniqueID();
    
    if (uniqueId.length() == 0) { // The ID was not defined from the dictionary
        uniqueId = newUniqueID; // assign it here
    }
    else if (uniqueId != newUniqueID) {
        std::clog << "WARNING: imported ID:" << uniqueId << ", expected ID " << newUniqueID;
    }
}

// The UUID should be unique and should be based on family name, given name, and birthday
wxString Patient::generateUniqueID()
{
    wxString uniqueHashString = wxString::Format("%s.%s.%s", familyName , givenName, birthDate);
    uniqueHashString.LowerCase();
    return UTI::sha256(uniqueHashString);
}

