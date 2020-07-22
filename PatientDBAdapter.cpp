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
    wxString documentsDir = wxStandardPaths::Get().GetDocumentsDir();
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

    std::clog <<"Patient DB file NOT found: " << filePath << std::endl;
    if (myPatientDb->createWithPath_andTable_andColumns(filePath, DATABASE_TABLE, DATABASE_COLUMNS)) {
        myPatientDb->initReadWriteWithPath(filePath);
        return true;
    }

    return false;
}

// 130
wxString PatientDBAdapter::addEntry(Patient *patient)
{
#if 1
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
#else
    if (myPatientDb) {
        // Patient entry does not exist (yet)
        NSString *uuidStr = [patient generateUniqueID];    // e.g. 3466684318797166812
        NSString *timeStr = [MLUtilities currentTime];
        NSString *columnStr = [NSString stringWithFormat:@"(%@)", ALL_COLUMNS];
        NSString *valueStr = [NSString stringWithFormat:@"(%ld, \"%@\", \"%@\", \"%@\", \"%@\", \"%@\", \"%@\", %d, %d, \"%@\", \"%@\", \"%@\", \"%@\", \"%@\", \"%@\")", patient.rowId, timeStr, uuidStr, patient.familyName, patient.givenName, patient.birthDate, patient.gender, patient.weightKg, patient.heightCm, patient.zipCode, patient.city, patient.country, patient.postalAddress, patient.phoneNumber, patient.emailAddress];
        // Insert new entry into DB
        [myPatientDb insertRowIntoTable:@"patients" forColumns:columnStr andValues:valueStr];
        return uuidStr;
    }
#endif

    return wxEmptyString;
}

// 145
wxString PatientDBAdapter::insertEntry(Patient *patient)
{
#if 1
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
#else
    if (myPatientDb) {
        // If UUID exist re-use it!
        if (patient.uniqueId!=nil && [patient.uniqueId length]>0) {
            NSString *expressions = [NSString stringWithFormat:@"%@=%d, %@=%d, %@=\"%@\", %@=\"%@\", %@=\"%@\", %@=\"%@\", %@=\"%@\", %@=\"%@\", %@=\"%@\"", KEY_WEIGHT_KG, patient.weightKg, KEY_HEIGHT_CM, patient.heightCm, KEY_ZIPCODE, patient.zipCode, KEY_CITY, patient.city, KEY_COUNTRY, patient.country, KEY_ADDRESS, patient.postalAddress, KEY_PHONE, patient.phoneNumber, KEY_EMAIL, patient.emailAddress, KEY_GENDER, patient.gender];
            NSString *conditions = [NSString stringWithFormat:@"%@=\"%@\"", KEY_UID, patient.uniqueId];
            // Update existing entry
            [myPatientDb updateRowIntoTable:@"patients" forExpressions:expressions andConditions:conditions];
            return patient.uniqueId;
        } else {
            return [self addEntry:patient];
        }
    }
#endif
    return wxEmptyString;
}

// 186
long PatientDBAdapter::getLargestRowId()
{
    wxString query = wxString::Format("select max(%s) from %s", KEY_ROWID, DATABASE_TABLE);

    ALL_SQL_RESULTS results = myPatientDb->performQuery(query);
    if (results.size() > 0) {
#if 1
        std::clog << __PRETTY_FUNCTION__ << " Line " << __LINE__ << " TODO" << std::endl;
#else
        if (results[0]!=nil) {
            NSString *r = (NSString *)[results[0] objectAtIndex:0];
            if (![r isEqual:[NSNull null]])
                return [r longLongValue];
        }
#endif
    }

    return 0;
}

// 248
Patient * PatientDBAdapter::getPatientWithUniqueID(wxString uniqueID)
{
#if 1
    std::clog << __PRETTY_FUNCTION__ << " Line " << __LINE__ << " TODO" << std::endl;
#else
    if (uniqueID!=nil) {
        NSString *query = [NSString stringWithFormat:@"select %@ from %@ where %@ like '%@'", ALL_COLUMNS, DATABASE_TABLE, KEY_UID, uniqueID];
        NSArray *results = [myPatientDb performQuery:query];
        if ([results count]>0) {
            for (NSArray *cursor in results) {
                return [self cursorToPatient:cursor];
            }
        }
    }
#endif
    return nullptr;
}
