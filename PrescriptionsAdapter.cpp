//  PrescriptionsAdapter.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 23 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <iostream>
#include <iomanip>
#include <fstream>

#include <wx/wx.h>
#include <wx/arrstr.h>
#include <wx/wfstream.h>
#include <wx/base64.h>
#include <wx/url.h>

#include <nlohmann/json.hpp>

#include "PrescriptionsAdapter.hpp"
#include "Medication.hpp"
#include "PrescriptionItem.hpp"

#include "Patient.hpp"
//#include "Contacts.h"
#include "Operator.hpp"
#include "Utilities.hpp"
#include "DefaultsController.hpp"

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

// 160
// It will in any case create a new file
// if the overwrite flag is set, delete the original file
// Instead of using nested dictionaries like in amiko-osx, work with JSON directly.
wxURL PrescriptionsAdapter::savePrescriptionForPatient_withUniqueHash_andOverwrite(Patient *p, wxString hash, bool overwrite)
{
    wxURL url;

    if (!p) {
        std::cerr << __FUNCTION__ << " " << __LINE__ << ", patient not defined\n";
        return url;
    }

    if (overwrite && currentFileName.IsEmpty()) {
        std::cerr << __FUNCTION__ << " " << __LINE__ << ", cannot overwrite an empty filename\n";
        return url;
    }

    if (cart.size() < 1) {
        std::cerr << __FUNCTION__ << " " << __LINE__ << ", cart is empty\n";
        return url;
    }

    // 181
    // Assign patient
    patient = p;
    
    wxString documentsDir = UTI::documentsDirectory();

    // Check if patient has already a directory, if not create one
    wxString patientDir = documentsDir + wxFILE_SEP_PATH + patient->uniqueId;
    
    if (overwrite) {
        // Delete old file
        if (!wxRemoveFile(currentFileName)) {
            std::cerr << "Error deleting: " << currentFileName << std::endl;
        }
    }
    
    // 199
    // Define a new filename
    wxString currentTime = UTI::currentTime();
    currentTime.Replace(":", "");
    currentTime.Replace(".", "");
    wxString fileName = wxString::Format("RZ_%s.amk", currentTime);
    
    wxMkdir(patientDir);
    wxString path = patientDir + wxFILE_SEP_PATH + fileName;

    currentFileName = path;  // full path
    std::clog << __FUNCTION__ << " new currentFileName:" << currentFileName << std::endl;

    // 210
    nlohmann::json jsonStr; // prescriptionDict
    
    nlohmann::json patientDict;
    patientDict[KEY_AMK_PAT_ID] = patient->uniqueId;
    patientDict[KEY_AMK_PAT_SURNAME] = patient->familyName;
    patientDict[KEY_AMK_PAT_NAME] = patient->givenName;
    patientDict[KEY_AMK_PAT_BIRTHDATE] = patient->birthDate;
    patientDict[KEY_AMK_PAT_GENDER] = patient->gender;
    patientDict[KEY_AMK_PAT_WEIGHT] = wxString::Format("%d", patient->weightKg);
    patientDict[KEY_AMK_PAT_HEIGHT] = wxString::Format("%d", patient->heightCm);
    patientDict[KEY_AMK_PAT_ADDRESS] = patient->postalAddress;
    patientDict[KEY_AMK_PAT_ZIP] = patient->zipCode;
    patientDict[KEY_AMK_PAT_CITY] = patient->city;
    patientDict[KEY_AMK_PAT_COUNTRY] = patient->country;
    patientDict[KEY_AMK_PAT_PHONE] = patient->phoneNumber;
    patientDict[KEY_AMK_PAT_EMAIL] = patient->emailAddress;

    DefaultsController* defaults = DefaultsController::Instance();

    nlohmann::json operatorDict;
    operatorDict[KEY_AMK_DOC_TITLE] = defaults->getString(DEFAULTS_DOC_TITLE, "");
    operatorDict[KEY_AMK_DOC_SURNAME] = defaults->getString(DEFAULTS_DOC_SURNAME, "");
    operatorDict[KEY_AMK_DOC_NAME] = defaults->getString(DEFAULTS_DOC_NAME, "");
    operatorDict[KEY_AMK_DOC_ADDRESS] = defaults->getString(DEFAULTS_DOC_ADDRESS, "");
    operatorDict[KEY_AMK_DOC_ZIP] = defaults->getString(DEFAULTS_DOC_ZIP, "");
    operatorDict[KEY_AMK_DOC_CITY] = defaults->getString(DEFAULTS_DOC_CITY, "");
    operatorDict[KEY_AMK_DOC_PHONE] = defaults->getString(DEFAULTS_DOC_PHONE, "");
    operatorDict[KEY_AMK_DOC_EMAIL] = defaults->getString(DEFAULTS_DOC_EMAIL, "");
    
    placeDate = wxString::Format("%s, %s",
                 defaults->getString(DEFAULTS_DOC_CITY, ""),
                 UTI::prettyTime());
    
    wxString encodedImgStr;
    wxString pngFilePath = UTI::documentsDirectory() + wxFILE_SEP_PATH + DOC_SIGNATURE_FILENAME;
    if (pngFilePath.length() > 0) {
#if 1
        std::clog << __PRETTY_FUNCTION__ << " Line " << __LINE__
        << " TODO: encode signature " << pngFilePath << " as base64" << std::endl;
#else
        // 245
        NSImage *img = [[NSImage alloc] initWithContentsOfFile:pngFilePath];
        NSData *imgData = [img TIFFRepresentation];
        NSBitmapImageRep *imageRep = [NSBitmapImageRep imageRepWithData:imgData];
        NSData *data = [imageRep representationUsingType:NSPNGFileType properties:@{}];
        encodedImgStr = [data base64Encoding];
#endif
    }
    operatorDict[KEY_AMK_DOC_SIGNATURE] = encodedImgStr;

    // 253
    nlohmann::json prescription = nlohmann::json::array();

    for (PrescriptionItem *item : cart) {
        nlohmann::json dict;

        dict["product_name"] = item->title;
        dict["package"] = item->fullPackageInfo;
        if (item->eanCode.length() > 0)
            dict["eancode"] = item->eanCode;

        dict["comment"] = item->comment;
        dict["title"] = item->med->title;
        dict["owner"] = item->med->auth;
        dict["regnrs"] = item->med->regnrs;
        dict["atccode"] = item->med->atccode;

        prescription.push_back(dict);
    }

    jsonStr["prescription_hash"] = hash;
    jsonStr["place_date"] = placeDate;
    jsonStr["patient"] = patientDict;
    jsonStr["operator"] = operatorDict;
    jsonStr["medications"] = prescription;
    
#if 1
    std::clog << __PRETTY_FUNCTION__ << " Line " << __LINE__
    << " TODO: encode " << path << " as base64" << std::endl;

    std::ofstream o(path, std::ios::trunc); // overwrite
    o << std::setw(4) << jsonStr << std::endl;
    return wxURL(path);
#else
    // Map cart array to json
    NSError *error = nil;
    NSData *jsonObject = [NSJSONSerialization dataWithJSONObject:prescriptionDict
                                                         options:NSJSONWritingPrettyPrinted
                                                           error:&error];
    // BOOL success = [jsonObject writeToFile:path options:NSUTF8StringEncoding error:&error];
    
    NSString *jsonStr = [[NSString alloc] initWithData:jsonObject encoding:NSUTF8StringEncoding];
    NSString *base64Str = [MLUtilities encodeStringToBase64:jsonStr];
    
    BOOL success = [base64Str writeToFile:path atomically:YES encoding:NSUTF8StringEncoding error:&error];
    if (!success) {
        NSLog(@"Error: %@", [error userInfo]);
    }

    return [[NSURL alloc] initWithString:path];
#endif
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
