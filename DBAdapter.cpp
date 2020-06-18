//  dbAdapter.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 16 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <vector>

#include <wx/wx.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>

#include "DBAdapter.hpp"
#include "SQLiteDatabase.hpp"
#include "Medication.hpp"

// 28
enum {
    kMedId = 0, kTitle, kAuth, kAtcCode, kSubstances,
    kRegnrs, kAtcClass, kTherapy, kApplication, kIndications,
    kCustomerId, kPackInfo, kPackages, kAddInfo, kIdsStr,
    kSectionsStr, kContentStr, kStyleStr,
    
    kNumberOfKeys
};

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
MYRESULTS DBAdapter::searchTitle(wxString title)
{
    std::clog << __PRETTY_FUNCTION__ << ", title: " << title.ToStdString() << std::endl;
    
    wxString query = wxString::Format("select %s from %s where %s like '%s%%' or %s like '%%%s%%'",
            SHORT_TABLE.ToStdString(),
            DATABASE_TABLE.ToStdString(),
            KEY_TITLE,
            title.ToStdString(),
            KEY_TITLE,
            title.ToStdString());

    //std::clog << "query: " << query.ToStdString() << std::endl;

    MYRESULTS results = mySqliteDb->performQuery(query);
    return extractShortMedInfoFrom(results);
}

// 177
// Search Inhaber
MYRESULTS DBAdapter::searchAuthor(wxString author)
{
    // TODO:
    MYRESULTS results;
    return results;
}

// 187
// Search ATC Code
MYRESULTS DBAdapter::searchATCCode(wxString atccode)
{
    // TODO:
    MYRESULTS results;
    return results;
}

// 209
// Search Reg. Nr.
MYRESULTS DBAdapter::searchRegNr(wxString regnr)
{
    // TODO:
    MYRESULTS results;
    return results;
}

// 230
// Search Application
MYRESULTS DBAdapter::searchApplication(wxString application)
{
    // TODO:
    MYRESULTS results;
    return results;
}

// 307
Medication * DBAdapter::cursorToShortMedInfo(MYARRAY &cursor)
{
    Medication *medi = new Medication;
    // Type:
    //  0-1  type 1 SQLITE_INTEGER
    //  2-17 type 3 SQLITE_TEXT
    for (int i=0; i<kNumberOfKeys; i++) {
        std::cerr << i << " Type:" << cursor[i].type;
        if (cursor[i].type == SQLITE_INTEGER)
            std::cerr << "  int value: " << cursor[i].u.i;
        else
            std::cerr  << " char value: " << cursor[i].u.c;

        std::cerr << std::endl;
    }

    int medID = cursor[kMedId].u.i;

    return medi;
}

// 365
MYRESULTS DBAdapter::extractShortMedInfoFrom(MYRESULTS &results)
{
    MYRESULTS medList;
    
    for (auto cursor : results)  {
        Medication *medi = cursorToShortMedInfo(cursor);
//        [medList addObject:medi];
    }

    return medList;
}
