//  InteractionsAdapter.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 17 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <wx/wx.h>
#include <map>

class InteractionsAdapter
{
public:
    InteractionsAdapter();
    virtual ~InteractionsAdapter() {}
    
    bool openInteractionsCsvFile(wxString name);
    void closeInteractionsCsvFile();
    int getNumInteractions();
    
private:
    bool readDrugInteractionMap(wxString filePath);
    //std::map<std::string, std::string> myDrugInteractionMap;
    std::map<wxString, wxString> myDrugInteractionMap;
};
