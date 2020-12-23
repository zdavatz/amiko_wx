//  LabelPrintout.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 10 Dec 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include "wx/wx.h"
#include "wx/print.h"

class MainWindow;

class LabelPrintout: public wxPrintout
{
public:
    LabelPrintout(MainWindow* frame, const wxString &title = _("Label"))
        : wxPrintout(title) { m_frame=frame; }

    virtual bool OnPrintPage(int page) wxOVERRIDE;
    virtual bool HasPage(int page) wxOVERRIDE;
    virtual bool OnBeginDocument(int startPage, int endPage) wxOVERRIDE;
    virtual void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo) wxOVERRIDE;
    virtual void OnPreparePrinting() wxOVERRIDE;

private:
    MainWindow *m_frame;
    float logUnitsFactor;
};
