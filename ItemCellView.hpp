//  ItemCellView.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 16 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <wx/wx.h>

class Medication;
class DataObject;

class ItemCellView
{
public:
    static ItemCellView * Instance();
    virtual ~ItemCellView() {}
    
    void selectBasket(int cartNumber);
    void tableViewSelectionDidChange(int row, int packageIndex, DataObject *dobj);
    
    // 32
    DataObject *selectedMedi;
    wxString packagesStr;
    
    // 35
    bool showContextualMenu;
    
    // .m 36
    wxArrayString listOfPackages;

    // .m 38
    wxString selectedPackage;
    
private:
    // private constructor so that it can not be called
    ItemCellView()
    : selectedMedi(nullptr)
    , showContextualMenu(true)
    {}
    
    static ItemCellView* m_pInstance;
};
