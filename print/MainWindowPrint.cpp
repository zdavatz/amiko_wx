//  MainWindowPrint.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 11 Dec 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <stdio.h>

#include <wx/wx.h>
#include <wx/print.h>
#include <wx/math.h>

#include "MainWindow.h"
#include "SignatureView.hpp"
#include "PrescriptionItem.hpp"
#include "PrescriptionPrintout.hpp"
#include "LabelPrintout.hpp"
#include "Label.h"
#include "PatientSheet.h"

// Global print data, to remember settings during the session
wxPrintData *g_printData = NULL;

// Global page setup data
wxPageSetupDialogData* g_pageSetupData = NULL;

static int myTextWrapper(wxString &s, const int newlinePos)
{
    int n = s.length();
    int countNewline = 0;
    for (int i = newlinePos; i<n; i+=newlinePos) {
        s.insert(i, "\n");
        countNewline++;
    }

    return countNewline;
}

#define mm2pix(x)   (x * 842.0 / 297.0)

void MainWindow::initPrint(const wxString &printerName)
{
    g_printData = new wxPrintData;

    if (printerName.IsEmpty())
        g_printData->SetPaperId(wxPAPER_A4);
    else {
        // 1301 printMedicineLabel
        g_printData->SetPaperId(wxPAPER_NONE);
        //g_printData->SetPaperSize(wxSize(mm2pt*36, mm2pt*89));
        g_printData->SetPaperSize(wxSize(220, 180));
        g_printData->SetPrinterName(printerName);
        g_printData->SetOrientation(wxLANDSCAPE);
    }

#ifndef NDEBUG
    wxSize sz = g_printData->GetPaperSize();
    wxString oldPrinterName = g_printData->GetPrinterName();
    std::clog << __PRETTY_FUNCTION__
    << ", old printer name:<" << oldPrinterName << ">"
    << ", printer name:<" << printerName << ">"
    << ", paper size:" << sz.x << "," << sz.y
    << std::endl;
#endif

    g_pageSetupData = new wxPageSetupDialogData;
    // copy over initial paper size from print record
    (*g_pageSetupData) = *g_printData;

    if (printerName.IsEmpty()) {
        // Set some initial page margins in mm.
        g_pageSetupData->SetMarginTopLeft(wxPoint(15, 15));
        g_pageSetupData->SetMarginBottomRight(wxPoint(15, 15));
    }
    else {
        g_pageSetupData->SetMarginTopLeft(wxPoint(0, 0));
        g_pageSetupData->SetMarginBottomRight(wxPoint(150, 150));
    }
}

void MainWindow::terminatePrint()
{
    delete g_printData;
    delete g_pageSetupData;
}

#include "Operator.hpp"
#include "OperatorIDSheet.h"

void MainWindow::DrawLabel1(wxDC&dc)
{
    std::clog << __PRETTY_FUNCTION__ << std::endl;
//    myLabelPanel * medicineLabelView = new myLabelPanel(this);
    
    Operator *d = mOperatorIDSheet->loadOperator();
    
    wxCoord yPos = 10;
    const wxCoord dy = 10;
    const wxCoord fontSize = 14;
    wxFont m_testFont = wxFontInfo( fontSize ).Family(wxFONTFAMILY_SWISS);

    wxString placeDate = myPlaceDateField->GetLabelText();
    // TODO: discard time
    wxString firstLine = wxString::Format("%s %s %s - %s %s", d->title, d->givenName, d->familyName, d->zipCode, placeDate);
    dc.DrawText(firstLine, 0, yPos);
    yPos += 2*dy;

    wxPen pen = dc.GetPen();
    pen.SetWidth(4);
    dc.SetPen(pen);
    dc.DrawLine(0, yPos, 200, yPos);
    yPos += dy;
        
    wxString patient = myPatientAddressTextField->GetLabelText();
    Patient *p = mPatientSheet->getAllFields();
    wxString secondLine = wxString::Format("%s, %s %s", patient, _("born"), p->birthDate);
    dc.DrawText(secondLine, 0, yPos); yPos += dy;

    dc.DrawText("Medicine", 0, yPos); yPos += dy;
    dc.DrawText("Comment", 0, yPos); yPos += dy;
    dc.DrawText("Swissmed", 0, yPos);
    dc.DrawText("Price 1", 200, yPos);

    //medicineLabelView->render(dc);
    delete d;
}

void MainWindow::DrawLabel2(wxPrintout *printout, wxDC *dc, float mmToLogical)
{
    //printout->SetPageSizeMM(120, 90);
#ifndef NDEBUG
    std::clog << __PRETTY_FUNCTION__
            << "\n\t mmToLogical: " << mmToLogical // 3.7
//            << "\n\t yExtent:" << yExtent // 15
//            << "\n\t pageMM:" << pageWidthMM << "," << pageHeightMM // 197, 276
            << std::endl;
#endif
    
    // 1320
    Operator *d = mOperatorIDSheet->loadOperator();

    wxCoord yPos = 10*mmToLogical;
    const wxCoord dy = 10*mmToLogical;
    const wxCoord fontSize = 14;
    
    wxFont m_testFont = wxFontInfo( fontSize ).Family(wxFONTFAMILY_SWISS);
    dc->SetFont(m_testFont);

    wxString placeDate = myPlaceDateField->GetLabelText();
    // TODO: discard time
    wxString firstLine = wxString::Format("%s %s %s - %s %s", d->title, d->givenName, d->familyName, d->zipCode, placeDate);

    dc->DrawText(firstLine, 0, yPos);
    yPos += 2*dy;

    dc->SetPen(* wxGREEN_PEN);
    wxPen pen = dc->GetPen();
    pen.SetWidth(4);
    dc->SetPen(pen);
    dc->DrawLine(0, yPos, 200, yPos);
    yPos += dy;


    dc->DrawText("Patient", 0, yPos); yPos += dy;
    dc->DrawText("Medicine", 0, yPos); yPos += dy;
    dc->DrawText("Comment", 0, yPos); yPos += dy;
    dc->DrawText("Swissmed", 0, yPos);
    dc->DrawText("Price 2", 50*mmToLogical, yPos);
    
    delete d;
}

