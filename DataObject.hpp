//  DataObject.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 19 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

struct DataObject
{
	wxString title;
	wxString subTitle;
    long medId;
    wxString hashId;
    
    DataObject() { medId = 0;} // constructor
};
