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
    //std::clog << __FUNCTION__ << " listOfArticles size: " << listOfArticles.size() << std::endl;

    int rows = 0;
    wxString htmlStr = wxT("<ul>");

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

    std::map<wxString, int> chaptersCountDict; // chapter name (key), chapter count

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
        // 73
        wxString contentChapters = wxEmptyString;

        // 74
        wxArrayString regnrs = wxSplit(m->regnrs, ',');
        std::map<wxString, wxString> indexToTitlesDict = m->indexToTitlesDict();    // id -> chapter title

        wxString anchor;
        // 78
        // List of chapters
        if (regnrs.size() > 0) {
            wxString r = regnrs[0];
            // 81
            if (M_DICT.find(r) != M_DICT.end()) {
                std::set<wxString> chapters = M_DICT[r];
                // 83
                for (wxString c : chapters) {
                    // 84
                    if (indexToTitlesDict.find(c) != indexToTitlesDict.end()) {
                        wxString cStr = indexToTitlesDict[c];
                        wxString sectionNamePrefix;
                        if (wxAtoi(c) > 100)
                            sectionNamePrefix = "Section";
                        else
                            sectionNamePrefix = "section";

                        anchor = wxString::Format("%s%s", sectionNamePrefix, c);

                        // 94
                        int count = 0;
                        // If already counting chapter occurrences, get previous count
                        if (chaptersCountDict.find(cStr) != chaptersCountDict.end())
                            count = chaptersCountDict[cStr];

                        // 97
                        chaptersCountDict[cStr] = count+1; // bump count

                        if (filter.IsEmpty() || filter == cStr) {
                            contentChapters += wxString::Format("<span style=\"font-size:0.75em; color:#0088BB\"> <a onclick=\"displayFachinfo('%s','%s')\">%s</a></span><br>", m->regnrs, anchor, cStr);
                            filtered = false;
                        }
                    }
                } // for chapters
            }
        }

        // 106
        wxString contentTitle = wxString::Format("<a onclick=\"displayFachinfo('%s','%s')\"><span style=\"font-size:0.8em\"><b>%s</b></span></a> <span style=\"font-size:0.7em\"> | %s</span><br>", m->regnrs, anchor, m->title, m->auth);

        // 107
        if (!filtered) {
            htmlStr += wxString::Format("%s%s%s</li>", contentStyle, contentTitle, contentChapters);
            rows++;
        }
    } // for list of articles
    
    htmlStr += wxT("</ul>");

    // 115
    wxArrayString listOfIds;// = [[NSMutableArray alloc] init];
    wxArrayString listOfTitles;// = [[NSMutableArray alloc] init];

    std::map<wxString, int>::iterator it;
    for (it = chaptersCountDict.begin(); it != chaptersCountDict.end(); ++it) {
        listOfIds.Add(it->first);
        listOfTitles.Add(wxString::Format("%s (%i)", it->first, it->second));
    }

    // Update section ids (anchors)
    listOfSectionIds = listOfIds;
    // Update section titles
    listOfSectionTitles = listOfTitles;
    
    return htmlStr;
}
