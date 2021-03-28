//  PrescriptionsAdapter.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 23 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <string>

#include <wx/url.h>
#include <wx/filename.h>

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
    wxURL savePrescriptionForPatient_withUniqueHash_andOverwrite(Patient *p, wxString hash, bool overwrite);
    void deletePrescriptionWithName_forPatient(wxString name, Patient *p);
    wxFileName amkPathForPatient(Patient *p);
    
    // 30
    std::vector<PrescriptionItem *> cart;
    Patient *patient;
    Operator *doctor;

    // 33
    std::string placeDate;
    
    // .m 31
    wxString currentFileName;
};
