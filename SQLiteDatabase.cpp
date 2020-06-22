//  SQLiteDatabase.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 16 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <string.h> // for strdup()
#include "SQLiteDatabase.hpp"

SQLiteDatabase::SQLiteDatabase()
: database(nullptr)
{
    
}

// 68
void SQLiteDatabase::initReadOnlyWithPath(wxString path)
{
#ifndef NDEBUG
    //std::clog << __PRETTY_FUNCTION__ << " " << path.ToStdString() << std::endl;
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

// 151
int SQLiteDatabase::numberRecordsForTable(wxString table)
{
    int numTableRecords = -1;
    sqlite3_stmt *sqlClause = NULL;
    
    wxString sqlStatement = wxString::Format("SELECT COUNT(*) FROM %s", table.ToStdString());
    const char *sql = sqlStatement.c_str();
    if (sqlite3_prepare_v2(database, sql, -1, &sqlClause, NULL) == SQLITE_OK) {
        while(sqlite3_step(sqlClause) == SQLITE_ROW) {
            numTableRecords = sqlite3_column_int(sqlClause, 0);
        }
    }
    else {
        std::cerr << __FUNCTION__
        << " Could not prepare statement: " << sqlite3_errmsg(database) << std::endl;
    }

    return numTableRecords;
}

// 169
ALL_RESULTS SQLiteDatabase::performQuery(wxString query)
{
    //std::clog << __FUNCTION__ << ", query: " << query.ToStdString() << std::endl;

    ALL_RESULTS result;

    sqlite3_stmt *compiledStatement = NULL;
    // Convert wxString to a C String
    const char *sql = query.c_str();
    
    int error_code = SQLITE_OK;
    // Open database from users filesystem
    error_code = sqlite3_prepare_v2(database, sql, -1, &compiledStatement, NULL);
    if ( error_code != SQLITE_OK) {
        std::cerr << __FUNCTION__ << " Error with code " << error_code << " when preparing query!\n";
    }
    else
    {
        // Loop through results and add them to feeds array
        while (sqlite3_step(compiledStatement) == SQLITE_ROW)
        {
            std::vector<struct myStruct> row;
            int n = sqlite3_column_count(compiledStatement);
            for (int i=0; i<n; i++)
            {
                struct myStruct ms;
                ms.type = sqlite3_column_type(compiledStatement, i);
                //std::cerr << i << " type: " << ms.type << std::endl;

                if (ms.type == SQLITE_TEXT) {
                    const unsigned char *str = sqlite3_column_text(compiledStatement, i);
                    int len = sqlite3_column_bytes(compiledStatement, i);
                    if (str && len > 0)
                        ms.u.c = strdup((const char *)str);
                    row.push_back(ms);
                }
                else if (ms.type == SQLITE_INTEGER) {
                    ms.u.i = sqlite3_column_int(compiledStatement, i);
                    row.push_back(ms);
                }
                else if (ms.type == SQLITE_FLOAT) {
                    ms.u.d = sqlite3_column_double(compiledStatement, i);
                    row.push_back(ms);
                }
                else if (ms.type == SQLITE_NULL) {
                    row.push_back(ms);
                }
                else {
                    std::cerr << __FUNCTION__ << " Unknown data type.\n";
                }
            } // for

            // Add row to array
            result.push_back(row);
        } // while

        // Reset statement (not necessary)
        sqlite3_reset(compiledStatement);
        // Release compiled statement from memory
        sqlite3_finalize(compiledStatement);
    }

    return result;
}
