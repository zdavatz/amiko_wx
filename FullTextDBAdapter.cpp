//  FullTextDBAdapter.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 17 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <unordered_set>
#include <map>

#include <wx/wx.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>

#include "FullTextDBAdapter.hpp"
#include "SQLiteDatabase.hpp"
#include "FullTextEntry.hpp"

// 29
enum {
    kRowId = 0, kKeyword, kRegnrs
};

// 32
static const char *KEY_ROWID = "id";
static const char *KEY_KEYWORD = "keyword";
static const char *KEY_REGNR = "regnr";

static wxString DATABASE_TABLE("frequency");

FullTextDBAdapter::FullTextDBAdapter()
: myFullTextDb(nullptr)
{
    //std::cerr << __PRETTY_FUNCTION__ << " constructor, this: " << this << std::endl;
}

// 49
// TODO: unify repeated code from class DBAdapter
bool FullTextDBAdapter::openDatabase(wxString dbName)
{
#ifndef NDEBUG
    //std::cerr << __PRETTY_FUNCTION__ << " " << dbName << std::endl;
#endif
    // A. Check first users documents folder
    wxString documentsDir = wxStandardPaths::Get().GetUserDataDir();
    wxString filePath( documentsDir + wxFILE_SEP_PATH + dbName + ".db");
    if (wxFileName::Exists(filePath)) {
        std::clog << "Fulltext DB found in UserData dir: " << filePath << std::endl;
        if (!myFullTextDb)
            myFullTextDb = new SQLiteDatabase();

        myFullTextDb->initReadOnlyWithPath(filePath);
        return true;
    }

    // B. If no database is available, check if db is in app bundle
    documentsDir = wxStandardPaths::Get().GetResourcesDir();
    // TODO:

#ifndef NDEBUG
    std::cerr << "Didn't find " << filePath << std::endl;
#endif

    return false;
}

// 76
void FullTextDBAdapter::closeDatabase()
{
    if (myFullTextDb)
        myFullTextDb->close();
}

// 82
int FullTextDBAdapter::getNumRecords()
{
    return myFullTextDb->numberRecordsForTable(DATABASE_TABLE);
}

// 87
// Get full text from hash
FullTextEntry * FullTextDBAdapter::searchHash(wxString hash)
{
    wxString query = wxString::Format("select * from %s where %s like '%s'", DATABASE_TABLE, KEY_ROWID, hash);

    ALL_SQL_RESULTS results = myFullTextDb->performQuery(query);
    return cursorToFullTextEntry(results[0]);
}

// 97
// Search fulltext containing keyword
FULLTEXT_RESULTS FullTextDBAdapter::searchKeyword(wxString keyword)
{
    wxString query = wxString::Format("select * from %s where %s like '%s%%'",
                                      DATABASE_TABLE,
                                      KEY_KEYWORD,
                                      keyword.c_str());

    ALL_SQL_RESULTS results = myFullTextDb->performQuery(query);
    return extractFullTextEntryFrom(results);
}

// 107
FullTextEntry *
FullTextDBAdapter::cursorToFullTextEntry(ONE_SQL_RESULT &cursor)
{
    FullTextEntry *entry = new FullTextEntry;
    
    entry->hash = cursor[kRowId].u.c; // TBC .u.i
    entry->keyword = cursor[kKeyword].u.c;

    wxString regnrsAndChapters = cursor [kRegnrs].u.c;

    if (!regnrsAndChapters.IsEmpty()) {
        std::map<wxString, std::set<wxString>> dict = regChapterDict(regnrsAndChapters);
        entry->regChaptersDict = dict;
    }

    entry->regnrs = regnrsAndChapters;
    
    return entry;
}

// 123
FULLTEXT_RESULTS FullTextDBAdapter::extractFullTextEntryFrom(ALL_SQL_RESULTS &results)
{
    std::clog << __FUNCTION__ << " results size: " << results.size() << std::endl;

    FULLTEXT_RESULTS entryList;
    
    for (auto cursor : results) {
        
        // TODO: assert(cursor);
        
        FullTextEntry *entry = new FullTextEntry;
        entry->hash = cursor[kRowId].u.c;
        entry->keyword = cursor[kKeyword].u.c;
        wxString regnrsAndChapters = cursor[kRegnrs].u.c;
        if (!regnrsAndChapters.IsEmpty()) {
            std::map<wxString, std::set<wxString>> dict = regChapterDict(regnrsAndChapters);
            entry->regChaptersDict = dict;
        }

        entry->regnrs = regnrsAndChapters;
        
        entryList.push_back(entry);
    }
    
    return entryList;
}

// 147
//- (NSMutableDictionary *) regChapterDict:(NSString *)regChapterStr
std::map<wxString, std::set<wxString>>
FullTextDBAdapter::regChapterDict(wxString regChapterStr)
{
    //std::clog << __FUNCTION__ << " regChapterStr: <" << regChapterStr << ">" << std::endl;

    wxString regnr;
    wxString chapters;
    std::map<wxString, std::set<wxString>> dict;   // regnr -> set of chapters
    std::map<wxString, std::set<wxString>>::iterator it;

    // Format: 65000(13)|65001(14)|...
    wxArrayString rac = wxSplit(regChapterStr ,'|');

    std::unordered_set<wxString> set;
    for (wxString r : rac)
        set.insert(r);

    // Loop through all regnr-chapter pairs
    for (wxString r : set) {
        // Extract chapters located between parentheses
        wxArrayString str1 = wxSplit(r, '(');
        if (str1.size() > 0) {
            regnr = str1[0];
            if (str1.size() > 1) {
                wxArrayString str2 = wxSplit(str1[1], ')');
                chapters = str2[0];
            }

            // 165
            std::set<wxString> chaptersSet;
            it = dict.find(regnr);
            if (it != dict.end()) {
                // 167
                std::set<wxString> mySet = it->second;
                for (auto s : mySet)
                    chaptersSet.insert(s);
            }

            // 169
            // Split chapters listed as comma-separated string
            wxArrayString c = wxSplit(chapters, ',');
            // c: 13
            for (wxString chapter : c)
                chaptersSet.insert(chapter);

            // Update dictionary
            dict[regnr] = chaptersSet;
        }
        else {
            // No chapters for this regnr -> do nothing
        }
    }

#if 0
    std::clog << "Line " << __LINE__ << " dict.size(): " << dict.size() << std::endl;
    for (it = dict.begin(); it != dict.end(); ++it) {
        std::set<wxString> setVerify = it->second;
        std::cout << '\t' << it->first << ", set: ";
        for (auto s : setVerify)
            std::cout  << s << ",";

        std::cout << '\n';
    }

    std::cout << std::endl;
#endif
    return dict;
}
