//  PrescriptionsCart.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 16 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <vector>
#include <map>

#include <wx/wx.h>

class PrescriptionItem;
class Medication;
class InteractionsCart;
class InteractionsAdapter;

class PrescriptionsCart
{
public:
    PrescriptionsCart();
    virtual ~PrescriptionsCart() {}
    
    void clearCart();
    void makeNewUniqueHash();
    void setInteractionsAdapter(InteractionsAdapter *adapter);
    void addItemToCart(PrescriptionItem *item);
    PrescriptionItem * getItemAtIndex(int index);
    void removeItemFromCart(PrescriptionItem *item);
    
    // 31
    int cartId;
    std::vector<PrescriptionItem *> cart;
    wxString uniqueHash;
    
    // .m 31
    //std::map<wxString, Medication *>
    InteractionsAdapter *interactionsAdapter;
    InteractionsCart *interactionsCart;  // unused ?
};
