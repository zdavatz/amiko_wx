//  TableViewDelegate.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 18 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include "TableViewDelegate.hpp"
#include "DataObject.hpp"
#include "MainWindow.h"

extern bool searchStateFullText();

wxIMPLEMENT_DYNAMIC_CLASS(TableViewDelegate, wxHtmlListBox);

TableViewDelegate::TableViewDelegate(wxWindow *parent, bool multi)
             : wxHtmlListBox(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                             multi ? wxLB_MULTIPLE : 0)
{
    m_change = true;

    SetMargins(5, 5);
    SetItemCount(0);
}

void TableViewDelegate::OnDrawSeparator(wxDC& dc, wxRect& rect, size_t) const
{
    dc.SetPen(*wxBLACK_PEN);
    dc.DrawLine(rect.x, rect.GetBottom(), rect.GetRight(), rect.GetBottom());
}

// See MLMainWindowController.m:2783 tableView:viewForTableColumn:row
// In AmiKo-osx this is a handler for 3 table views
//      myTableView, mySectionTitles, myPrescriptionsTableView
wxString TableViewDelegate::OnGetItem(size_t n) const
{
	//std::cerr << "=== n:" << n << " === " << __PRETTY_FUNCTION__ << std::endl;
    
    const wxColour typicalGray(127,127,127); // MLColors.m:26
    const wxColour typicalGreen(0,0.8F*255,0.2F*255);
    const wxColour typicalRed(255,0,0);
    const wxColour lightYellow(255,255,0); // MLColors.m:32

    DataObject *dobj = searchRes[n];

    // ItemCellView.m 123 tableView:viewForTableColumn:row
    wxArrayString listOfPackages = wxSplit(wxString(dobj->subTitle), '\n');

    // favoritesCheckBox
    wxColour starColor = typicalGray;
    {
        // MLMainWindowController.m:2801
        MainWindow * parent = wxDynamicCast(m_parent, MainWindow); // GetParent()
        int m = parent->favoriteKeyData.size();
        // Compare index n with count m
#if 0
        wxASSERT_MSG( n < m, wxT("item index out of bounds") );
#else
        if (n >= m) {
            std::cerr << "ERROR: === " << __FUNCTION__
            << " n:" << n << " out of bounds: " << m << " === " << std::endl;
        }
        else
#endif
        {
            if (!searchStateFullText()) {
                wxString regnrStr = parent->favoriteKeyData[n];
                FAVORITES_SET::iterator it = parent->favoriteMedsSet.find(regnrStr);
                if (it != parent->favoriteMedsSet.end())
                    starColor = lightYellow;
            }
            else {
                wxString hashId = parent->favoriteKeyData[n];
                FAVORITES_SET::iterator it = parent->favoriteFTEntrySet.find(hashId);
                if (it != parent->favoriteFTEntrySet.end())
                    starColor = lightYellow;
            }
        }
    }
    
    //label += "<STYLE>A {text-decoration: none;} </STYLE>"; // not effective

    // Note the string literal defined as: L"" because it contains a Unicode character
    wxString star_TAG = wxString::Format(L"<font color=%s size=+3>★</font>",
                                         starColor.GetAsString(wxC2S_HTML_SYNTAX));

    wxString title_TAG = wxString::Format("<b><font size=+2> %s</font></b>", dobj->title);
    
    wxString allPackagesList;

    for (int i=0; i<listOfPackages.size(); i++) {
        // Set colors: O original red, G Generika green, default gray
        wxColour packageColor = typicalGray;
        const char *packageColorCSS = "gray";
        if (listOfPackages[i].Contains(", O]")) {
            packageColor = typicalRed;
            packageColorCSS = "red";
        }
        else if (listOfPackages[i].Contains(", G]")) {
            packageColor = typicalGreen;
            packageColorCSS = "green";
        }

        // Packages are clickable
#if 0
        // This way we lose the color coding and gain an ugly underline
        wxString onePackage_TAG = wxString::Format("<br><font color=%s><a href='%d'>%s</a></font>",
                                  packageColor.GetAsString(wxC2S_HTML_SYNTAX),
                                  i,
                                  listOfPackages[i].c_str());
#else
        // Ok color coding but adding "text-decoration: none;" is not effective in removing the underline
        wxString onePackage_TAG = wxString::Format("<a href='%d' style=\"color: %s;\">%s</a>",
                                  i,
                                  packageColorCSS,
                                  listOfPackages[i].c_str());
#endif

        allPackagesList += wxString::Format("<li>%s</li>", onePackage_TAG);
    }

    wxString ul_TAG = wxString::Format("<ul>%s</ul>", allPackagesList);
    wxString oneCell_HTML = wxString::Format("<br>%s %s%s", star_TAG, title_TAG, ul_TAG);
    return oneCell_HTML;
}

wxColour TableViewDelegate::GetSelectedTextColour(const wxColour& colFg) const
{
    return m_change ? wxHtmlListBox::GetSelectedTextColour(colFg) : colFg;
}


