//  dbAdapter.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 16 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <wx/wx.h>
#include "SQLiteDatabase.hpp"

#define MEDICATION_RESULTS         std::vector<Medication *>

class SQLiteDatabase;
class Medication;

class DBAdapter
{
public:
    DBAdapter();
    virtual ~DBAdapter() {}
    
    bool openDatabase(wxString dbName);
    void closeDatabase();

    MEDICATION_RESULTS getAll();
    MEDICATION_RESULTS searchTitle(wxString title);
    MEDICATION_RESULTS searchAuthor(wxString author);
    MEDICATION_RESULTS searchATCCode(wxString atccode);
    MEDICATION_RESULTS searchRegNr(wxString regnr);
    MEDICATION_RESULTS searchApplication(wxString application);
    MEDICATION_RESULTS searchRegnrsFromList(wxArrayString listOfRegnrs);
    ALL_SQL_RESULTS getFullRecord(long rowId);
    int getNumRecords();
    MEDICATION_RESULTS extractShortMedInfoFrom(ALL_SQL_RESULTS &results);
    Medication * cursorToVeryShortMedInfo(ONE_SQL_RESULT &cursor);
    Medication * cursorToShortMedInfo(ONE_SQL_RESULT &cursor);
    Medication * cursorToFullMedInfo(ONE_SQL_RESULT &cursor);
    Medication * getMediWithId(long rowId);
    Medication * getShortMediWithId(long rowId);
    Medication * getMediWithRegnr(wxString regnr);

private:
     
    SQLiteDatabase *mySqliteDb;
    MEDICATION_RESULTS cachedAll;
};
