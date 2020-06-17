//  FullTextDBAdapter.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 17 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <wx/wx.h>

class SQLiteDatabase;

class FullTextDBAdapter
{
public:
    FullTextDBAdapter();
    virtual ~FullTextDBAdapter() {}

    bool openDatabase(wxString dbName);
    int getNumRecords();

private:
    SQLiteDatabase *myFullTextDb;
};
