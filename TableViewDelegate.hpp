//  TableViewDelegate.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 18 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <vector>

#include <wx/wx.h>
#include "wx/htmllbox.h"

class DataObject;

// to use wxHtmlListBox you must derive a new class from it as you must
// implement pure virtual OnGetItem()
class TableViewDelegate : public wxHtmlListBox
{
public:
    TableViewDelegate() { }
    TableViewDelegate(wxWindow *parent, bool multi = false);

protected:
    // override this method to return data to be shown in the listbox (this is
    // mandatory)
    virtual wxString OnGetItem(size_t n) const wxOVERRIDE;

    // change the appearance by overriding these functions (this is optional)
    virtual void OnDrawSeparator(wxDC& dc, wxRect& rect, size_t n) const wxOVERRIDE;
    virtual wxColour GetSelectedTextColour(const wxColour& colFg) const wxOVERRIDE;

    // flag telling us whether we should use fg colour even for the selected
    // item
    bool m_change;

    // flag which we toggle to update the first items text in OnGetItem()
    //bool m_firstItemUpdated;

public:

    // flag which we toggle when the user clicks on the link in 2nd item
    // to change 2nd item's text
    bool m_linkClicked;
    
    std::vector<DataObject *> searchRes;

#ifdef USE_HTML_FILE
    wxTextFile m_file;
#endif

    wxDECLARE_NO_COPY_CLASS(TableViewDelegate);
    wxDECLARE_DYNAMIC_CLASS(TableViewDelegate);
};

