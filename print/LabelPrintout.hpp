//  LabelPrintout.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 10 Dec 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include "wx/wx.h"
#include "wx/print.h"

//class MainWindow;
class myLabelPanel;

class LabelPrintout: public wxPrintout
{
public:
    LabelPrintout(myLabelPanel* frame, const wxString &title = "My printout")
        : wxPrintout(title) { m_frame=frame; }

    virtual bool OnPrintPage(int page) wxOVERRIDE;
    virtual bool HasPage(int page) wxOVERRIDE;
    virtual bool OnBeginDocument(int startPage, int endPage) wxOVERRIDE;
    virtual void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo) wxOVERRIDE;

    void DrawPageOne();
    void DrawPageTwo();

    // Writes a header on a page. Margin units are in millimetres.
    bool WritePageHeader(wxPrintout *printout, wxDC *dc, const wxString& text, float mmToLogical);

private:
    myLabelPanel *m_frame;
};
