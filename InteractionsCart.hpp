//  InteractionsCart.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 9 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <vector>

class InteractionsCart
{
public:
    InteractionsCart();
    virtual ~InteractionsCart() {}
    
    int size();
    
    //NSMutableDictionary *
    std::vector<char*> cart;
};
