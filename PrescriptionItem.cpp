//  PrescriptionItem.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 10 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include "PrescriptionItem.hpp"

PrescriptionItem::PrescriptionItem()
{
    
}

// 37
void PrescriptionItem::importFromDict(MED_DICT &dict)
{
    title           = dict[KEY_AMK_MED_PROD_NAME];
    fullPackageInfo = dict[KEY_AMK_MED_PACKAGE];
    eanCode         = dict[KEY_AMK_MED_EAN];
    comment         = dict[KEY_AMK_MED_COMMENT];
    
#if 0 //ndef NDEBUG
    std::clog << __PRETTY_FUNCTION__
    << ", title: " << title
    << ", fullPackageInfo: " << fullPackageInfo
    << ", eanCode: " << eanCode
    << ", comment: " << comment
    << std::endl;
#endif
}
