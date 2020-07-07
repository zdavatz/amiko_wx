//  FullTextSearch.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 24 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.


#include <wx/wx.h>
#include <wx/arrstr.h>

#include "FullTextSearch.hpp"
#include "Medication.hpp"

FullTextSearch::FullTextSearch()
{
    
}

// 45
// Create HTML to be shown in the webview
wxString FullTextSearch::tableWithArticles_andRegChaptersDict_andFilter(
                        std::vector<Medication *> listOfArticles,
                        std::map<wxString, std::set<wxString>> dict,
                        wxString filter)
{
    std::clog << __FUNCTION__ << " listOfArticles size: " << listOfArticles.size() << std::endl;

    int rows = 0;
    wxString htmlStr = "<ul>";

    // Assign list and dictionaries only if != nil
    if (listOfArticles.size() > 0) {
#ifdef USE_MLIST
        mListOfArticles.clear();
        mListOfArticles = listOfArticles;
#endif
        
#if 0 // TODO: @@@
        // Sort alphabetically (this is pretty neat!)
        NSSortDescriptor *sort = [NSSortDescriptor sortDescriptorWithKey:@"title" ascending:YES];
        LIST_OF_ARTICLES = [LIST_OF_ARTICLES sortedArrayUsingDescriptors:@[sort]];
#endif
    }

#ifdef USE_MDICT
    if (dict.size() > 0)
        mDict = dict;
#endif

    std::map<wxString, wxString> chaptersCountDict; // TBC

    // 63
    // Loop through all articles
    for (Medication *m : LIST_OF_ARTICLES) {
        bool filtered = true;
        wxString contentStyle;
        wxString firstLetter = wxString(m->title, wxConvUTF8).Left(1).Upper();
        if (rows % 2 == 0)
            contentStyle = wxString::Format("<li style=\"background-color:var(--background-color-gray);\" id=\"%s\">", firstLetter);
        else
            contentStyle = wxString::Format("<li style=\"background-color:transparent;\" id=\"%s\">", firstLetter);

        wxString contentChapters = wxEmptyString;

        wxArrayString regnrs = wxSplit(m->regnrs, ',');
        std::map<wxString, wxString> indexToTitlesDict = m->indexToTitlesDict();    // id -> chapter title

        wxString anchor;
        // List of chapters
        if (regnrs.size() > 0) {
            wxString r = regnrs[0];
            if (M_DICT.find(r) != M_DICT.end()) {
                std::set<wxString> chapters = M_DICT[r];
                for (wxString c : chapters) {
                    if (indexToTitlesDict.find(c) != indexToTitlesDict.end()) {
                        wxString cStr = indexToTitlesDict[c];
                        wxString sectionNamePrefix;
                        if (wxAtoi(c) > 100)
                            sectionNamePrefix = "Section";
                        else
                            sectionNamePrefix = "section";

                        anchor = wxString::Format("%s%s", sectionNamePrefix, c);
                        std::clog << __FUNCTION__ << " line " << __LINE__ << " anchor: <" << anchor << ">" << std::endl;


#if 1 // TODO @@@
                        contentChapters += wxT("TODO: chapter link<br>");
#else
                        int count = 0;
                        if (chaptersCountDict [cStr])
                            count = [chaptersCountDict[cStr] intValue];

                        chaptersCountDict[cStr] = [NSNumber numberWithInt:count+1];
                        if (filter isEmpty() || filter == cStr)
                        {
                            contentChapters += wxString::Format("<span style=\"font-size:0.75em; color:#0088BB\"> <a onclick=\"displayFachinfo('%s','%s')\">%s</a></span><br>", m.regnrs, anchor, cStr);
                            filtered = false;
                        }
#endif
                    }
                }
            }
        }

        wxString contentTitle = wxString::Format("<a onclick=\"displayFachinfo('%s','%s')\"><span style=\"font-size:0.8em\"><b>%s</b></span></a> <span style=\"font-size:0.7em\"> | %s</span><br>", m->regnrs, anchor, m->title, m->auth);

#if 0 // TODO @@@
        if (!filtered)
#endif
        {
            htmlStr += wxString::Format("%s%s%s</li>", contentStyle, contentTitle, contentChapters);
            rows++;
        }
    }
    
    htmlStr += "</ul>";
    
#if 0 // TODO: @@@
    NSMutableArray *listOfIds;// = [[NSMutableArray alloc] init];
    NSMutableArray *listOfTitles;// = [[NSMutableArray alloc] init];
    for (wxString cStr : chaptersCountDict) {
        [listOfIds addObject:cStr];
        [listOfTitles addObject:wxString::Format("%s (%s)", cStr, chaptersCountDict[cStr])];
    }
    // Update section ids (anchors)
    listOfSectionIds = [NSArray arrayWithArray:listOfIds];
    // Update section titles
    listOfSectionTitles = [NSArray arrayWithArray:listOfTitles];
#endif
    
    std::clog << __FUNCTION__ << " line " << __LINE__ << " htmlStr: <" << htmlStr << ">" << std::endl;

    return htmlStr;
}
