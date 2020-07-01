//  TableViewDelegate.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 18 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include "TableViewDelegate.hpp"
#include "DataObject.hpp"

wxIMPLEMENT_DYNAMIC_CLASS(TableViewDelegate, wxHtmlListBox);

TableViewDelegate::TableViewDelegate(wxWindow *parent, bool multi)
             : wxHtmlListBox(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                             multi ? wxLB_MULTIPLE : 0)
{
    m_change = true;
    //m_firstItemUpdated = false;
    m_linkClicked = false;

    SetMargins(5, 5);
    SetItemCount(0);
}

void TableViewDelegate::OnDrawSeparator(wxDC& dc, wxRect& rect, size_t) const
{
#if 0 // TODO:
    if ( ((MyFrame *)GetParent())->
            GetMenuBar()->IsChecked(HtmlLbox_DrawSeparator) )
#endif
    {
        dc.SetPen(*wxBLACK_PEN);
        dc.DrawLine(rect.x, rect.y, rect.GetRight(), rect.y);
        dc.DrawLine(rect.x, rect.GetBottom(), rect.GetRight(), rect.GetBottom());
    }
}

// see MLMainWindowController.m:2783 tableView:viewForTableColumn:row
wxString TableViewDelegate::OnGetItem(size_t n) const
{
	//std::cerr << "=== n:" << n << " === " << __PRETTY_FUNCTION__ << std::endl;
    
    const wxColour typicalGray(127,127,127); // MLColors.m:26
    const wxColour typicalGreen(0,0.8F*255,0.2F*255);
    const wxColour typicalRed(255,0,0);
    const wxColour lightYellow(255,255,0); // MLColors.m:32

    DataObject *m = searchRes[n];

    // MLItemCellView.m 120 tableView:viewForTableColumn:row
    wxArrayString listOfPackages = wxSplit(wxString(m->subTitle), '\n');

    wxColour starColor = lightYellow;
    if (n % 5)
        starColor = typicalGray;
    
    wxString label;
    //label += "<STYLE>A {text-decoration: none;} </STYLE>"; // not effective

    // Note the string literal defined as: L"" because it contains a Unicode character
    label += wxString::Format(L"<font color=%s size=+3>★</font>",
								starColor.GetAsString(wxC2S_HTML_SYNTAX));

    label += wxString::Format("<b><font size=+2> %s</font></b>", m->title);

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
#if 0
        label += wxString::Format("<br><font color=%s>%s</font>",
                                  packageColor.GetAsString(wxC2S_HTML_SYNTAX),
                                  listOfPackages[i].c_str());
#else
        // Packages are clickable
        
        // This way we lose the color coding and gain an ugly underline
//        label += wxString::Format("<br><font color=%s><a href='%d'>%s</a></font>",
//                                  packageColor.GetAsString(wxC2S_HTML_SYNTAX),
//                                  i,
//                                  listOfPackages[i].c_str());

        // Ok color coding but adding "text-decoration: none;" is not effective in removing the underline
        label += wxString::Format("<br><a href='%d' style=\"color: %s;\">%s</a>",
                                  i,
                                  packageColorCSS,
                                  listOfPackages[i].c_str());
#endif
    }

    return label;
}

wxColour TableViewDelegate::GetSelectedTextColour(const wxColour& colFg) const
{
    return m_change ? wxHtmlListBox::GetSelectedTextColour(colFg) : colFg;
}


