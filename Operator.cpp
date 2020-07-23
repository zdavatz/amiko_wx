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
