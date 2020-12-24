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
#include <wx/textfile.h>
#include <wx/dir.h>
#include <wx/utils.h>
#include <wx/mstream.h>

#include <nlohmann/json.hpp>

#include "PrescriptionsAdapter.hpp"
#include "Medication.hpp"
#include "PrescriptionItem.hpp"

#include "Patient.hpp"
//#include "Contacts.h"
#include "Operator.hpp"
#include "Utilities.hpp"
#include "DefaultsController.hpp"
#include "sync/GoogleSyncManager.hpp"

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
    wxArrayString amkFiles;

    if (p == nullptr)
        return amkFiles;
    
    wxFileName patientDir( UTI::documentsDirectory(), wxEmptyString); // important for directory names: specify empty filename

    // Check if patient has already a directory, TODO: if not create one
    patientDir.AppendDir("amk");
    patientDir.AppendDir(p->uniqueId);

    if (wxDir::Exists(patientDir.GetFullPath())) {
        // List content of directory
        wxArrayString rzList;
        wxDir::GetAllFiles(patientDir.GetFullPath(), &rzList, "*.amk");
        for (auto rz : rzList) {
            wxFileName fn(rz);
            amkFiles.Add( fn.GetName());
        }
        
        // 70
        // TODO: Sort
    }

    return amkFiles;
}

// 79
// Returns an array of filenames, the full path, including the extension .amk
wxArrayString PrescriptionsAdapter::listOfPrescriptionURLsForPatient(Patient *p)
{
    wxArrayString amkURLs;

    if (p == nullptr)
        return amkURLs;

    wxFileName patientDir( UTI::documentsDirectory(), wxEmptyString); // important for directory names: specify empty filename
    
    // 92
    // Check if patient has already a directory, if not create one
    patientDir.AppendDir("amk");
    patientDir.AppendDir(p->uniqueId);
    
    if (wxDir::Exists(patientDir.GetFullPath())) {
        // 98
        // List content of directory
        wxDir::GetAllFiles(patientDir.GetFullPath(), &amkURLs, "*.amk");
        
        // 108
        // TODO: Sort
    }
    
    return amkURLs;
}

// 117
void PrescriptionsAdapter::deletePrescriptionWithName_forPatient(wxString name, Patient *p)
{
    if (p == nullptr)
        return;

    // Assign patient
    patient = p;
    
    wxFileName patientDir( UTI::documentsDirectory(), wxEmptyString); // important for directory names: specify empty filename
    
    patientDir.AppendDir("amk");
    patientDir.AppendDir(p->uniqueId);
    patientDir.SetName(name);
    patientDir.SetExt("amk");

    // Delete file
    if (wxFileName::Exists(patientDir.GetFullPath()))
        wxRemoveFile(patientDir.GetFullPath());

    GoogleSyncManager::Instance()->requestSync();
}

