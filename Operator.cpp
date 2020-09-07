//  Operator.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 17 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <iostream>

#include "Operator.hpp"

Operator::Operator()
{
    
}

// 38
wxString Operator::retrieveOperatorAsString()
{
#if 0
    if (title.length() == 0)
        title = wxEmptyString;
    if (postalAddress.length() == 0)
        postalAddress = wxEmptyString;
    if (zipCode.length() == 0)
        zipCode = wxEmptyString;
    if (city.length() == 0)
        city = wxEmptyString;
    if (phoneNumber.length() == 0)
        phoneNumber = wxEmptyString;
    if (emailAddress.length() == 0)
        emailAddress = wxEmptyString;
    if (givenName.length() == 0)
        givenName = wxEmptyString;
#endif
    
    if (familyName.length() > 0) {
        return wxString::Format("%s %s %s\n%s\n%s %s\n%s\n%s",
                title, givenName, familyName, postalAddress, zipCode, city, phoneNumber, emailAddress);
    }

    return "...";
}

// 63
void Operator::importFromDict(OPER_DICT &dict)
{
    title =         dict[KEY_AMK_DOC_TITLE];
    familyName =    dict[KEY_AMK_DOC_SURNAME];
    givenName =     dict[KEY_AMK_DOC_NAME];
    postalAddress = dict[KEY_AMK_DOC_ADDRESS];
    zipCode =       dict[KEY_AMK_DOC_ZIP];
    city =          dict[KEY_AMK_DOC_CITY];
    //country = "";
    phoneNumber =   dict[KEY_AMK_DOC_PHONE];
    emailAddress =  dict[KEY_AMK_DOC_EMAIL];
}
