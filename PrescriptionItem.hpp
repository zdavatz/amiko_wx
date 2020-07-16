//  PrescriptionItem.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 10 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <wx/wx.h>

class Medication;

class PrescriptionItem
{
public:
    PrescriptionItem();
    virtual ~PrescriptionItem() {}
    
    // 30
    long mid;

    // 33
    wxString fullPackageInfo;

    // 38
    Medication *med;
};
