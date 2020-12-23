//  PrescriptionPrintout.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 10 Dec 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <wx/font.h>

#include "PrescriptionPrintout.hpp"
#include "MainWindow.h"

// Global page setup data
extern wxPageSetupDialogData* g_pageSetupData;

void PrescriptionPrintout::OnPreparePrinting()
{
#ifndef NDEBUG
    std::clog << __PRETTY_FUNCTION__ << std::endl;
#endif
    // See sample richtextprint.cpp
    
    //m_numPages = 1;
    m_pageEnds.Clear();
}

bool PrescriptionPrintout::OnPrintPage(int page)
{
    wxDC *dc = GetDC();
    if (!dc || !dc->IsOk())
        return false;

    // Get the logical pixels per inch of screen and printer
    int ppiScreenX, ppiScreenY;
    GetPPIScreen(&ppiScreenX, &ppiScreenY);
    int ppiPrinterX, ppiPrinterY;
    GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);

    // This scales the DC so that the printout roughly represents the screen
    // scaling. The text point size _should_ be the right size but in fact is
    // too small for some reason. This is a detail that will need to be
    // addressed at some point but can be fudged for the moment.
    float scale = (float)((float)ppiPrinterX/(float)ppiScreenX);

    // Now we have to check in case our real page size is reduced (e.g. because
    // we're drawing to a print preview memory DC)
    int pageWidth, pageHeight;
    int w, h;
    dc->GetSize(&w, &h);
    GetPageSizePixels(&pageWidth, &pageHeight);

    // If printer pageWidth == current DC width, then this doesn't change. But w
    // might be the preview bitmap width, so scale down.
    float overallScale = scale * (float)(w/(float)pageWidth);
    dc->SetUserScale(overallScale, overallScale);

    // Calculate conversion factor for converting millimetres into logical
    // units. There are approx. 25.4 mm to the inch. There are ppi device units
    // to the inch. Therefore 1 mm corresponds to ppi/25.4 device units. We also
    // divide by the screen-to-printer scaling factor, because we need to
    // unscale to pass logical units to DrawLine.

    float logUnitsFactor = (float)(ppiPrinterX/(scale*25.4));

    m_frame->OnDraw_Prescription(this, dc, logUnitsFactor, page);

    return true;
}

bool PrescriptionPrintout::OnBeginDocument(int startPage, int endPage)
{
    if (!wxPrintout::OnBeginDocument(startPage, endPage))
        return false;

    return true;
}

void PrescriptionPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
    *minPage = 1;
    *selPageFrom = 1;

//    // TODO: CountPages() see sample edit.cpp
//    m_pageEnds.Clear();

    *maxPage = m_numPages;
    *selPageTo = m_numPages;
}

bool PrescriptionPrintout::HasPage(int pageNum)
{
    //return (pageNum <= (int)m_pageEnds.Count());
    return (pageNum > 0) && (pageNum <= m_numPages);
}
