//  LabelPrintout.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 10 Dec 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <wx/font.h>

#include "LabelPrintout.hpp"
#include "MainWindow.h"

// Global page setup data
extern wxPageSetupDialogData* g_pageSetupData;

void LabelPrintout::OnPreparePrinting()
{
    SetPageSizeMM(36, 89);
    
    wxDC *dc = GetDC();
    if (!dc || !dc->IsOk())
        return;

    int ppiScreenX, ppiScreenY;
    GetPPIScreen(&ppiScreenX, &ppiScreenY);

    int ppiPrinterX, ppiPrinterY;
    GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);

    float scale = (float)((float)ppiPrinterX/(float)ppiScreenX);

    int w, h;
    dc->GetSize(&w, &h);

    int pageWidth, pageHeight;
    GetPageSizePixels(&pageWidth, &pageHeight);

    float overallScale = scale * (float)(w/(float)pageWidth);
    dc->SetUserScale(overallScale, overallScale);

    logUnitsFactor = (float)(ppiPrinterX/(scale*25.4));
}

bool LabelPrintout::OnPrintPage(int page)
{
    wxDC *dc = GetDC();
    if (!dc || !dc->IsOk())
        return false;

    m_frame->OnDraw_Label(dc, logUnitsFactor);
    return true;
}

bool LabelPrintout::OnBeginDocument(int startPage, int endPage)
{
    if (!wxPrintout::OnBeginDocument(startPage, endPage))
        return false;

    return true;
}

void LabelPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
    *minPage = 1;
    *maxPage = 1;
    *selPageFrom = *minPage;
    *selPageTo = *maxPage;
}

bool LabelPrintout::HasPage(int pageNum)
{
    return (pageNum == 1);
}

