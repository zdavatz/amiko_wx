//  dbAdapter.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 16 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <vector>
#include <algorithm>    // std::min

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
    kCustomerId, kPackInfo, kPackages,    // short query up to here
    
    // full query includes the following:
    kAddInfo, kIdsStr, kSectionsStr, kContentStr, kStyleStr,
    
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
                      KEY_ROWID, KEY_TITLE, KEY_AUTH, KEY_ATCCODE, KEY_SUBSTANCES, KEY_REGNRS, KEY_ATCCLASS, KEY_THERAPY, KEY_APPLICATION, KEY_INDICATIONS, KEY_CUSTOMER_ID, KEY_PACK_INFO, KEY_PACKAGES,
                      KEY_ADDINFO, KEY_IDS, KEY_SECTIONS, KEY_CONTENT, KEY_STYLE);
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

// 136
ALL_RESULTS DBAdapter::getFullRecord(long rowId)
{
    wxString query = wxString::Format("select %s from %s where %s=%ld",
                                      FULL_TABLE.ToStdString(),
                                      DATABASE_TABLE.ToStdString(),
                                      KEY_ROWID,
                                      rowId);
    
    return mySqliteDb->performQuery(query);
}

// 144
Medication * DBAdapter::getMediWithId(long rowId)
{
    ALL_RESULTS results = getFullRecord(rowId);
    ONE_RESULT firstObject = results[0];
    return cursorToFullMedInfo( firstObject );
}

// 169
std::vector<Medication *> DBAdapter::searchTitle(wxString title)
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

    ALL_RESULTS results = mySqliteDb->performQuery(query);
    return extractShortMedInfoFrom(results);
}

// 177
// Search Inhaber
ALL_RESULTS DBAdapter::searchAuthor(wxString author)
{
    // TODO:
    ALL_RESULTS results;
    return results;
}

// 187
// Search ATC Code
ALL_RESULTS DBAdapter::searchATCCode(wxString atccode)
{
    // TODO:
    ALL_RESULTS results;
    return results;
}

// 209
// Search Reg. Nr.
ALL_RESULTS DBAdapter::searchRegNr(wxString regnr)
{
    // TODO:
    ALL_RESULTS results;
    return results;
}

// 230
// Search Application
ALL_RESULTS DBAdapter::searchApplication(wxString application)
{
    // TODO:
    ALL_RESULTS results;
    return results;
}

// 307
Medication * DBAdapter::cursorToShortMedInfo(ONE_RESULT &cursor)
{
    Medication *medi = new Medication;

#if 0
    // kMedId type 1 SQLITE_INTEGER
    // kCustomerId 3 if empty, 1 if it contains a number
    // others type 3 SQLITE_TEXT
    int n = std::min((int)kNumberOfKeys,    // 18
                     (int)cursor.size());   // 13 for short query
    for (int i=0; i<n; i++) {
        std::cerr << i << " Type:" << cursor[i].type;
        if (cursor[i].type == SQLITE_INTEGER)
            std::cerr << "  int value: " << cursor[i].u.i;
        else
            std::cerr << " char value: <" << cursor[i].u.c << ">";

        std::cerr << std::endl;
    }
#endif

    medi->medId = cursor[kMedId].u.i;
    medi->title = cursor[kTitle].u.c;
    medi->auth = cursor[kAuth].u.c;
    medi->atccode = cursor[kAtcCode].u.c;
//    [medi setSubstances:(NSString *)[cursor objectAtIndex:kSubstances]];
//    [medi setRegnrs:(NSString *)[cursor objectAtIndex:kRegnrs]];
//    [medi setAtcClass:(NSString *)[cursor objectAtIndex:kAtcClass]];
//    [medi setTherapy:(NSString *)[cursor objectAtIndex:kTherapy]];
//    [medi setApplication:(NSString *)[cursor objectAtIndex:kApplication]];
//    [medi setIndications:(NSString *)[cursor objectAtIndex:kIndications]];
//    [medi setCustomerId:[(NSString *)[cursor objectAtIndex:kCustomerId] intValue]];
    
    // Note that sqlite3 returns type SQLITE_TEXT if the cell is empty
    if (cursor[kCustomerId].type == SQLITE_INTEGER)
        medi->customerId = cursor[kCustomerId].u.i;

    medi->packInfo = cursor[kPackInfo].u.c;
//    [medi setPackages:(NSString *)[cursor objectAtIndex:kPackages]];

    return medi;
}

// 328
Medication * DBAdapter::cursorToFullMedInfo(ONE_RESULT &cursor)
{
    Medication *medi = cursorToShortMedInfo(cursor);
    // TODO: extra fields. TBC no 'kPackages'
//    [medi setAddInfo:(NSString *)[cursor objectAtIndex:kAddInfo]];
//    [medi setSectionIds:(NSString *)[cursor objectAtIndex:kIdsStr]];
    medi->sectionTitles = cursor[kSectionsStr].u.c;
    medi->contentStr = cursor[kContentStr].u.c;
//    [medi setStyleStr:(NSString *)[cursor objectAtIndex:kStyleStr]];

    return medi;
}

// 365
std::vector<Medication *> DBAdapter::extractShortMedInfoFrom(ALL_RESULTS &results)
{
    std::vector<Medication *> medList;
    
    for (auto cursor : results)  {
        Medication *medi = cursorToShortMedInfo(cursor);
        medList.push_back(medi);
    }

    return medList;
}
