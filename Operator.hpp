//  Operator.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 17 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <map>
#include <string>

// 36
#define KEY_AMK_DOC_TITLE       "title"
#define KEY_AMK_DOC_NAME        "given_name"
#define KEY_AMK_DOC_SURNAME     "family_name"
#define KEY_AMK_DOC_ADDRESS     "postal_address"
#define KEY_AMK_DOC_CITY        "city"
#define KEY_AMK_DOC_ZIP         "zip_code"
#define KEY_AMK_DOC_PHONE       "phone_number"
#define KEY_AMK_DOC_EMAIL       "email_address"

#define OPER_DICT            std::map<std::string, std::string>

class Operator
{
public:
    Operator();
    virtual ~Operator() {}
    
    void importFromDict(OPER_DICT &dict);
};
