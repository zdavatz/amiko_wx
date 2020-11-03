//  TableViewDelegate.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 18 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <wx/colour.h>

#include "TableViewDelegate.hpp"
#include "DataObject.hpp"
#include "MainWindow.h"
#include "ItemCellView.hpp"

extern bool searchStateFullText();

wxIMPLEMENT_DYNAMIC_CLASS(TableViewDelegate, wxHtmlListBox);

TableViewDelegate::TableViewDelegate(wxWindow *parent, bool multi)
: wxHtmlListBox(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                             multi ? wxLB_MULTIPLE : 0)
, m_change(false)
{
    SetMargins(5, 5);
    SetItemCount(0);
    
    // MLCustomTableView.m line 40
    SetSelectionBackground(wxColor(0.8*255, 0.8*255, 255)); // selectBlue
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
    const wxColour lightYellow(255,255,0); // MLColors.m:32

    DataObject *dobj = searchRes[n];

    // ItemCellView.m 123 tableView:viewForTableColumn:row
    wxArrayString listOfPackages = wxSplit(wxString(dobj->subTitle), '\n');

    // favoritesCheckBox
    wxColour starColor = typicalGray;
    wxString starChar = wxString::FromUTF8("☆");
    {
        // MLMainWindowController.m:2801
        MainWindow * parent = wxDynamicCast(m_parent, MainWindow); // GetParent()
        int m = parent->favoriteKeyData.size();
        // Compare index n with count m

        if (n >= m) {
#if 0 //ndef NDEBUG
            std::cerr << "ERROR: === " << __FUNCTION__
            << " n: " << n << " out of bounds: " << m << " === " << std::endl;
#endif
        }
        else
        {
            bool starOn(false);

            if (!searchStateFullText()) {
                wxString regnrStr = parent->favoriteKeyData[n];
                FAVORITES_SET::iterator it = parent->favoriteMedsSet.find(regnrStr);

                starOn = (it != parent->favoriteMedsSet.end());
            }
            else {
                wxString hashId = parent->favoriteKeyData[n];
                FAVORITES_SET::iterator it = parent->favoriteFTEntrySet.find(hashId);

                starOn = (it != parent->favoriteFTEntrySet.end());
            }

            if (starOn) {
                starColor = lightYellow;
                starChar = wxString::FromUTF8("★");
            }
        }
    }
    
    //label += "<STYLE>A {text-decoration: none;} </STYLE>"; // not effective

    // Note the string literal defined as: L"" because it contains a Unicode character
    wxString star_TAG = wxString::Format(L"<font color=%s size=+3>%s</font>",
                                         starColor.GetAsString(wxC2S_HTML_SYNTAX),
                                         starChar);

    wxString title_TAG = wxString::Format("<b><font size=+2> %s</font></b>", dobj->title);
    
    wxString allPackagesList;

    // Encode in the URL the row index and the package index
    unsigned long rowIndex = (unsigned long)n;

    for (int i=0; i<listOfPackages.size(); i++)
    {
        const char *packageColorCSS = "gray";           // default
        if (listOfPackages[i].Contains(", O]"))         // original
            packageColorCSS = "red";
        else if (listOfPackages[i].Contains(", G]"))    // Generika
            packageColorCSS = "green";

        // Packages are clickable
        wxString onePackage_TAG = wxString::Format("<a href='%lu_%d' style=\"color: %s;\">%s</a>",
                                  rowIndex,
                                  i,
                                  packageColorCSS,
                                  listOfPackages[i].c_str());

        allPackagesList += wxString::Format("<li>%s</li>", onePackage_TAG);
    }

    wxString ul_TAG;
    if (listOfPackages.size() > 0)
        ul_TAG = wxString::Format("<ul>%s</ul>", allPackagesList);
    else
        ul_TAG = _("No packages"); // We should never get here

    wxString oneCell_HTML = wxString::Format("<br>%s %s%s", star_TAG, title_TAG, ul_TAG);
    return oneCell_HTML;
}

wxColour TableViewDelegate::GetSelectedTextColour(const wxColour& colFg) const
{
    return m_change ? wxHtmlListBox::GetSelectedTextColour(colFg) : colFg;
}
