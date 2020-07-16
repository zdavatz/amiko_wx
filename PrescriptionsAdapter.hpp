//  PrescriptionsAdapter.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 23 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

class Patient;

class PrescriptionsAdapter
{
public:
    PrescriptionsAdapter();
    virtual ~PrescriptionsAdapter() {}
    
    wxArrayString listOfPrescriptionURLsForPatient(Patient *p);
    wxArrayString listOfPrescriptionsForPatient(Patient *p);
    
    // 33
    wxString placeDate;
};
