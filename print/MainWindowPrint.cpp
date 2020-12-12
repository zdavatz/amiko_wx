//  MainWindowPrint.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 11 Dec 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <stdio.h>

#include <wx/wx.h>
#include <wx/print.h>

#include "MainWindow.h"
#include "SignatureView.hpp"
#include "PrescriptionItem.hpp"
#include "MyPrintout.hpp"

// Global print data, to remember settings during the session
wxPrintData *g_printData = NULL;

// Global page setup data
wxPageSetupDialogData* g_pageSetupData = NULL;

static int myTextWrapper(wxString &s, const int newlinePos)
{
    //const int newlinePos = 60;
    int n = s.length();
    int countNewline = 0;
    for (int i = newlinePos; i<n; i+=newlinePos) {
        s.insert(i, "\n");
        countNewline++;
        //yPos += 10;
    }

    return countNewline;
}

void MainWindow::initPrint()
{
    g_printData = new wxPrintData;
    g_printData->SetPaperId(wxPAPER_A4);    // for everyone else

    g_pageSetupData = new wxPageSetupDialogData;
    // copy over initial paper size from print record
    (*g_pageSetupData) = *g_printData;

    // Set some initial page margins in mm.
    g_pageSetupData->SetMarginTopLeft(wxPoint(15, 15));
    g_pageSetupData->SetMarginBottomRight(wxPoint(15, 15));
}

void MainWindow::terminatePrint()
{
    delete g_printData;
    delete g_pageSetupData;
}

void MainWindow::Draw(wxDC&dc)
{
#ifndef NDEBUG
    std::clog << __PRETTY_FUNCTION__ << " TODO:" << std::endl;
#endif

    const wxCoord xPos = 10;
    const wxCoord xOffset = 127;
    wxCoord yPos = 30;
    const wxCoord lineHeight = 9;
    const wxCoord wrapAt = 65;

    dc.SetBackground(*wxWHITE_BRUSH);
    // dc.Clear();
    wxFont m_testFont = wxFontInfo(6).Family(wxFONTFAMILY_DEFAULT);
    //std::clog << "font size: " << m_testFont.GetFractionalPointSize() << std::endl;
    dc.SetFont(m_testFont);

    // dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);

    int row = mySectionTitles->GetSelectedRow();
    dc.DrawText(mListOfSectionTitles[row], xPos, yPos); // "RZ_2020-12-10T153736"
    dc.DrawText("Page 1 of 1", xPos+xOffset, yPos);
    yPos += 2*lineHeight;

    dc.DrawText(myPatientAddressTextField->GetValue(), xPos, yPos);
    dc.DrawText(myOperatorIDTextField->GetValue(), xPos+xOffset, yPos);
    yPos += 5*lineHeight;

    wxSize sz(90,40);
    wxImage img = mySignView->getSignaturePNG();
    //img.Resize(sz, wxPoint(0,0)); // add border or crop
    //img = img.Scale(sz.x, sz.y, wxIMAGE_QUALITY_HIGH);
    img.Rescale(sz.x, sz.y, wxIMAGE_QUALITY_HIGH);
    wxBitmap m_bitmap = wxBitmap(img);
    if (m_bitmap.IsOk())
        dc.DrawBitmap( m_bitmap, xPos+xOffset, yPos );
#if 1
    sz.IncBy(3);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.SetPen(*wxBLACK_PEN);
    dc.DrawRectangle(xPos+xOffset, yPos, sz.x, sz.y);
#endif

    dc.DrawText(myPlaceDateField->GetLabelText(), xPos, yPos);

    yPos += 5*lineHeight;

    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(*wxLIGHT_GREY_BRUSH);
    dc.DrawRectangle(0, yPos, 230, 350-yPos);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    yPos += lineHeight;

    wxTreeItemId rootItem = myPrescriptionsTableView->GetRootItem();
    if (!rootItem.IsOk())
        return;

    int numRows = myPrescriptionsTableView->GetChildrenCount(rootItem, false);
    std::clog << "numRows: " << numRows << std::endl;

    const int cartNo = 0;
    int cartSize = mPrescriptionsCart[ cartNo].cart.size();
    std::clog << "cartSize: " << cartSize << std::endl;

    for (int i=0; i<cartSize; i++) {
        PrescriptionItem *item = mPrescriptionsCart[ cartNo].getItemAtIndex(i);
        wxString str;

        str.Printf("%s", item->fullPackageInfo);
        int n = myTextWrapper(str, wrapAt);
        dc.DrawText(str, xPos, yPos);
        yPos += lineHeight*(1+n);

//        str.Printf( "%s", item->eanCode );
//        dc.DrawText(str, xPos, yPos);
//        yPos += lineHeight;

        if (item->comment.length() > 0) {
            str.Printf("%s", item->comment);
            int n = myTextWrapper(str, wrapAt);
            dc.DrawText(str, xPos, yPos);
            yPos += lineHeight*(1+n);
        }

        yPos += lineHeight;
    }
}

// 1071
// Reference wxWidgets sample printing.app printing.cpp:384
void MainWindow::printPrescription()
{
    std::clog << __PRETTY_FUNCTION__ << std::endl;
    wxPrintDialogData printDialogData(* g_printData);
    wxPrintPreview *preview =
    new wxPrintPreview(new MyPrintout(this), new MyPrintout(this), &printDialogData);
    if (!preview->IsOk())
    {
        delete preview;
        wxLogError("There was a problem previewing.\nPerhaps your current printer is not set correctly?");
        return;
    }

    wxPreviewFrame *frame =
        new wxPreviewFrame(preview, this, "Print Preview", wxPoint(100, 100), wxSize(600, 650));
    frame->Centre(wxBOTH);
    frame->InitializeWithModality(wxPreviewFrame_AppModal);
    frame->Show();
}
