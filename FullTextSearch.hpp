//  FullTextSearch.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 24 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <vector>
#include <map>
#include <set>

#define USE_MLIST
#define USE_MDICT

#ifdef USE_MLIST
#define LIST_OF_ARTICLES     mListOfArticles
#else
#define LIST_OF_ARTICLES     listOfArticles
#endif

#ifdef USE_MDICT
#define M_DICT     mDict
#else
#define M_DICT     dict
#endif

class Medication;

class FullTextSearch
{
public:
    FullTextSearch();
    virtual ~FullTextSearch() {}
    
    wxString tableWithArticles_andRegChaptersDict_andFilter(std::vector<Medication *> listOfArticles,
                                                            std::map<wxString, std::set<wxString>> dict,
                                                            wxString filter);
    // 28
    wxArrayString listOfSectionIds;
    wxArrayString listOfSectionTitles;

    // .m 30
#ifdef USE_MLIST
    std::vector<Medication *> mListOfArticles; // do we really need this ?
#endif

#ifdef USE_MDICT
    std::map<wxString, std::set<wxString>> mDict; // do we really need this ?
#endif
};

