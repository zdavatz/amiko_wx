//  InteractionsAdapter.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 17 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <wx/wx.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>

#include "InteractionsAdapter.hpp"

InteractionsAdapter::InteractionsAdapter()
{
    
}

// 36
bool InteractionsAdapter::openInteractionsCsvFile(wxString name)
{
#ifndef NDEBUG
    std::clog << __PRETTY_FUNCTION__ << " " << name.ToStdString() << std::endl;
#endif
     
    // ** A. Check first users documents folder
    wxString documentsDir = wxStandardPaths::Get().GetUserDataDir();
    wxString filePath( documentsDir + wxFILE_SEP_PATH + name + ".csv");
    if (wxFileName::Exists(filePath)) {
        std::clog << "Drug interactions csv found in UserData dir: " << filePath.ToStdString() << std::endl;
        return readDrugInteractionMap(filePath);
        
    }
    
    // ** B. If no database is available, check if db is in app bundle
    documentsDir = wxStandardPaths::Get().GetResourcesDir();
    // TODO:
    
#ifndef NDEBUG
    std::cerr << "Didn't find " << filePath.ToStdString() << std::endl;
#endif
    
    return false;
}

// 86
// Read drug interactions csv line after line
bool InteractionsAdapter::readDrugInteractionMap(wxString filePath)
{
#ifndef NDEBUG
    std::clog << __PRETTY_FUNCTION__ << " " << filePath.ToStdString() << std::endl;
#endif
    return true;
}
