//  PatientDBAdapter.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 21 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

class SQLiteDatabase;
class Patient;

class PatientDBAdapter
{
public:
    static PatientDBAdapter * sharedInstance();
    virtual ~PatientDBAdapter() {}
    
    long getLargestRowId();
    bool openDatabase(wxString dbName);
    Patient * getPatientWithUniqueID(wxString uniqueID);
    wxString addEntry(Patient *patient);
    wxString insertEntry(Patient *patient);
    
private:
    // private constructor so that it can not be called
    PatientDBAdapter();
    static PatientDBAdapter* sharedObject;

    // .m 55
    SQLiteDatabase *myPatientDb;
};
