//  SQLiteDatabase.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 16 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <wx/wx.h>
#include <sqlite3.h>

// TODO wxVector<wxVariant>
#define ONE_SQL_RESULT          std::vector<struct myStruct>
#define ALL_SQL_RESULTS         std::vector<ONE_SQL_RESULT>

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
    void initReadWriteWithPath(wxString path);
    bool createWithPath_andTable_andColumns(wxString path, wxString table, wxString columns);
    ALL_SQL_RESULTS performQuery(wxString query);
    int numberRecordsForTable(wxString table);
    bool insertRowIntoTable_forColumns_andValues(wxString table, wxString columns, wxString values);
    bool updateRowIntoTable_forExpressions_andConditions(wxString table, wxString expressions, wxString conditions);
    bool deleteRowFromTable_withUId(wxString table, wxString uId);
    void close();
    
private:
    sqlite3 *database;
};

