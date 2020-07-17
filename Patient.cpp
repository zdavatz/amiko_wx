//  Patient.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 24 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <iostream>

#include "Patient.hpp"

Patient::Patient()
{
    
}

// 44
void Patient::importFromDict(PAT_DICT &dict)
{
#if 1
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
#else
    uniqueId =      [dict objectForKey: KEY_AMK_PAT_ID];
    familyName =    [dict objectForKey: KEY_AMK_PAT_SURNAME];
    givenName =     [dict objectForKey: KEY_AMK_PAT_NAME];
    birthDate =     [dict objectForKey: KEY_AMK_PAT_BIRTHDATE];
    weightKg =      [[dict objectForKey:KEY_AMK_PAT_WEIGHT] intValue];
    heightCm =      [[dict objectForKey:KEY_AMK_PAT_HEIGHT] intValue];
    gender =        [dict objectForKey: KEY_AMK_PAT_GENDER];
    postalAddress = [dict objectForKey: KEY_AMK_PAT_ADDRESS];
    zipCode =       [dict objectForKey: KEY_AMK_PAT_ZIP];
    city =          [dict objectForKey: KEY_AMK_PAT_CITY];
    country =       [dict objectForKey: KEY_AMK_PAT_COUNTRY];
    phoneNumber =   [dict objectForKey: KEY_AMK_PAT_PHONE];
    emailAddress =  [dict objectForKey: KEY_AMK_PAT_EMAIL];
    
    NSString *newUniqueID = [self generateUniqueID];
    
    if (!uniqueId.length) { // The ID was not defined from the dictionary
        uniqueId = newUniqueID; // assign it here
    }
    else if (![uniqueId isEqualToString:newUniqueID]) {
        NSLog(@"WARNING: imported ID:%@, expected ID %@", uniqueId, newUniqueID);
    }
#endif
}
