//  FullTextEntry.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 3 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include "FullTextEntry.hpp"

FullTextEntry::FullTextEntry()
{
    
}

// 54
// Unused ?
wxString FullTextEntry::getRegnrs()
{
    wxString regStr;
    std::map<wxString, std::set<wxString>>::iterator it;
    for (it = regChaptersDict.begin(); it != regChaptersDict.end(); ++it) {
        std::set<wxString> value = it->second;
        for (auto s : value)
            regStr += wxString::Format("%s,", s);

        // TODO: std::cout << "key=" << it->first << " value=" << value;
    }

    return regStr;
}

// 67
std::set<wxString> FullTextEntry::getChaptersForKey(wxString &regnr)
{
    return regChaptersDict[regnr];
}

// 72
wxArrayString FullTextEntry::getRegnrsAsArray()
{
    //return [regChaptersDict allKeys];
    wxArrayString regStr;
    std::map<wxString, std::set<wxString>>::iterator it;
    for (it = regChaptersDict.begin(); it != regChaptersDict.end(); ++it)
        regStr.Add( it->first );

    return regStr;
}

// 77
unsigned long FullTextEntry::getNumHits()
{
    return regChaptersDict.size();
}
