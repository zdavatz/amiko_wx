//  dbAdapter.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 16 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <wx/wx.h>
#include "SQLiteDatabase.hpp"

class SQLiteDatabase;
class Medication;

class DBAdapter
{
public:
    DBAdapter();
    virtual ~DBAdapter() {}
    
    bool openDatabase(wxString dbName);
    void closeDatabase();

    std::vector<Medication *> searchTitle(wxString title);
    std::vector<Medication *> searchAuthor(wxString author);
    std::vector<Medication *> searchATCCode(wxString atccode);
    std::vector<Medication *> searchRegNr(wxString regnr);
    std::vector<Medication *> searchApplication(wxString application);
    std::vector<Medication *> searchRegnrsFromList(wxArrayString listOfRegnrs);
    ALL_SQL_RESULTS getFullRecord(long rowId);
    int getNumRecords();
    std::vector<Medication *> extractShortMedInfoFrom(ALL_SQL_RESULTS &results);
    Medication * cursorToVeryShortMedInfo(ONE_SQL_RESULT &cursor);
    Medication * cursorToShortMedInfo(ONE_SQL_RESULT &cursor);
    Medication * cursorToFullMedInfo(ONE_SQL_RESULT &cursor);
    Medication * getMediWithId(long rowId);
    
private:
     
    SQLiteDatabase *mySqliteDb;
};