// 160
// It will in any case create a new file
// if the overwrite flag is set, delete the original file
// Instead of using nested dictionaries like in amiko-osx, work with JSON directly.
wxURL PrescriptionsAdapter::savePrescriptionForPatient_withUniqueHash_andOverwrite(Patient *p, wxString hash, bool overwrite)
{
    wxURL url;

    if (!p) {
        std::cerr << "savePrescriptionForPatient " << __LINE__ << ", patient not defined\n";
        return url;
    }

    if (overwrite && currentFileName.IsEmpty()) {
        std::cerr << "savePrescriptionForPatient " << __LINE__ << ", cannot overwrite an empty filename\n";
        return url;
    }

    if (cart.size() < 1) {
        std::cerr << "savePrescriptionForPatient " << __LINE__ << ", cart is empty\n";
        return url;
    }

    // 181
    // Assign patient
    patient = p;
    
    // Check if patient has already a directory, if not create one
    wxFileName patientDir( UTI::documentsDirectory(), wxEmptyString);

    patientDir.AppendDir("amk");
    if (!wxDirExists(patientDir.GetFullPath())) {
        wxMkdir(patientDir.GetFullPath());
    }
    
    patientDir.AppendDir(p->uniqueId);
    if (!wxDirExists(patientDir.GetFullPath())) {
        wxMkdir(patientDir.GetFullPath());
    }

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

    wxFileName pathBase64 = patientDir;
    pathBase64.SetName("RZ_" + currentTime);
    pathBase64.SetExt("amk");

#ifndef NDEBUG
    wxFileName pathJson = patientDir;
    pathJson.SetName("RZ_" + currentTime);
    pathJson.SetExt("json");
#endif

    currentFileName = pathBase64.GetFullPath();

#ifndef NDEBUG
    std::clog << "savePrescriptionForPatient() new currentFileName:" << currentFileName << std::endl;
#endif

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

    wxString cityString;
    try {
        std::ifstream i((UTI::documentsDirectory() + wxFILE_SEP_PATH + DOC_JSON_FILENAME).ToStdString());
        nlohmann::json json;
        i >> json;

        operatorDict[KEY_AMK_DOC_TITLE] = json[DOC_JSON_TITLE].get<std::string>();
        operatorDict[KEY_AMK_DOC_SURNAME] = json[DOC_JSON_SURNAME].get<std::string>();
        operatorDict[KEY_AMK_DOC_NAME] = json[DOC_JSON_NAME].get<std::string>();
        operatorDict[KEY_AMK_DOC_ADDRESS] = json[DOC_JSON_ADDRESS].get<std::string>();
        operatorDict[KEY_AMK_DOC_ZIP] = json[DOC_JSON_ZIP].get<std::string>();
        operatorDict[KEY_AMK_DOC_CITY] = json[DOC_JSON_CITY].get<std::string>();
        operatorDict[KEY_AMK_DOC_PHONE] = json[DOC_JSON_PHONE].get<std::string>();
        operatorDict[KEY_AMK_DOC_EMAIL] = json[DOC_JSON_EMAIL].get<std::string>();
        cityString = json[DOC_JSON_CITY].get<std::string>();
    } catch (const std::exception& e) {
        // Just in case the file is not initialized
    }
    
    placeDate = wxString::Format("%s, %s",
                 cityString,
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

        dict[KEY_AMK_MED_PROD_NAME] = item->title;
        dict[KEY_AMK_MED_PACKAGE] = item->fullPackageInfo;
        if (item->eanCode.length() > 0)
            dict[KEY_AMK_MED_EAN] = item->eanCode;

        dict[KEY_AMK_MED_COMMENT] = item->comment;
        dict[KEY_AMK_MED_TITLE] = item->med->title;
        dict[KEY_AMK_MED_OWNER] = item->med->auth;
        dict[KEY_AMK_MED_REG_N] = item->med->regnrs;
        dict[KEY_AMK_MED_ATC] = item->med->atccode;

        prescription.push_back(dict);
    }

    jsonStr[KEY_AMK_PRESC_HASH] = hash;
    jsonStr[KEY_AMK_PRESC_PLACE_DATE] = placeDate;
    jsonStr[KEY_AMK_PRESC_PAT] = patientDict;
    jsonStr[KEY_AMK_PRESC_DOC] = operatorDict;
    jsonStr[KEY_AMK_PRESC_MEDS] = prescription;
    
    //std::cerr << "===JSON AMK:\n" << jsonStr << "\n===\n";

#ifndef NDEBUG
    // Crete JSON file
    std::ofstream o1(pathJson.GetFullPath(), std::ios::trunc); // overwrite
    o1 << std::setw(4) << jsonStr << std::endl;
#endif

    // Crete amk file, encoded Base64
    wxString o;
    o << jsonStr.dump(4);
#ifndef NDEBUG
    std::cerr << "Line " << __LINE__ << ", JSON length " << o.length()
    //<<< " jsonStr\n" << o
    << std::endl;
#endif

    wxCharBuffer buffer = o.ToUTF8();
    wxMemoryOutputStream mos(buffer.data(), strlen(buffer.data()));

#ifndef NDEBUG
    std::cerr << "Line " << __LINE__ << " mos.GetSize() " << mos.GetSize() << std::endl;
#endif

    wxString base64Str = wxBase64Encode(mos.GetOutputStreamBuffer()->GetBufferStart(),
                                        mos.GetSize());

#ifndef NDEBUG
    std::cerr << "Line " << __LINE__ 
            << " encoded base64Str size " << base64Str.length()
            //<< " base64Str\n" << base64Str 
            << std::endl;
#endif

#if 0 //ndef NDEBUG
    wxMemoryBuffer *buffy = new wxMemoryBuffer;
    size_t len = wxBase64EncodedSize(o.length());
    buffy->GetWriteBuf(len);
    buffy->AppendData(buffer.data(), strlen(buffer.data()));
    //wxString base64Str2 = wxBase64Encode(*buffy);
    //std::cerr << "Line " << __LINE__ << " base64Str2\n" << base64Str2 << std::endl;
    delete buffy;
#endif

    wxFileOutputStream file( pathBase64.GetFullPath() );
    file.Write(base64Str, base64Str.length());
    file.Close();

    GoogleSyncManager::Instance()->requestSync();

    return wxURL(pathBase64.GetFullPath());
}

