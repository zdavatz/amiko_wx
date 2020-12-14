//  MyPrintout.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 10 Dec 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include "wx/wx.h"
#include "wx/print.h"

#ifndef NDEBUG
#define NUM_PRINT_PAGES 2
#else
#define NUM_PRINT_PAGES 1
#endif

class MainWindow;

class MyPrintout: public wxPrintout
{
public:
    MyPrintout(MainWindow* frame, const wxString &title = "My printout")
        : wxPrintout(title) { m_frame=frame; numPages=NUM_PRINT_PAGES; }

    virtual bool OnPrintPage(int page) wxOVERRIDE;
    virtual bool HasPage(int page) wxOVERRIDE;
    virtual bool OnBeginDocument(int startPage, int endPage) wxOVERRIDE;
    virtual void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo) wxOVERRIDE;

    void DrawPageOne();
    void DrawPageTwo();

    // Writes a header on a page. Margin units are in millimetres.
    bool WritePageHeader(wxPrintout *printout, wxDC *dc, const wxString& text, float mmToLogical);

private:
    MainWindow *m_frame;
    int numPages;
};
