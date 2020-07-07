//  FullTextDBAdapter.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 17 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <set>
#include <map>

#include <wx/wx.h>
#include "SQLiteDatabase.hpp"
#include "FullTextSearch.hpp"  // for NSDictionary

//class SQLiteDatabase;
class FullTextEntry;

class FullTextDBAdapter
{
public:
    FullTextDBAdapter();
    virtual ~FullTextDBAdapter() {}

    bool openDatabase(wxString dbName);
    void closeDatabase();
    int getNumRecords();
    std::vector<FullTextEntry *> searchKeyword(wxString keyword);
    std::vector<FullTextEntry *> extractFullTextEntryFrom(ALL_SQL_RESULTS &results);
    std::map<wxString, std::set<wxString>> regChapterDict(wxString regChapterStr);
    FullTextEntry * searchHash(wxString hash);
    FullTextEntry * cursorToFullTextEntry(ONE_SQL_RESULT &cursor);

private:

    SQLiteDatabase *myFullTextDb;
};
