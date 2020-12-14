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
    g_printData->SetPaperId(wxPAPER_A4);
#ifndef NDEBUG
    wxSize sz = g_printData->GetPaperSize();;
    std::clog << __PRETTY_FUNCTION__ << " paper size:" << sz.x << "," << sz.y << std::endl;
#endif

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

void MainWindow::Draw2(wxPrintout *printout, wxDC *dc, float mmToLogical)
{
    int pageWidthMM, pageHeightMM;
    printout->GetPageSizeMM(&pageWidthMM, &pageHeightMM);
    //wxUnusedVar(pageHeightMM);

    int leftMarginMM = 10;
    int topMarginMM = 10;
    int rightMarginMM = 20;
    int bottomMarginMM = 20;

    float leftMarginLogical = (float)(mmToLogical*leftMarginMM);
    float topMarginLogical = (float)(mmToLogical*topMarginMM);
    float rightMarginLogical = (float)(mmToLogical*(pageWidthMM - rightMarginMM));
    float bottomMarginLogical = (float)(mmToLogical*(pageHeightMM - bottomMarginMM));

    wxCoord xExtent, yExtent;
    dc->GetTextExtent("Test", &xExtent, &yExtent);
    //float xPos = (float)(((((pageWidthMM - leftMargin - rightMargin)/2.0)+leftMargin)*mmToLogical) - (xExtent/2.0));

    std::clog << __PRETTY_FUNCTION__ 
            << "\n\t mmToLogical:" << mmToLogical 
            << "\n\t yExtent:" << yExtent 
            << "\n\t pageMM:" << pageWidthMM << "," << pageHeightMM
            << std::endl;
    
    const wxCoord xPos = (leftMarginMM+10)*mmToLogical;
    const wxCoord xOffset = 92*mmToLogical;
    wxCoord yPos = (topMarginMM+30)*mmToLogical;
    const wxCoord fontSize = 14;
#ifdef __linux__
    const wxCoord lineHeight = 25;
#else
    const wxCoord lineHeight = yExtent; // 15
#endif
    const wxCoord wrapAtChar = 65;

    wxFont m_testFont = wxFontInfo( fontSize ).Family(wxFONTFAMILY_SWISS);
    dc->SetFont(m_testFont);

    // dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);

    int row = mySectionTitles->GetSelectedRow();
    dc->DrawText(mListOfSectionTitles[row], xPos, yPos); // "RZ_2020-12-10T153736"
    dc->DrawText("Page 1 of 1", xPos+xOffset, yPos);
    yPos += 2*lineHeight;
    
    dc->DrawText(myPatientAddressTextField->GetValue(), xPos, yPos);
    dc->DrawText(myOperatorIDTextField->GetValue(), xPos+xOffset, yPos);
    yPos += (5+1)*lineHeight;

    dc->DrawText(myPlaceDateField->GetLabelText(), xPos, yPos);
    
    //wxSize szSignature(90*mmToLogical, 40*mmToLogical);
    wxSize szSignature(rightMarginLogical - (xPos+xOffset), 40*mmToLogical);
    wxPoint ptSignature(xPos+xOffset, yPos);
    wxImage img = mySignView->getSignaturePNG();
    //img.Resize(sz, wxPoint(0,0)); // add border or crop
    //img = img.Scale(sz.x, sz.y, wxIMAGE_QUALITY_HIGH);
    img.Rescale(szSignature.x, szSignature.y, wxIMAGE_QUALITY_HIGH);
    wxBitmap m_bitmap = wxBitmap(img);
    if (m_bitmap.IsOk())
        dc->DrawBitmap( m_bitmap, ptSignature.x, ptSignature.y );
#if 1
    szSignature.IncBy(3);
    dc->SetBrush(*wxTRANSPARENT_BRUSH);
    dc->SetPen(*wxBLACK_PEN);
    dc->DrawRectangle(ptSignature.x, ptSignature.y, szSignature.x, szSignature.y);
#endif

    yPos += szSignature.y;
    yPos += lineHeight;

    dc->SetPen(*wxBLACK_PEN);
    dc->SetBrush(*wxLIGHT_GREY_BRUSH);
    dc->DrawRectangle(leftMarginLogical,
                        yPos,
                        rightMarginLogical,
                        bottomMarginLogical-yPos); //(350*mmToLogical)-yPos);
    dc->SetBrush(*wxTRANSPARENT_BRUSH);
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
        int n = myTextWrapper(str, wrapAtChar);
        dc->DrawText(str, xPos, yPos);
        yPos += lineHeight*(1+n);

//        str.Printf( "%s", item->eanCode );
//        dc.DrawText(str, xPos, yPos);
//        yPos += lineHeight;

        if (item->comment.length() > 0) {
            str.Printf("%s", item->comment);
            int n = myTextWrapper(str, wrapAtChar);
            dc->DrawText(str, xPos, yPos);
            yPos += lineHeight*(1+n);
        }

        yPos += lineHeight;
    }
}

