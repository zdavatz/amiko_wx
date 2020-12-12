//  PatientDBAdapter.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 21 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <wx/wx.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>

#include "PatientDBAdapter.hpp"
#include "SQLiteDatabase.hpp"
#include "Patient.hpp"
#include "Utilities.hpp"

// 30
static const char * KEY_ROWID = "_id";
static const char * KEY_TIMESTAMP = "time_stamp";
static const char * KEY_UID = "uid";
static const char * KEY_FAMILYNAME = "family_name";
static const char * KEY_GIVENNAME = "given_name";
static const char * KEY_BIRTHDATE = "birthdate";
static const char * KEY_GENDER  = "gender";
static const char * KEY_WEIGHT_KG = "weight_kg";
static const char * KEY_HEIGHT_CM = "height_cm";
static const char * KEY_ZIPCODE = "zip";
static const char * KEY_CITY = "city";
static const char * KEY_COUNTRY = "country";
static const char * KEY_ADDRESS = "address";
static const char * KEY_PHONE = "phone";
static const char * KEY_EMAIL = "email";

// 46
static wxString DATABASE_TABLE("patients");

// 48
// Table columns for fast queries
static wxString ALL_COLUMNS;
static wxString DATABASE_COLUMNS;

// 62
PatientDBAdapter* PatientDBAdapter::sharedObject;

// Singleton
PatientDBAdapter* PatientDBAdapter::sharedInstance()
{
    if (!sharedObject) {
        sharedObject = new PatientDBAdapter;
        
        // Strange name "patient_db.db" in amiko-osx
        if (!sharedObject->openDatabase("patient.db")) {
            std::cerr << "Could not open patient DB!";
            sharedObject = nullptr;
        }
    }

    return sharedObject;
}

// 80
PatientDBAdapter::PatientDBAdapter()
: myPatientDb(nullptr)
{
    // 82
    if (ALL_COLUMNS.IsEmpty()) {
        ALL_COLUMNS = wxString ::Format("%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s", KEY_ROWID, KEY_TIMESTAMP, KEY_UID, KEY_FAMILYNAME, KEY_GIVENNAME, KEY_BIRTHDATE, KEY_GENDER, KEY_WEIGHT_KG, KEY_HEIGHT_CM, KEY_ZIPCODE, KEY_CITY, KEY_COUNTRY, KEY_ADDRESS, KEY_PHONE, KEY_EMAIL);
    }

    if (DATABASE_COLUMNS.IsEmpty()) {
        DATABASE_COLUMNS = wxString ::Format("(%s INTEGER, %s TEXT, %s TEXT, %s TEXT, %s TEXT, %s TEXT, %s TEXT, %s INTEGER, %s INTEGER, %s TEXT, %s TEXT, %s TEXT, %s TEXT, %s TEXT, %s TEXT)", KEY_ROWID, KEY_TIMESTAMP, KEY_UID, KEY_FAMILYNAME, KEY_GIVENNAME, KEY_BIRTHDATE, KEY_GENDER, KEY_WEIGHT_KG, KEY_HEIGHT_CM, KEY_ZIPCODE, KEY_CITY, KEY_COUNTRY, KEY_ADDRESS, KEY_PHONE, KEY_EMAIL);
    }
}

// 96
bool PatientDBAdapter::openDatabase(wxString dbName)
{
    std::clog << __FUNCTION__ << std::endl;

    if (myPatientDb)
        return false;
    
    myPatientDb = new SQLiteDatabase();

    // Patient DB should be in the user's documents folder

    // 101
    // Get documents directory
    wxString documentsDir = wxStandardPaths::Get().GetUserDataDir();
    wxString filePath( documentsDir + wxFILE_SEP_PATH + dbName);

    // 104
    // Check if database exists
    if (filePath.length() <= 0)
        return false;

    // 106
    // Load database if it exists already
    if (wxFileName::Exists(filePath)) {
        std::clog << "Patient DB file found: " << filePath << std::endl;
        myPatientDb->initReadWriteWithPath(filePath);
        return true;
    }

    std::clog << "Patient DB file NOT found: " << filePath << std::endl;
    if (myPatientDb->createWithPath_andTable_andColumns(filePath, DATABASE_TABLE, DATABASE_COLUMNS)) {
        myPatientDb->initReadWriteWithPath(filePath);
        return true;
    }

    return false;
}

// 130
wxString PatientDBAdapter::addEntry(Patient *patient)
{
    if (myPatientDb) {
        // Patient entry does not exist (yet)
        wxString uuidStr = patient->generateUniqueID();
        wxString timeStr = UTI::currentTime();
        wxString columnStr = wxString::Format("(%s)", ALL_COLUMNS);
        wxString valueStr = wxString::Format("(%ld, \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", %d, %d, \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\")", patient->rowId, timeStr, uuidStr, patient->familyName, patient->givenName, patient->birthDate, patient->gender, patient->weightKg, patient->heightCm, patient->zipCode, patient->city, patient->country, patient->postalAddress, patient->phoneNumber, patient->emailAddress);

        // Insert new entry into DB
        bool ok =
        myPatientDb->insertRowIntoTable_forColumns_andValues(DATABASE_TABLE, columnStr, valueStr);
        return uuidStr;
    }

    return wxEmptyString;
}

