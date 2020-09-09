//  PrescriptionsCart.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 16 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <iostream>

#include "PrescriptionsCart.hpp"
#include "InteractionsCart.hpp"
#include "PrescriptionItem.hpp"

PrescriptionsCart::PrescriptionsCart()
{
    interactionsCart = new InteractionsCart;
}

// 48
// Creates and returns a new UUID with RFC 4122 version 4 random bytes
void PrescriptionsCart::makeNewUniqueHash()
{
#if 1
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
    // https://github.com/benwebber/sqlite3-uuid
    //uuidCreate();
#else
    uniqueHash = [[NSUUID UUID] UUIDString];
#endif
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

// 82
void PrescriptionsCart::clearCart()
{
    if (cart.size() > 0) {
        cart.clear();                   // removeAllObjects
        interactionsCart->cart.clear(); // removeAllObjects
        makeNewUniqueHash();
    }
}
