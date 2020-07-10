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

    //std::cerr << __PRETTY_FUNCTION__ << " constructor, this: " << this << std::endl;
}

// 85
bool DBAdapter::openDatabase(wxString dbName)
{
#ifndef NDEBUG
    //std::cerr << __PRETTY_FUNCTION__ << " " << dbName << std::endl;
#endif
    if (!mySqliteDb)
        mySqliteDb = new SQLiteDatabase();

    // A. Check first user's documents folder

    wxString documentsDir = wxStandardPaths::Get().GetUserDataDir();
    wxString filePath( documentsDir + wxFILE_SEP_PATH + dbName + ".db");

    if (wxFileName::Exists(filePath)) {
        std::clog << "AIPS DB found in UserData dir: " << filePath << std::endl;
        mySqliteDb->initReadOnlyWithPath(filePath);
        return true;
    }

    // B. If no database is available, check if DB is in app bundle
    documentsDir = wxStandardPaths::Get().GetResourcesDir();
    // TODO:

#ifndef NDEBUG
    std::cerr << "Didn't find " << filePath << std::endl;
#endif

    return false;
}

// 112
void DBAdapter::closeDatabase()
{
    if (mySqliteDb)
        mySqliteDb->close();
}

// 118
int DBAdapter::getNumRecords()
{
    int numRecords = mySqliteDb->numberRecordsForTable(DATABASE_TABLE);

    return numRecords;
}

// 136
ALL_SQL_RESULTS DBAdapter::getFullRecord(long rowId)
{
    wxString query = wxString::Format("select %s from %s where %s=%ld",
                                      FULL_TABLE,
                                      DATABASE_TABLE,
                                      KEY_ROWID,
                                      rowId);

    return mySqliteDb->performQuery(query);
}

// 144
Medication * DBAdapter::getMediWithId(long rowId)
{
    ALL_SQL_RESULTS results = getFullRecord(rowId);
    ONE_SQL_RESULT firstObject = results[0];
    return cursorToFullMedInfo( firstObject );
}

Medication * DBAdapter::getShortMediWithId(long rowId)
{
    ALL_SQL_RESULTS results = getFullRecord(rowId);
    ONE_SQL_RESULT firstObject = results[0];
    return cursorToShortMedInfo(firstObject);
}

// 154
Medication * DBAdapter::getMediWithRegnr(wxString regnr)
{
    wxString query = wxString::Format("select %s from %s where %s like '%%, %s%%' or %s like '%s%%'", FULL_TABLE, DATABASE_TABLE, KEY_REGNRS, regnr, KEY_REGNRS, regnr);
    ONE_SQL_RESULT cursor = mySqliteDb->performQuery(query)[0];
    
    return cursorToFullMedInfo(cursor);
}

// 169
MEDICATION_RESULTS DBAdapter::searchTitle(wxString title)
{
    wxString query = wxString::Format("select %s from %s where %s like '%s%%' or %s like '%%%s%%'",
            SHORT_TABLE,
            DATABASE_TABLE,
            KEY_TITLE,
            title,
            KEY_TITLE,
            title);

#ifdef __linux__
	if (!mySqliteDb)  // Issue #8 null in Linux
    {
		std::cerr << __PRETTY_FUNCTION__ << " Line " << __LINE__ << std::endl;
        MEDICATION_RESULTS temp;
        return temp;
    }
    else
#endif
	{
		ALL_SQL_RESULTS results = mySqliteDb->performQuery(query);
		return extractShortMedInfoFrom(results);
	}
}

// 177
// Search Inhaber
MEDICATION_RESULTS DBAdapter::searchAuthor(wxString author)
{
    wxString query = wxString::Format("select %s from %s where %s like '%s%%'",
                                      SHORT_TABLE,
                                      DATABASE_TABLE,
                                      KEY_AUTH,
                                      author);
    ALL_SQL_RESULTS results = mySqliteDb->performQuery(query);
    return extractShortMedInfoFrom(results);
}

// 187
// Search ATC Code
MEDICATION_RESULTS DBAdapter::searchATCCode(wxString atccode)
{
    wxString query = wxString::Format("select %s from %s where %s like '%%;%s%%' or %s like '%s%%' or %s like '%% %s%%' or %s like '%%%s%%' or %s like '%%;%%%s%%'",
                                      SHORT_TABLE,
                                      DATABASE_TABLE,
                                      KEY_ATCCODE, atccode,
                                      KEY_ATCCODE, atccode,
                                      KEY_ATCCODE, atccode,
                                      KEY_ATCCLASS, atccode,
                                      KEY_ATCCLASS, atccode);
    ALL_SQL_RESULTS results = mySqliteDb->performQuery(query);
    return extractShortMedInfoFrom(results);
}

// 200
// Search Wirkstoff (unused)
//MEDICATION_RESULTS DBAdapter::searchIngredients(wxString ingredients) {}

// 209
// Search Reg. Nr.
MEDICATION_RESULTS DBAdapter::searchRegNr(wxString regnr)
{
    wxString query = wxString::Format("select %s from %s where %s like '%%, %s%%' or %s like '%s%%'",
                                      SHORT_TABLE,
                                      DATABASE_TABLE,
                                      KEY_REGNRS, regnr,
                                      KEY_REGNRS, regnr);

    ALL_SQL_RESULTS results = mySqliteDb->performQuery(query);
    return extractShortMedInfoFrom(results);
}

// 222
// Search Therapie (unused)
//MEDICATION_RESULTS DBAdapter::searchTherapy(wxString therapy) {}

