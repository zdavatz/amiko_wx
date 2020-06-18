//  FullTextDBAdapter.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 17 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <wx/wx.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>

#include "FullTextDBAdapter.hpp"
#include "SQLiteDatabase.hpp"

// 32
static const char *KEY_ROWID = "id";
static const char *KEY_KEYWORD = "keyword";
static const char *KEY_REGNR = "regnr";

static wxString DATABASE_TABLE("frequency");

FullTextDBAdapter::FullTextDBAdapter()
: myFullTextDb(nullptr)
{
    
}

// 49
// TODO: unify repeated code from class DBAdapter
bool FullTextDBAdapter::openDatabase(wxString dbName)
{
#ifndef NDEBUG
    std::clog << __PRETTY_FUNCTION__ << " " << dbName.ToStdString() << std::endl;
#endif
    // A. Check first users documents folder
    wxString documentsDir = wxStandardPaths::Get().GetUserDataDir();
    wxString filePath( documentsDir + wxFILE_SEP_PATH + dbName + ".db");
    if (wxFileName::Exists(filePath)) {
        std::clog << "Fulltext DB found in UserData dir: " << filePath.ToStdString() << std::endl;
        myFullTextDb = new SQLiteDatabase;
        myFullTextDb->initReadOnlyWithPath(filePath);
        return true;

    }

    // B. If no database is available, check if db is in app bundle
    documentsDir = wxStandardPaths::Get().GetResourcesDir();
    // TODO:

#ifndef NDEBUG
    std::cerr << "Didn't find " << filePath.ToStdString() << std::endl;
#endif

    return false;
}

// 82
int FullTextDBAdapter::getNumRecords()
{
    return myFullTextDb->numberRecordsForTable(DATABASE_TABLE);
}

// 97
// Search fulltext containing keyword
MYRESULTS FullTextDBAdapter::searchKeyword(wxString keyword)
{
    wxString query = wxString::Format("select * from %s where %s like '%s%%'",
                                      DATABASE_TABLE.ToStdString(),
                                      KEY_KEYWORD,
                                      keyword.c_str());
    MYRESULTS results = myFullTextDb->performQuery(query);
    return results;
}
