//  dbAdapter.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 16 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <wx/wx.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>

#include "DBAdapter.hpp"
#include "SQLiteDatabase.hpp"

// 32
static const char * KEY_ROWID = "_id";
static const char * KEY_TITLE = "title";
static const char * KEY_AUTH = "auth";
static const char * KEY_ATCCODE = "atc";
static const char * KEY_SUBSTANCES = "substances";
static const char * KEY_REGNRS = "regnrs";
static const char * KEY_ATCCLASS = "atc_class";
static const char * KEY_THERAPY = "tindex_str";
static const char * KEY_APPLICATION = "application_str";
static const char * KEY_INDICATIONS = "indications_str";
static const char * KEY_CUSTOMER_ID = "customer_id";
static const char * KEY_PACK_INFO = "pack_info_str";
static const char * KEY_ADDINFO = "add_info_str";
static const char * KEY_IDS = "ids_str";
static const char * KEY_SECTIONS = "titles_str";
static const char * KEY_CONTENT = "content";
static const char * KEY_STYLE = "style_str";
static const char * KEY_PACKAGES = "packages";

// 51
static wxString DATABASE_TABLE("amikodb");

// Table columns used for fast queries
static wxString SHORT_TABLE;
static wxString FULL_TABLE;

DBAdapter::DBAdapter()
: mySqliteDb(nullptr)
{
    if (SHORT_TABLE.size() == 0) {
        SHORT_TABLE = wxString::Format("%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",
                       KEY_ROWID, KEY_TITLE, KEY_AUTH, KEY_ATCCODE, KEY_SUBSTANCES, KEY_REGNRS, KEY_ATCCLASS, KEY_THERAPY, KEY_APPLICATION, KEY_INDICATIONS, KEY_CUSTOMER_ID, KEY_PACK_INFO, KEY_PACKAGES);
    }

    if (FULL_TABLE.size() == 0) {
        FULL_TABLE = wxString::Format("%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",
                      KEY_ROWID, KEY_TITLE, KEY_AUTH, KEY_ATCCODE, KEY_SUBSTANCES, KEY_REGNRS, KEY_ATCCLASS, KEY_THERAPY, KEY_APPLICATION, KEY_INDICATIONS, KEY_CUSTOMER_ID, KEY_PACK_INFO, KEY_PACKAGES, KEY_ADDINFO, KEY_IDS, KEY_SECTIONS, KEY_CONTENT, KEY_STYLE);
    }
    
}

// 85
bool DBAdapter::openDatabase(wxString dbName)
{
#ifndef NDEBUG
    std::clog << __PRETTY_FUNCTION__ << " " << dbName.ToStdString() << std::endl;
#endif

    // A. Check first user's documents folder

    wxString documentsDir = wxStandardPaths::Get().GetUserDataDir();
    wxString filePath( documentsDir + wxFILE_SEP_PATH + dbName + ".db");

    if (wxFileName::Exists(filePath)) {
        std::clog << "AIPS DB found in UserData dir: " << filePath.ToStdString() << std::endl;
        mySqliteDb = new SQLiteDatabase;
        mySqliteDb->initReadOnlyWithPath(filePath);
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

// 118
int DBAdapter::getNumRecords()
{
    int numRecords = mySqliteDb->numberRecordsForTable(DATABASE_TABLE);
    
    return numRecords;
}

// 169
MYARRAY DBAdapter::searchTitle(wxString title)
{
    //std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
    
    wxString query = wxString::Format("select %s from %s where %s like '%s%%' or %s like '%%%s%%'",
            SHORT_TABLE.ToStdString(),
            DATABASE_TABLE.ToStdString(),
            KEY_TITLE,
            title.ToStdString(),
            KEY_TITLE,
            title.ToStdString());

    MYARRAY results = mySqliteDb->performQuery(query);
    return results;
}
