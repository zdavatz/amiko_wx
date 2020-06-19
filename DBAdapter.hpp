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
    MYRESULTS searchAuthor(wxString author);
    MYRESULTS searchATCCode(wxString atccode);
    MYRESULTS searchRegNr(wxString regnr);
    MYRESULTS searchApplication(wxString application);
    int getNumRecords();
    std::vector<Medication *> extractShortMedInfoFrom(MYRESULTS &results);
    Medication * cursorToShortMedInfo(MYARRAY & cursor);
    
private:
     SQLiteDatabase *mySqliteDb;
};
