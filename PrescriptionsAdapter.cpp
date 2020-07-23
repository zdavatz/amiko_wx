//  PrescriptionsAdapter.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 23 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <iostream>

#include <wx/wx.h>
#include <wx/arrstr.h>
#include <wx/wfstream.h>
#include "wx/base64.h"

#include <nlohmann/json.hpp>

#include "PrescriptionsAdapter.hpp"
#include "Medication.hpp"
#include "PrescriptionItem.hpp"

#include "Patient.hpp"
//#include "Contacts.h"
#include "Operator.hpp"

PrescriptionsAdapter::PrescriptionsAdapter()
: patient(nullptr)
, doctor(nullptr)
{
    
}

// 39
// Returns an array of filenames (wxString),
// just the basename with the extension ".amk" stripped off
wxArrayString PrescriptionsAdapter::listOfPrescriptionsForPatient(Patient *p)
{
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
    wxArrayString a;
    return a;
}

// 79
// Returns an array of filenames (wxString), the full path
wxArrayString PrescriptionsAdapter::listOfPrescriptionURLsForPatient(Patient *p)
{
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;

    wxArrayString a;
    return a;
}

// 292
// see Generika importReceiptFromURL
// see AmiKo-osx loadPrescriptionFromFile
// see AmiKo-ios importFromURL
wxString PrescriptionsAdapter::loadPrescriptionFromFile(wxString filePath)
{
    wxFFileInputStream file(filePath);
    size_t FileSize = file.GetSize();
    
    char *buffer = new char[FileSize];
    file.Read(buffer, FileSize);
    
    wxString base64Str = wxString::FromUTF8(buffer);
    //std::clog << " base64Str: " << base64Str << std::endl;

    wxMemoryBuffer buf = wxBase64Decode(base64Str.c_str(), wxNO_LEN, wxBase64DecodeMode_Strict);
    wxString jsonStr((const char *)buf.GetData(), buf.GetDataLen());
    
    //std::clog << " buf: " << jsonStr << std::endl;
    
    // 300
    currentFileName = filePath;
    
    //nlohmann::json tree;

    auto jsonDict = nlohmann::json::parse((char *)buf.GetData());
//    for (auto & element : jsonDict) {
//        std::clog << "element "  << element << std::endl;
//    }

    auto medicat = jsonDict["medications"];
    //std::clog << "number of meds: " << medicat.count() << std::endl;
//    for (auto & op : medicat.items()) {
//        std::clog << op.key() << ", value: " << op.value() << std::endl;
//    }

    // 303
    // Prescription
    std::vector<PrescriptionItem *> prescription;
    for (auto & p : medicat) {
        MED_DICT mediDict;
        mediDict[KEY_AMK_MED_TITLE] = p["title"];
        mediDict[KEY_AMK_MED_OWNER] = p["owner"];
        mediDict[KEY_AMK_MED_REG_N] = p["regnrs"];
        mediDict[KEY_AMK_MED_ATC] = p["atccode"];

        Medication *med = new Medication;
        med->importFromDict(mediDict);
        
        PrescriptionItem *item = new PrescriptionItem;
        item->importFromDict(mediDict);
        item->med = med;
        
        // 313
        prescription.push_back(item);
    }
    
    cart = prescription;

    // 317
    {
        auto patientDict = jsonDict["patient"];
        PAT_DICT dict;
        dict[KEY_AMK_PAT_BIRTHDATE] = patientDict["birth_date"];
        dict[KEY_AMK_PAT_CITY] = patientDict["city"];
        dict[KEY_AMK_PAT_COUNTRY] = patientDict["country"];
        dict[KEY_AMK_PAT_EMAIL] = patientDict["email_address"];
        dict[KEY_AMK_PAT_SURNAME] = patientDict["family_name"];
        dict[KEY_AMK_PAT_GENDER] = patientDict["gender"];
        dict[KEY_AMK_PAT_NAME] = patientDict["given_name"];
        dict[KEY_AMK_PAT_HEIGHT] = patientDict["height_cm"];
        dict[KEY_AMK_PAT_ID] = patientDict["patient_id"];
        dict[KEY_AMK_PAT_PHONE] = patientDict["phone_number"];
        dict[KEY_AMK_PAT_ADDRESS] = patientDict["postal_address"];
        dict[KEY_AMK_PAT_WEIGHT] = patientDict["weight_kg"];
        dict[KEY_AMK_PAT_ZIP] = patientDict["zip_code"];
        patient = new Patient;
        patient->importFromDict(dict);
    }
    
    // 321
    {
        auto operatorDict = jsonDict["operator"];
        OPER_DICT dict;
        dict[KEY_AMK_DOC_CITY] = operatorDict["city"];
        dict[KEY_AMK_DOC_EMAIL] = operatorDict["email_address"];
        dict[KEY_AMK_DOC_SURNAME] = operatorDict["family_name"];
        dict[KEY_AMK_DOC_NAME] = operatorDict["given_name"];
        dict[KEY_AMK_DOC_PHONE] = operatorDict["phone_number"];
        dict[KEY_AMK_DOC_ADDRESS] = operatorDict["postal_address"];
        dict[KEY_AMK_DOC_SIGNATURE] = operatorDict["signature"];
        dict[KEY_AMK_DOC_TITLE] = operatorDict["title"];
        dict[KEY_AMK_DOC_ZIP] = operatorDict["zip_code"];
        doctor = new Operator;
        doctor->importFromDict(dict);
    }
    
    // 325
    placeDate = jsonDict["place_date"];
    if (placeDate.length() == 0)
        placeDate = jsonDict["date"];

    std::string hash = jsonDict["prescription_hash"];
    return hash;
}
