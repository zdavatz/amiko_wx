//  InteractionsCart.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 9 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <vector>
#include <map>

#include <wx/wx.h>

class Medication;
class InteractionsAdapter;

class InteractionsCart
{
public:
    InteractionsCart();
    virtual ~InteractionsCart() {}
    
    int size();
    wxString interactionsAsHtmlForAdapter_withTitles_andIds(InteractionsAdapter *adapter, wxArrayString titles, wxArrayString ids);
    
    std::map<wxString, Medication *> cart;
};