// 292
// see Generika importReceiptFromURL
// see AmiKo-osx loadPrescriptionFromFile
// see AmiKo-ios importFromURL
wxString PrescriptionsAdapter::loadPrescriptionFromFile(wxString filePath)
{
    //std::clog << __FUNCTION__  << "\n\t" << filePath << std::endl;

    std::string hash;

    wxFFileInputStream file(filePath);
    size_t FileSize = file.GetSize();
    
    char *buffer = new char[FileSize];
    if (!buffer) {
        std::cerr << __FUNCTION__ << __LINE__ << " Couldn't allocate buffer\n";
        return hash;
    }
    file.Read(buffer, FileSize);
    
    // Specifying the file size for FromUTF8() is very important in this case
    wxString base64Str = wxString::FromUTF8(buffer, FileSize);
    if (base64Str.IsEmpty()) {
        std::cerr << __FUNCTION__ << " line " << __LINE__
                << "\n\t file: " << filePath
                << "\n\t FileSize: " << FileSize
                << " Empty base64Str\n";
        delete [] buffer;
        return hash;
    }
    
    delete [] buffer;
    
    if (FileSize != base64Str.length()) {
        std::cerr << __FUNCTION__ << " line " << __LINE__
        << ", FileSize: " << FileSize
        << ", base64Str: " << base64Str.length()
        << std::endl;
        return hash;
    }

    base64Str += wxT("\n");  // Linux parser seems to expect a line terminator

    try {
        wxMemoryBuffer buf = wxBase64Decode(base64Str.c_str(), wxNO_LEN, wxBase64DecodeMode_SkipWS);

        wxString jsonStr((const char *)buf.GetData(), buf.GetDataLen());
        if (jsonStr.IsEmpty()) {
#if 0 //ndef NDEBUG
            std::cerr << __FUNCTION__ << " line " << __LINE__
            << "\n\tbase64Str:" << base64Str.length()
            << "\n" << base64Str << ">>> base64Str >>>\n";
#endif
            std::cerr << __FUNCTION__ << __LINE__ << " Empty jsonStr\n";
            return hash;
        }

        // 300
        currentFileName = filePath;
        
        nlohmann::json jsonDict;
        try {
            //auto jsonDict = nlohmann::json::parse((char *)buf.GetData());
            jsonDict = nlohmann::json::parse((char *)jsonStr.char_str());
        }
        catch (const std::exception&e) {
            std::cerr << __FUNCTION__ << " line " << __LINE__ << std::endl
#if 0 //ndef NDEBUG
            << "base64Str: " << base64Str.length() << "\n" << base64Str
            << ">>> base64Str >>>\n"

            << "jsonStr:" << jsonStr.length() << "\n" << jsonStr
            << ">>> jsonStr >>>\n"

            << "buf.GetDataLen(): " << buf.GetDataLen() << std::endl
            << "jsonStr.length(): " << jsonStr.length() << std::endl
            << "jsonStr: " << jsonStr << std::endl
#endif
            << "Exception " << e.what() << std::endl;

            return hash;
        }

#if 0 //ndef NDEBUG
        for (auto & element : jsonDict)
            std::clog << "element "  << element.dump(4) << std::endl;
#endif

        auto medicat = jsonDict[KEY_AMK_PRESC_MEDS];
#if 0 //ndef NDEBUG
        std::clog << "medications: " << medicat.dump(4) << std::endl;
        for (auto & op : medicat.items())
            std::clog << op.key() << ", value: " << op.value() << std::endl;
#endif

    // 303
    // Prescription
    std::vector<PrescriptionItem *> prescription;
    for (auto & p : medicat) {
        MED_DICT mediDict;
        // For Medication:
        mediDict[KEY_AMK_MED_TITLE] = p[KEY_AMK_MED_TITLE];
        mediDict[KEY_AMK_MED_OWNER] = p[KEY_AMK_MED_OWNER];
        mediDict[KEY_AMK_MED_REG_N] = p[KEY_AMK_MED_REG_N];
        mediDict[KEY_AMK_MED_ATC] = p[KEY_AMK_MED_ATC];
        // For PrescriptionItem:
        mediDict[KEY_AMK_MED_PROD_NAME] = p[KEY_AMK_MED_PROD_NAME];
        mediDict[KEY_AMK_MED_PACKAGE] = p[KEY_AMK_MED_PACKAGE];

        // If the GTIN is missing
        // the field KEY_AMK_MED_PACKAGE in the JSON file is missing altogether,
        // (it's not an empty string). This will cause an exception
        // that we need to catch
        try {
            mediDict[KEY_AMK_MED_EAN] =  p[KEY_AMK_MED_EAN];
        }
        catch (const std::exception&e) {
            std::cerr << "Missing " << KEY_AMK_MED_EAN << ", exception:"
            << e.what() << std::endl;
        }

        mediDict[KEY_AMK_MED_COMMENT] = p[KEY_AMK_MED_COMMENT];

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
            auto patientDict = jsonDict[KEY_AMK_PRESC_PAT];
            PAT_DICT dict;
            dict[KEY_AMK_PAT_BIRTHDATE] = patientDict[KEY_AMK_PAT_BIRTHDATE];
            dict[KEY_AMK_PAT_CITY] = patientDict[KEY_AMK_PAT_CITY];
            dict[KEY_AMK_PAT_COUNTRY] = patientDict[KEY_AMK_PAT_COUNTRY];
            dict[KEY_AMK_PAT_EMAIL] = patientDict[KEY_AMK_PAT_EMAIL];
            dict[KEY_AMK_PAT_SURNAME] = patientDict[KEY_AMK_PAT_SURNAME];
            dict[KEY_AMK_PAT_GENDER] = patientDict[KEY_AMK_PAT_GENDER];
            dict[KEY_AMK_PAT_NAME] = patientDict[KEY_AMK_PAT_NAME];
            dict[KEY_AMK_PAT_HEIGHT] = patientDict[KEY_AMK_PAT_HEIGHT];
            dict[KEY_AMK_PAT_ID] = patientDict[KEY_AMK_PAT_ID];
            dict[KEY_AMK_PAT_PHONE] = patientDict[KEY_AMK_PAT_PHONE];
            dict[KEY_AMK_PAT_ADDRESS] = patientDict[KEY_AMK_PAT_ADDRESS];
            dict[KEY_AMK_PAT_WEIGHT] = patientDict[KEY_AMK_PAT_WEIGHT];
            dict[KEY_AMK_PAT_ZIP] = patientDict[KEY_AMK_PAT_ZIP];
            patient = new Patient;
            patient->importFromDict(dict);
        }
    
        // 321
        {
            auto operatorDict = jsonDict[KEY_AMK_PRESC_DOC];
            OPER_DICT dict;
            dict[KEY_AMK_DOC_CITY] = operatorDict[KEY_AMK_DOC_CITY];
            dict[KEY_AMK_DOC_EMAIL] = operatorDict[KEY_AMK_DOC_EMAIL];
            dict[KEY_AMK_DOC_SURNAME] = operatorDict[KEY_AMK_DOC_SURNAME];
            dict[KEY_AMK_DOC_NAME] = operatorDict[KEY_AMK_DOC_NAME];
            dict[KEY_AMK_DOC_PHONE] = operatorDict[KEY_AMK_DOC_PHONE];
            dict[KEY_AMK_DOC_ADDRESS] = operatorDict[KEY_AMK_DOC_ADDRESS];
            dict[KEY_AMK_DOC_SIGNATURE] = operatorDict[KEY_AMK_DOC_SIGNATURE];
            dict[KEY_AMK_DOC_TITLE] = operatorDict[KEY_AMK_DOC_TITLE];
            dict[KEY_AMK_DOC_ZIP] = operatorDict[KEY_AMK_DOC_ZIP];
            doctor = new Operator;
            doctor->importFromDict(dict);
        }
    
        // 325
        placeDate = jsonDict[KEY_AMK_PRESC_PLACE_DATE];
        if (placeDate.length() == 0)
            placeDate = jsonDict[KEY_AMK_PRESC_PLACE_DATE_OLD]; // backward compatibility ?

        hash = jsonDict[KEY_AMK_PRESC_HASH];
    }
    catch (const std::exception&e) {
        std::cerr << "Error parsing: " << filePath << std::endl
        << e.what() << std::endl;
    }

    return hash;
}
