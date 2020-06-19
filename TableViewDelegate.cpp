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
        dc.SetPen(*wxBLACK_DASHED_PEN);
        dc.DrawLine(rect.x, rect.y, rect.GetRight(), rect.y);
        dc.DrawLine(rect.x, rect.GetBottom(), rect.GetRight(), rect.GetBottom());
    }
}

wxString TableViewDelegate::OnGetItem(size_t n) const
{
//    if ( !n && m_firstItemUpdated )
//        return "<h1><b>Just updated</b></h1>";

    int level = n % 6 + 1;

    wxColour clr((unsigned char)(abs((int)n - 192) % 256),
                 (unsigned char)(abs((int)n - 256) % 256),
                 (unsigned char)(abs((int)n - 128) % 256));

    DataObject *m = searchRes[n];
    wxString label = wxString::Format("<b><font color=%s>*</font> %s</b><br>%s",
                                      clr.GetAsString(wxC2S_HTML_SYNTAX),
                                      m->title,
                                      m->subTitle);
    if ( n == 1 ) {
        if ( !m_linkClicked )
            label += "<br><a href='1'>Click here...</a>";
        else
            label += "<br><font color='#9999ff'>Clicked here...</font>";
    }

    return label;
}

wxColour TableViewDelegate::GetSelectedTextColour(const wxColour& colFg) const
{
    return m_change ? wxHtmlListBox::GetSelectedTextColour(colFg) : colFg;
}


