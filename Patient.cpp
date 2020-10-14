//  Patient.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 24 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <iostream>

#include "Patient.hpp"
#include "Utilities.hpp"

Patient::Patient()
: weightKg(0)
, heightCm(0)
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
        std::clog << "WARNING: imported ID:\n\t" << uniqueId
        << "\nexpected ID:\n\t" << newUniqueID
        << std::endl;
    }
}

// 70
// The UUID should be unique and should be based on family name, given name, and birthday
// https://github.com/zdavatz/amiko_csharp/issues/81#issue-330573485
wxString Patient::generateUniqueID()
{
    wxArrayString dateFields = wxSplit(birthDate, '.');
    
    wxString birthDateSanitized = birthDate;
    if (dateFields.size() == 3) {
        // Strip possible leading zeros from day and month
        int dd   = wxAtoi(dateFields[0]);
        int mm   = wxAtoi(dateFields[1]);
        int yyyy = wxAtoi(dateFields[2]);
        birthDateSanitized.Printf("%d.%d.%d", dd, mm, yyyy);
    }

    wxString uniqueHashString = wxString::Format("%s.%s.%s",
                                                 familyName,
                                                 givenName,
                                                 birthDateSanitized);
    uniqueHashString.LowerCase();
    return UTI::sha256(uniqueHashString);
}

// 77
wxString Patient::asString()
{
    return wxString::Format("%s %s\n%s\nCH-%s %s\n%s\n%s", givenName, familyName, postalAddress, zipCode, city, phoneNumber, emailAddress);
}

wxString Patient::description()
{
    return wxString::Format("Patient givenName:%s, familyName:%s, birthDate:%s, uniqueId:%s", givenName, familyName, birthDate, uniqueId);
}

