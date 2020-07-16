//  ItemCellView.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 16 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <wx/wx.h>

class Medication;

class ItemCellView
{
public:
    static ItemCellView * Instance();
    virtual ~ItemCellView() {}
    
    void selectBasket(int cartNumber);
    void tableViewSelectionDidChange();
    
    // 32
    Medication *selectedMedi;
    
    // .m 38
    wxString selectedPackage;
    
private:
    // private constructor so that it can not be called
    ItemCellView() : selectedMedi(nullptr) {}
    
    static ItemCellView* m_pInstance;
};
