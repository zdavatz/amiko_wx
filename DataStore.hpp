//  DataStore.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 8 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <set>
#include <wx/wx.h>

class DataStore
{
public:
    DataStore();
    virtual ~DataStore() {}
    
    std::set<wxString> favMedsSet;
    std::set<wxString> favFTEntrySet;
};
