//  PrescriptionItem.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 10 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <wx/wx.h>
#include <string>

#include "Medication.hpp"

class PrescriptionItem
{
public:
    PrescriptionItem();
    virtual ~PrescriptionItem() {}
    
    void importFromDict(MED_DICT &dict);
    
    // 30
    long mid;

    // 31
    std::string eanCode;

    // 33
    std::string fullPackageInfo;
    std::string title;

    // 37
    std::string price;
    std::string comment;
    Medication *med;
};
