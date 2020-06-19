//  SQLiteDatabase.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 16 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <wx/wx.h>
#include <sqlite3.h>

// TODO wxVector<wxVariant>
#define MYARRAY             std::vector<struct myStruct>
#define MYRESULTS           std::vector<MYARRAY>

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
    MYRESULTS performQuery(wxString query);
    int numberRecordsForTable(wxString table);
    
private:
    sqlite3 *database;
};