// 230
// Search Application
MEDICATION_RESULTS DBAdapter::searchApplication(wxString application)
{
    wxString query = wxString::Format("select %s from %s where %s like '%%, %s%%' or %s like '%s%%' or %s like '%% %s%%' or %s like '%%;%s%%' or %s like '%s%%' or %s like '%%;%s%%'",
                                      SHORT_TABLE,
                                      DATABASE_TABLE,
                                      KEY_APPLICATION, application,
                                      KEY_APPLICATION, application,
                                      KEY_APPLICATION, application,
                                      KEY_APPLICATION, application,
                                      KEY_INDICATIONS, application,
                                      KEY_INDICATIONS, application);

    ALL_SQL_RESULTS results = mySqliteDb->performQuery(query);
    return extractShortMedInfoFrom(results);
}

// 240
// Search Reg. Nrs. given a list of reg. nr.
MEDICATION_RESULTS DBAdapter::searchRegnrsFromList(wxArrayString listOfRegnrs)
{
    const unsigned int N = 40;
    MEDICATION_RESULTS listOfMedis;
    
    int C = listOfRegnrs.size();    // E.g. 100
    int capacityA = (C / N) * N;    // E.g. 100/40 * 40 = 80
    int capacityB = C - capacityA;  // 100 - 80 = 20
    wxArrayString listA;// = [NSMutableArray arrayWithCapacity:capacityA];
    wxArrayString listB;// = [NSMutableArray arrayWithCapacity:capacityB];
    
#if 0 // TODO @@@
    [listOfRegnrs enumerateObjectsUsingBlock:^(id object, NSUInteger index, BOOL *stop) {
        NSMutableArray *output = (index < capacityA) ? listA : listB;
        [output addObject:object];
    }];
#else
    int index = 0;
    for (auto object : listOfRegnrs) {
        if (index < capacityA)
            listA.Add(object);
        else
            listB.Add(object);
    }
#endif

    wxString subQuery = wxEmptyString;
    int count = 0;

    // Loop through first (long) list
    for (wxString reg : listA) {
        subQuery += wxString::Format("%s like '%%, %s%%' or %s like '%s%%'",
                                     KEY_REGNRS, reg,
                                     KEY_REGNRS, reg);
        count++;
        if (count % N == 0) {
            wxString query = wxString::Format("select %s from %s where %s",
                                              FULL_TABLE,
                                              DATABASE_TABLE,
                                              subQuery);
            ALL_SQL_RESULTS results = mySqliteDb->performQuery(query);
            for (auto cursor : results) {
                Medication *m = cursorToVeryShortMedInfo(cursor);
                listOfMedis.push_back(m);
            }
            subQuery = wxEmptyString;
        }
        else {
            subQuery += wxT(" or ");
        }
    }

    // Loop through second (short) list
    for (wxString reg : listB) {
        subQuery += wxString::Format("%s like '%%, %s%%' or %s like '%s%%' or ",
                                     KEY_REGNRS, reg,
                                     KEY_REGNRS, reg);
    }

    if (subQuery.size() > 4) {
        // Remove last 'or'
        subQuery.RemoveLast(4);
        wxString query = wxString::Format("select %s from %s where %s",
                                           FULL_TABLE,
                                           DATABASE_TABLE,
                                           subQuery);
        ALL_SQL_RESULTS results = mySqliteDb->performQuery(query);
        for (auto cursor : results) {
            Medication *m = cursorToVeryShortMedInfo(cursor);
            listOfMedis.push_back(m);
        }
    }
    
    return listOfMedis;
}

// 293
Medication * DBAdapter::cursorToVeryShortMedInfo(ONE_SQL_RESULT &cursor)
{
    Medication *medi = new Medication;
    
    medi->medId = cursor[kMedId].u.i;
    medi->title = cursor[kTitle].u.c;
    medi->auth = cursor[kAuth].u.c;
    medi->regnrs = cursor [kRegnrs].u.c;
    medi->sectionIds = cursor[kIdsStr].u.c;
    medi->sectionTitles = cursor[kSectionsStr].u.c;

    return medi;
}

// 307
Medication * DBAdapter::cursorToShortMedInfo(ONE_SQL_RESULT &cursor)
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
    medi->substances = cursor [kSubstances].u.c;
    medi->regnrs = cursor [kRegnrs].u.c;
    medi->atcClass = cursor [kAtcClass].u.c;
    medi->therapy = cursor [kTherapy].u.c;
    medi->application = cursor [kApplication].u.c;
    medi->indications = cursor [kIndications].u.c;
    //medi->customerId = cursor [kCustomerId].u.i;    // TBC do we need to convert c to i first ?;

    // Note that sqlite3 returns type SQLITE_TEXT if the cell is empty
    if (cursor[kCustomerId].type == SQLITE_INTEGER)
        medi->customerId = cursor[kCustomerId].u.i;

    medi->packInfo = cursor[kPackInfo].u.c;
//    [medi setPackages:(NSString *)[cursor objectAtIndex:kPackages]];

    return medi;
}

// 328
Medication * DBAdapter::cursorToFullMedInfo(ONE_SQL_RESULT &cursor)
{
    Medication *medi = cursorToShortMedInfo(cursor);
    // TODO: extra fields. TBC no 'kPackages'

//    [medi setAddInfo:(NSString *)[cursor objectAtIndex:kAddInfo]];
    medi->sectionIds = cursor[kIdsStr].u.c;
    medi->sectionTitles = cursor[kSectionsStr].u.c;
    medi->contentStr = cursor[kContentStr].u.c;
//    [medi setStyleStr:(NSString *)[cursor objectAtIndex:kStyleStr]];

    return medi;
}

// 365
MEDICATION_RESULTS DBAdapter::extractShortMedInfoFrom(ALL_SQL_RESULTS &results)
{
    MEDICATION_RESULTS medList;

    for (auto cursor : results)  {
        Medication *medi = cursorToShortMedInfo(cursor);
        medList.push_back(medi);
    }

    return medList;
}