void MainWindow::DrawPrescription(wxPrintout *printout, wxDC *dc, float mmToLogical)
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

    wxCoord xExtent, yExtent; // mm
    dc->GetTextExtent("Test", &xExtent, &yExtent);

#ifndef NDEBUG
    std::clog << __PRETTY_FUNCTION__ 
            << "\n\t mmToLogical:" << mmToLogical // 3.7
            << "\n\t yExtent:" << yExtent // 15
            << "\n\t pageMM:" << pageWidthMM << "," << pageHeightMM // 197, 276
            << std::endl;
#endif
    
    const wxCoord xPos = (leftMarginMM+10)*mmToLogical;
    const wxCoord xOffset = 92*mmToLogical;
    wxCoord yPos = (topMarginMM+30)*mmToLogical;
    const wxCoord fontSize = 14;
#ifdef __linux__
    const wxCoord lineHeight = 25;
#else
    const wxCoord lineHeight = yExtent; // 15
#endif
    const wxCoord wrapAtChar = 70;

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
    if (img.IsOk()) {
        //img.Resize(sz, wxPoint(0,0)); // add border or crop
        //img = img.Scale(sz.x, sz.y, wxIMAGE_QUALITY_HIGH);
        img.Rescale(szSignature.x, szSignature.y, wxIMAGE_QUALITY_HIGH);
        wxBitmap m_bitmap = wxBitmap(img);
        if (m_bitmap.IsOk())
            dc->DrawBitmap( m_bitmap, ptSignature.x, ptSignature.y );
    }
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

    const int cartNo = 0;
    int cartSize = mPrescriptionsCart[ cartNo].cart.size();
#ifndef NDEBUG
    std::clog
    << "numRows: " << numRows
    << "cartSize: " << cartSize << std::endl;
#endif

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
    dc.DrawText("Draw", 0, 10);

    if (!dc.IsOk())
        return;

    int w,h;
    dc.GetSize(&w, &h);
    double scaleW,scaleH;
    dc.GetUserScale(&scaleW, &scaleH);
    std::cerr << __FUNCTION__ << " " << __LINE__
    << "\n\t DC size:" << w << "," << h  // 510,715
    << "\n\t DC scale:" << scaleW << "," << scaleH // 1.97, 1.97
    << std::endl;

    const wxCoord xPos = 10;
    const wxCoord xOffset = w/2;
    wxCoord yPos = 30;
    const wxCoord fontSize = 14;
    const wxCoord lineHeight = fontSize+2;
    const wxCoord wrapAt = 65;

    dc.SetBackground(*wxWHITE_BRUSH);
    // dc.Clear();
    wxFont m_testFont = wxFontInfo( fontSize ).Family(wxFONTFAMILY_SWISS);
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
    if (img.IsOk()) {
        //img.Resize(sz, wxPoint(0,0)); // add border or crop
        //img = img.Scale(sz.x, sz.y, wxIMAGE_QUALITY_HIGH);
        img.Rescale(sz.x, sz.y, wxIMAGE_QUALITY_HIGH);
        wxBitmap m_bitmap = wxBitmap(img);
        if (m_bitmap.IsOk())
            dc.DrawBitmap( m_bitmap, xPos+xOffset, yPos );
    }
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
#endif
}

// 1071
// Reference wxWidgets sample printing.app printing.cpp:384
void MainWindow::printPrescription()
{
    std::clog << __PRETTY_FUNCTION__ << std::endl;
    initPrint(wxEmptyString);

    wxPrintDialogData printDialogData(* g_printData);
    wxPrintPreview *preview =
    new wxPrintPreview(new PrescriptionPrintout(this), new PrescriptionPrintout(this), &printDialogData);
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
    
#ifndef NDEBUG
    int ppiScreenX, ppiScreenY;
    preview->GetPrintout()->GetPPIScreen(&ppiScreenX,&ppiScreenY);
    int ppiPrinterX, ppiPrinterY;
    preview->GetPrintout()->GetPPIPrinter(&ppiPrinterX,&ppiPrinterY);
    std::cerr << __FUNCTION__ << " " << __LINE__
            << "\n\t PPI screen:" << ppiScreenX << "," << ppiScreenY // macOS 94,94  linux 300,300
            << "\n\t PPI printer:" << ppiPrinterX << "," << ppiPrinterY // macOS 300,300  linux 1200,1200
            << std::endl;
#endif

    frame->Show();
    terminatePrint();
}

// 1381
void MainWindow::printMedicineLabel()
{
    std::clog << __PRETTY_FUNCTION__ << std::endl;
    initPrint("DYMO LabelWriter 450");
    
    wxPrintDialogData printDialogData(* g_printData);
    myLabelPanel * medicineLabelView = new myLabelPanel(this);
    wxPrintout *printout = new LabelPrintout(medicineLabelView);
    printout->SetPageSizeMM(120, 90);

    wxPrintout *printoutForPrinting = new LabelPrintout(medicineLabelView);
    printoutForPrinting->SetPageSizeMM(120, 90);

    wxPrintPreview *preview = new wxPrintPreview(printout, printoutForPrinting, &printDialogData);
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
    
    frame->Show();
    terminatePrint();
}
