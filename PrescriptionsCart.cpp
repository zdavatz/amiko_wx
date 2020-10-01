//  PrescriptionsCart.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 16 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <iostream>

#include <wx/process.h>

#include "PrescriptionsCart.hpp"
#include "InteractionsCart.hpp"
#include "PrescriptionItem.hpp"

PrescriptionsCart::PrescriptionsCart()
{
    interactionsCart = new InteractionsCart;
}

// 48
// Creates and returns a new UUID with RFC 4122 version 4 random bytes
// amiko-osx     uniqueHash = [[NSUUID UUID] UUIDString];
void PrescriptionsCart::makeNewUniqueHash()
{
    wxArrayString output, errors;
    long pid = wxExecute("uuidgen", output, errors);

    size_t count = output.GetCount();
    if (count < 1) {
        for ( size_t n = 0; n < errors.GetCount(); n++ )
            std::cerr << errors[n] << std::endl;
        
        return;
    }

    uniqueHash = output[0];
}

// 66
void PrescriptionsCart::addItemToCart(PrescriptionItem *item)
{
    //if (cart)
    {
        cart.push_back(item);

#if 0 // TODO: define interactionsCart->cart as a std::map ?
        // Add item to interactions cart
        interactionsCart->cart.setObject_forKey(item->med, item->title);
#endif

        std::clog << "Num med in basket: " << cartId << " -> " << cart.size() << std::endl;
    }
}

void PrescriptionsCart::removeItemFromCart(PrescriptionItem *item)
{
    if (cart.size() < 1)
        return;

    cart.erase(std::remove(cart.begin(), cart.end(), item), cart.end());

#if 0 // TODO: define interactionsCart->cart as a std::map ?
    interactionsCart->cart.removeObjectForKey(item->title);
#endif
    std::clog << "Removed med " << item->title << " from basket " << cartId;
}

// 82
void PrescriptionsCart::clearCart()
{
    if (cart.size() > 0) {
        cart.clear();                   // removeAllObjects
        interactionsCart->cart.clear(); // removeAllObjects
        makeNewUniqueHash();
    }
}

// 94
PrescriptionItem * PrescriptionsCart::getItemAtIndex(int index)
{
    if ((index < 0) || (index >= cart.size()))
        return nullptr;

    return cart[index];
}