void MainWindow::Draw(wxDC&dc)
{
#ifndef NDEBUG
    std::clog << __PRETTY_FUNCTION__ << std::endl;
    dc.DrawText("Test1", 0, 10);

    if (!dc.IsOk())
        return;

    int w,h;
        dc.GetSize(&w, &h);
        double scaleW,scaleH;
        dc.GetUserScale(&scaleW, &scaleH);
        std::cerr << __FUNCTION__ << " " << __LINE__
                << "\n\t DC size:" << w << "," << h 
                << "\n\t DC scale:" << scaleW << "," << scaleH
                << std::endl;        
#endif
    const wxCoord xPos = 10;
    const wxCoord xOffset = w/2;
    wxCoord yPos = 30;
    const wxCoord fontSize = 14;
    const wxCoord lineHeight = fontSize+2;
    const wxCoord wrapAt = 65;

    dc.SetBackground(*wxWHITE_BRUSH);
    // dc.Clear();
    wxFont m_testFont = wxFontInfo( fontSize ).Family(wxFONTFAMILY_DEFAULT);
#if 1 //ndef NDEBUG
    wxSize szPPI = dc.GetPPI();
    
    std::cerr << __PRETTY_FUNCTION__ 
            << "font size: " << m_testFont.GetFractionalPointSize() // 6
            << "dc PPI: " << szPPI.x << "," << szPPI.y  // 72,72
            << std::endl;
#endif
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
        wxLogError(_("There was a problem previewing.\nPerhaps your current printer is not set correctly?"));
        return;
    }

    wxPreviewFrame *frame =
        new wxPreviewFrame(preview, this, _("Print Preview"), wxPoint(100, 100), wxSize(600, 650));
    frame->Centre(wxBOTH);
    frame->InitializeWithModality(wxPreviewFrame_AppModal);
    
 #if 1
    int ppiScreenX, ppiScreenY;
    preview->GetPrintout()->GetPPIScreen(&ppiScreenX,&ppiScreenY);
    int ppiPrinterX, ppiPrinterY;
    preview->GetPrintout()->GetPPIPrinter(&ppiPrinterX,&ppiPrinterY);
    std::cerr << __FUNCTION__ << " " << __LINE__
            << "\n\t PPI screen:" << ppiScreenX << "," << ppiScreenY // macOS 94,94  linux 300,300
            << "\n\t PPI printer:" << ppiPrinterX << "," << ppiPrinterY // macOS 300,300  linux 1200,1200
            << std::endl;

//    wxDC *dc2 = preview->GetPrintout()->GetDC();
//    if (dc2) {
//        int w,h;
//        dc2->GetSize(&w, &h);
//        double scaleW,scaleH;
//        dc2->GetUserScale(&scaleW, &scaleH);
//        std::cerr << __FUNCTION__ << " " << __LINE__
//                << "\n\t DC size:" << w << "," << h 
//                << "\n\t DC scale:" << scaleW << "," << scaleH
//                << std::endl;        
//    }
#endif

    frame->Show();
}
