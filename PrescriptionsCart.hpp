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

class PrescriptionsCart
{
public:
    PrescriptionsCart();
    virtual ~PrescriptionsCart() {}
    
    void clearCart();
    void makeNewUniqueHash();
    void addItemToCart(PrescriptionItem *item);
    PrescriptionItem * getItemAtIndex(int index);
    void removeItemFromCart(PrescriptionItem *item);
    
    // 31
    int cartId;
    std::vector<PrescriptionItem *> cart;
    wxString uniqueHash;
    
    // .m 32
    //std::map<wxString, Medication *>
    InteractionsCart *interactionsCart;
};
