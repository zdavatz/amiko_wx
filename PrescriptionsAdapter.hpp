//  PrescriptionsAdapter.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 23 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <string>

class Patient;
class PrescriptionItem;
class Operator;

class PrescriptionsAdapter
{
public:
    PrescriptionsAdapter();
    virtual ~PrescriptionsAdapter() {}
    
    wxArrayString listOfPrescriptionURLsForPatient(Patient *p);
    wxArrayString listOfPrescriptionsForPatient(Patient *p);
    wxString loadPrescriptionFromFile(wxString filePath);
    
    // 30
    std::vector<PrescriptionItem *> cart;
    Patient *patient;
    Operator *doctor;

    // 33
    std::string placeDate;
    
    // .m 31
    wxString currentFileName;
};
