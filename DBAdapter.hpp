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
    std::vector<Medication *> searchTitle(wxString title);
    ALL_RESULTS getFullRecord(long rowId);
    ALL_RESULTS searchAuthor(wxString author);
    ALL_RESULTS searchATCCode(wxString atccode);
    ALL_RESULTS searchRegNr(wxString regnr);
    ALL_RESULTS searchApplication(wxString application);
    int getNumRecords();
    std::vector<Medication *> extractShortMedInfoFrom(ALL_RESULTS &results);
    Medication * cursorToShortMedInfo(ONE_RESULT & cursor);
    Medication * cursorToFullMedInfo(ONE_RESULT &cursor);
    Medication * getMediWithId(long rowId);
    
private:
     SQLiteDatabase *mySqliteDb;
};
