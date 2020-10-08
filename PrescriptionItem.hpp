//  PrescriptionItem.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 10 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <wx/wx.h>
#include <string>

#include "Medication.hpp"

#define KEY_AMK_PRESC_PLACE_DATE        "place_date"
#define KEY_AMK_PRESC_PLACE_DATE_OLD    "date"
#define KEY_AMK_PRESC_HASH              "prescription_hash"
#define KEY_AMK_PRESC_PAT               "patient"
#define KEY_AMK_PRESC_DOC               "operator"
#define KEY_AMK_PRESC_MEDS              "medications"

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
