//  SQLiteDatabase.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 16 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <wx/filename.h>

#include <string.h> // for strdup()
#include "SQLiteDatabase.hpp"

SQLiteDatabase::SQLiteDatabase()
: database(nullptr)
{
    //std::cerr << __PRETTY_FUNCTION__ << " constructor, this: " << this << std::endl;
}

// 68
void SQLiteDatabase::initReadOnlyWithPath(wxString path)
{
#ifndef NDEBUG
    //std::clog << __PRETTY_FUNCTION__ << " " << path << std::endl;
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

// 97
// for patient DB
void SQLiteDatabase::initReadWriteWithPath(wxString path)
{
    std::cerr << __PRETTY_FUNCTION__ << " " << path << std::endl;

    // 100
    // Setup database object
    sqlite3 *dbConnection;
    
    // 110
    // Let's open it in read write mode
    int rc = sqlite3_open(path.c_str(), &dbConnection);
    if (rc != SQLITE_OK) {
        std::cerr << __PRETTY_FUNCTION__ << " Unable to open R/W database!\n";
        return;
    }

    // 115
    database = dbConnection;

    // Force using disk for temp storage to reduce memory footprint
    rc = sqlite3_exec(database, "PRAGMA temp_store=1", nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK)
        std::cerr << __PRETTY_FUNCTION__ << ":" << __LINE__ << ", rc " << rc << std::endl;
}

// 122
// for patient DB
bool SQLiteDatabase::createWithPath_andTable_andColumns(wxString path, wxString table, wxString columns)
{
    if (wxFileName::Exists(path)) {
#ifndef NDEBUG
        std::cerr << __FUNCTION__ << " file " << path << " exists already" << std::endl;
#endif
        return true;
    }

#ifndef NDEBUG
    std::cerr << __FUNCTION__ << " create " << path << std::endl;
#endif

    // 129
    // Setup database object
    sqlite3 *dbConnection;
    // 131
    // Database does not exist yet. Let's open and create an empty table
    int rc = sqlite3_open(path.c_str(), &dbConnection);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to create database with path " << path << std::endl;
        return false;
    }

    // 133
    wxString queryStr = wxString::Format("CREATE TABLE IF NOT EXISTS %s %s", table, columns);
    rc = sqlite3_exec(dbConnection, queryStr, nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) {
        // 139
        std::cerr << "Failed to create table " << table << " for database with path %@" << path << std::endl;
        return false;
    }
        
    // 135
#ifndef NDEBUG
    std::clog << table << " table created successfully...\n";
#endif
    database = dbConnection;

    return true;
}

// 151
int SQLiteDatabase::numberRecordsForTable(wxString table)
{
    int numTableRecords = -1;
    sqlite3_stmt *sqlClause = NULL;

    wxString sqlStatement = wxString::Format("SELECT COUNT(*) FROM %s", table);
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
ALL_SQL_RESULTS SQLiteDatabase::performQuery(wxString query)
{
    ALL_SQL_RESULTS result;

    if (!database) {
        std::cerr << __FUNCTION__ << ", database is not open" << std::endl;
        return result;
    }

#ifndef NDEBUG
    //std::cerr << __FUNCTION__ << " query:\n" << query << std::endl;
#endif

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

void SQLiteDatabase::close()
{
    sqlite3_close(database);
}
