//  SQLiteDatabase.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 16 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <wx/wx.h>
#include <sqlite3.h>

// TODO wxVector<wxVariant>
#define ONE_RESULT          std::vector<struct myStruct>
#define ALL_RESULTS         std::vector<ONE_RESULT>

union myType { // only simple types allowed
    char *c;
    int i;
    double d;
};

struct myStruct {
    int type;
    union myType u;

    myStruct() {u.c = nullptr;} // constructor
};

class SQLiteDatabase
{
public:
    SQLiteDatabase();
    virtual ~SQLiteDatabase() {}
    
    void initReadOnlyWithPath(wxString path);
    ALL_RESULTS performQuery(wxString query);
    int numberRecordsForTable(wxString table);
    void close();
    
private:
    sqlite3 *database;
};

