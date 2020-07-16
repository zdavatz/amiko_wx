//  PrescriptionsCart.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 16 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <iostream>

#include "PrescriptionsCart.hpp"
#include "InteractionsCart.hpp"

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
#else
    uniqueHash = [[NSUUID UUID] UUIDString];
#endif
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
