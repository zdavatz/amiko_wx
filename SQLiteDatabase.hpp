//  SQLiteDatabase.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 16 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <wx/wx.h>
#include <sqlite3.h>

class SQLiteDatabase
{
public:
    SQLiteDatabase();
    virtual ~SQLiteDatabase() {}
    
    void initReadOnlyWithPath(wxString path);
    
private:
    sqlite3 *database;
};

