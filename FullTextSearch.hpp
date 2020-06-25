//  FullTextSearch.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 24 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#define NSDictionary        void // TODO:
#define NSArray             void // TODO:

class FullTextSearch
{
public:
    FullTextSearch();
    virtual ~FullTextSearch() {}
    
    wxString tableWithArticles_andRegChaptersDict_andFilter(NSArray *listOfArticles,
                                                            NSDictionary *dict, wxString filter);
};

