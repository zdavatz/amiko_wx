//  DataObject.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 19 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

struct DataObject
{
    char *title;
    char *subTitle;
    long medId;
    char *hashId;
    
    DataObject() {title = subTitle = hashId = nullptr; medId = 0;} // constructor
};
