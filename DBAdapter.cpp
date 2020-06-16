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

DBAdapter::DBAdapter()
: mySqliteDb(nullptr)
{
    
}

bool DBAdapter::openDatabase(wxString dbName)
{
    //std::clog << __PRETTY_FUNCTION__ << " " << dbName.ToStdString() << std::endl;

    // A. Check first users documents folder

    wxString documentsDir = wxStandardPaths::Get().GetUserDataDir();
    wxString filePath( documentsDir + wxFILE_SEP_PATH + dbName + ".db");

    if (wxFileName::Exists(filePath)) {
        std::cerr << "AIPS DB found in user's documents folder - " << filePath.ToStdString() << std::endl;
        mySqliteDb = new SQLiteDatabase;
        mySqliteDb->initReadOnlyWithPath(filePath);
        return;
    }
    
    // B. If no database is available, check if db is in app bundle
    documentsDir = wxStandardPaths::Get().GetResourcesDir();
    // TODO:

    return true;
}
