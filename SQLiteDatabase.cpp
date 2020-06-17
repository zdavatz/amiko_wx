//  SQLiteDatabase.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 16 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include "SQLiteDatabase.hpp"

SQLiteDatabase::SQLiteDatabase()
: database(nullptr)
{
    
}

void SQLiteDatabase::initReadOnlyWithPath(wxString path)
{
#ifndef NDEBUG
    std::clog << __PRETTY_FUNCTION__ << " " << path.ToStdString() << std::endl;
#endif
    sqlite3 *dbConnection;
    
    int rc = sqlite3_open_v2(path.c_str(), &dbConnection, SQLITE_OPEN_READONLY, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << __PRETTY_FUNCTION__ << " Unable to open database!\n";
        return;
    }

    database = dbConnection;

    // Force using disk for temp storage to reduce memory footprint
    rc = sqlite3_exec(database, "PRAGMA temp_store=1", nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK)
        std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << ", rc " << rc << std::endl;
}
