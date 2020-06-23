//  PatientSheetController.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 23 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <iostream>
#include "PatientSheetController.hpp"

PatientSheetController::PatientSheetController()
: mSelectedPatient(nullptr)
{
    
}

// 261
Patient *PatientSheetController::retrievePatient()
{
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;

    return mSelectedPatient;
}
