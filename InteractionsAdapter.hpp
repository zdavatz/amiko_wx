//  InteractionsAdapter.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 17 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <wx/wx.h>

class InteractionsAdapter
{
public:
    InteractionsAdapter();
    virtual ~InteractionsAdapter() {}
    
    bool openInteractionsCsvFile(wxString name);
    
private:
    bool readDrugInteractionMap(wxString filePath);
};
