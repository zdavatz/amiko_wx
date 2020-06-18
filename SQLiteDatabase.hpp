//  SQLiteDatabase.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 16 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <wx/wx.h>
#include <sqlite3.h>

union myType {
    const unsigned char *c;
    int i;
    double d;
};

struct myStruct {
    int type;
    union myType u;
};

#define MYARRAY             std::vector<struct myStruct>
#define MYRESULTS           std::vector<MYARRAY>

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