// 145
wxString PatientDBAdapter::insertEntry(Patient *patient)
{
    if (!myPatientDb)
        return wxEmptyString;

    // If UUID exists re-use it!
    if (patient->uniqueId.length() > 0)
    {
        wxString expressions = wxString::Format("%s=%d, %s=%d, %s=\"%s\", %s=\"%s\", %s=\"%s\", %s=\"%s\", %s=\"%s\", %s=\"%s\", %s=\"%s\"",
                                                KEY_WEIGHT_KG, patient->weightKg,
                                                KEY_HEIGHT_CM, patient->heightCm,
                                                KEY_ZIPCODE, patient->zipCode,
                                                KEY_CITY, patient->city,
                                                KEY_COUNTRY, patient->country,
                                                KEY_ADDRESS, patient->postalAddress,
                                                KEY_PHONE, patient->phoneNumber,
                                                KEY_EMAIL, patient->emailAddress,
                                                KEY_GENDER, patient->gender);
        wxString conditions = wxString::Format("%s=\"%s\"", KEY_UID, patient->uniqueId);

        // Update existing entry
        myPatientDb->updateRowIntoTable_forExpressions_andConditions(DATABASE_TABLE, expressions, conditions);
        return patient->uniqueId;
    }

    // It doesn't normally get here
    std::clog << __FUNCTION__ << " Line " << __LINE__ << std::endl;
    return addEntry(patient);
}

// 170
bool PatientDBAdapter::deleteEntry(Patient *patient)
{
    if (myPatientDb) {
        myPatientDb->deleteRowFromTable_withUId(DATABASE_TABLE, patient->uniqueId);
        return true;
    }

    return false;
}

// 186
long PatientDBAdapter::getLargestRowId()
{
    wxString query = wxString::Format("select max(%s) from %s", KEY_ROWID, DATABASE_TABLE);

    ALL_SQL_RESULTS results = myPatientDb->performQuery(query);
    if (results.size() > 0)
    {
        ONE_SQL_RESULT cursor = results[0];
        if (cursor.size() > 0)
        {
//            wxString r = cursor[0].u.c;
//            if (r.length() > 0)
//                return wxAtol(r);
            return cursor[0].u.i;
        }
    }

    return 0L;
}

static bool
less_than_patient_name(const Patient * p1, const Patient * p2) {
    if (p1->familyName == p2->familyName)
        return strcmp(p1->givenName.Lower().c_str(),
                      p2->givenName.Lower().c_str()) < 0;

    return strcmp(p1->familyName.Lower().c_str(),
                  p2->familyName.Lower().c_str()) < 0;
}
    
// 200
std::vector<Patient *> PatientDBAdapter::getAllPatients()
{
    std::vector<Patient *> listOfPatients;

    wxString query = wxString::Format("select %s from %s", ALL_COLUMNS, DATABASE_TABLE);
    ALL_SQL_RESULTS results = myPatientDb->performQuery(query);
    //std::clog << __FUNCTION__ << " # patients in DB: " << results.size() << std::endl;
    if (results.size() > 0) {
        for (auto cursor : results)
            listOfPatients.push_back(cursorToPatient(cursor));

        // 210
        // Sort alphabetically
        std::sort(listOfPatients.begin(), listOfPatients.end(), less_than_patient_name);
    }

    return listOfPatients;
}

// 248
Patient * PatientDBAdapter::getPatientWithUniqueID(wxString uniqueID)
{
    if (uniqueID.length() > 0)
    {
        wxString query = wxString::Format("select %s from %s where %s like '%s'", ALL_COLUMNS, DATABASE_TABLE, KEY_UID, uniqueID);
        ALL_SQL_RESULTS results = myPatientDb->performQuery(query);
        if (results.size() > 0) {
            ONE_SQL_RESULT cursor = results[0];
            return cursorToPatient(cursor);
        }
    }

    return nullptr;
}

std::vector<Patient*> PatientDBAdapter::getPatientsWithUniqueIDs(std::set<std::string> uniqueIDs)
{
    std::vector<Patient*> patients;
    if (!uniqueIDs.empty())
    {
        std::string patientIDs;
        for (const auto& uniqueID : uniqueIDs) {
            if (!patientIDs.empty()) {
                patientIDs += ",";
            }
            patientIDs += "'" + uniqueID + "'";
        }
        wxString query = wxString::Format("select %s from %s where %s IN (%s)", ALL_COLUMNS, DATABASE_TABLE, KEY_UID, patientIDs);
        ALL_SQL_RESULTS results = myPatientDb->performQuery(query);
        for (auto row : results) {
            patients.push_back(cursorToPatient(row));
        }
    }
    return patients;
}

std::map<std::string, std::string> PatientDBAdapter::getAllTimestamps() {
    wxString query = wxString::Format("select %s, %s from %s", KEY_UID, KEY_TIMESTAMP, DATABASE_TABLE);
    ALL_SQL_RESULTS results = myPatientDb->performQuery(query);
    std::map<std::string, std::string> map;
    for (auto cursor : results) {
        map[cursor[0].u.c] = cursor[1].u.c;
    }
    return map;
}

Patient * PatientDBAdapter::cursorToPatient(ONE_SQL_RESULT &cursor)
{
    Patient *patient = new Patient;
    
    patient->rowId = cursor[0].u.i;
    patient->uniqueId = cursor [2].u.c;
    patient->familyName = cursor[3].u.c;
    patient->givenName = cursor[4].u.c;
    patient->birthDate = cursor[5].u.c;
    patient->gender = cursor[6].u.c;
    patient->weightKg = cursor[7].u.i;
    patient->heightCm = cursor[8].u.i;
    patient->zipCode = cursor[9].u.c;
    patient->city = cursor[10].u.c;
    patient->country = cursor[11].u.c;
    patient->postalAddress = cursor[12].u.c;
    patient->phoneNumber = cursor[13].u.c;
    patient->emailAddress = cursor[14].u.c;
    patient->databaseType = eLocal;
    
    return patient;
}
