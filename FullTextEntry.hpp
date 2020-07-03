//  FullTextEntry.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 3 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <wx/wx.h>

class FullTextEntry
{
public:
    FullTextEntry();
    virtual ~FullTextEntry() {}
    
    wxString hash;
    wxString keyword;
    wxString regnrs;
};
