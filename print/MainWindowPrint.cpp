//  MainWindowPrint.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 11 Dec 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <stdio.h>

#include <wx/wx.h>
#include <wx/print.h>
#include <wx/printdlg.h>
#if wxUSE_POSTSCRIPT
    #include "wx/generic/printps.h"
    #include "wx/generic/prntdlgg.h"
#endif
#ifdef __WXMAC__
    #include <wx/osx/printdlg.h>
#endif
#include "wx/generic/prntdlgg.h"
#include <wx/math.h>
#include <wx/tokenzr.h>

#include "MainWindow.h"
#include "SignatureView.hpp"
#include "PrescriptionItem.hpp"
#include "PrescriptionPrintout.hpp"
#include "LabelPrintout.hpp"
#include "PatientSheet.h"

// Global print data, to remember settings during the session
static wxPrintData *g_printData = NULL;

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
    std::clog << __PRETTY_FUNCTION__ << ", " << printerName << std::endl;

    g_printData = new wxPrintData;
    g_pageSetupData = new wxPageSetupDialogData;
    wxString oldPrinterName = g_printData->GetPrinterName();

    if (printerName.IsEmpty()) {
        g_printData->SetPaperId(wxPAPER_A4);
        (*g_pageSetupData) = *g_printData;
    }
    else {
        // 1301 printMedicineLabel
        g_printData->SetPrinterName(printerName);
        g_printData->SetOrientation(wxLANDSCAPE); // wxLANDSCAPE swaps X and Y
        //g_printData->SetPaperSize(wxSize(mm2pt*36, mm2pt*89));
        g_printData->SetPaperSize(wxSize(36, 89));
        g_printData->SetPaperId(wxPAPER_NONE);
#if 1
        // OnPageSetup
        // OnPageSetupPS wxGenericPageSetupDialog has no custom sizes
        {
            (*g_pageSetupData) = * g_printData;

            wxPageSetupDialog pageSetupDialog(this, g_pageSetupData);
            pageSetupDialog.ShowModal();

            (*g_printData) = pageSetupDialog.GetPageSetupDialogData().GetPrintData();
            (*g_pageSetupData) = pageSetupDialog.GetPageSetupDialogData();
#ifdef __WXMAC__
            wxMacPageMarginsDialog pageMarginsDialog(this, g_pageSetupData);
            pageMarginsDialog.ShowModal();

            (*g_printData) = pageMarginsDialog.GetPageSetupDialogData().GetPrintData();
            (*g_pageSetupData) = pageMarginsDialog.GetPageSetupDialogData();
#endif
        }
#else
        // copy over initial paper size from print record
        (*g_pageSetupData) = *g_printData;
#endif
    }

    if (printerName.IsEmpty()) {
        // Set some initial page margins in mm.
        g_pageSetupData->SetMarginTopLeft(wxPoint(15, 15));
        g_pageSetupData->SetMarginBottomRight(wxPoint(15, 15));
    }
    else {
        g_pageSetupData->SetMarginTopLeft(wxPoint(1, 1));
        g_pageSetupData->SetMarginBottomRight(wxPoint(1, 1));
    }

#ifndef NDEBUG
    wxSize sz = g_printData->GetPaperSize();
    wxPoint tl = g_pageSetupData->GetMarginTopLeft();
    wxPoint br = g_pageSetupData->GetMarginBottomRight();
    std::clog << __FUNCTION__
    << "\n\t old printer name:<" << oldPrinterName << ">"
    << "\n\t printer name:<" << printerName << ">"
    << "\n\t paper size: " << sz.x << ", " << sz.y
    << "\n\t paper size pt: " << mm2pt*sz.x << ", " << mm2pt*sz.y
    << "\n\t paper size mm: " << pt2mm*sz.x << ", " << pt2mm*sz.y
    << "\n\t margin TL: " << tl.x << ", " << tl.y
    << "\n\t margin BR: " << br.x << ", " << br.y
    << std::endl;
#endif
}

void MainWindow::terminatePrint()
{
    delete g_printData;
    delete g_pageSetupData;
}

#include "Operator.hpp"
#include "OperatorIDSheet.h"

void MainWindow::OnDraw_Label(wxDC *dc, float mmToLogical)
{
    int row = myPrescriptionsTableView_rowForView();
    if (row == -1)
        return;

    // 1320
    Operator *d = mOperatorIDSheet->loadOperator();

    wxCoord xPos = 5*mmToLogical;
    wxCoord yPos = 0*mmToLogical;
    const wxCoord dy = 4*mmToLogical;
    const wxCoord fontSize = 10;

    wxFont m_testFont = wxFontInfo( fontSize ).Family(wxFONTFAMILY_SWISS);
    dc->SetFont(m_testFont);

    {
        wxString placeDate = myPlaceDateField->GetLabelText();
        placeDate = placeDate.BeforeLast('('); // discard time
        placeDate.Trim();
        wxString firstLine = wxString::Format("%s %s %s - %s %s", d->title, d->givenName, d->familyName, d->zipCode, placeDate);
        dc->DrawText(firstLine, xPos, yPos);
        yPos += dy;
        yPos += 4;

#ifndef __linux__
        //dc->SetPen(* wxGREEN_PEN);
        wxPen pen = dc->GetPen();
        pen.SetWidth(3);
        dc->SetPen(pen); // FIXME: on Linux
#endif
        dc->DrawLine(xPos, yPos, 85*mmToLogical, yPos);
        yPos += 2;
    }

    // 1341 - Second line
    wxString patient = myPatientAddressTextField->GetValue();
    if (!patient.IsEmpty())
    {
        wxArrayString patientArray = wxSplit(patient, '\n');
        Patient *p = mPatientSheet->getAllFields();
        wxString secondLine = wxString::Format("%s, %s %s", patientArray[0], _("born"), p->birthDate);
        dc->DrawText(secondLine, xPos, yPos);
        yPos += dy;
    }

    std::vector<PrescriptionItem *> prescriptionBasket = mPrescriptionsCart[0].cart;
    wxString package = prescriptionBasket[row]->fullPackageInfo;

    // First split package line into left and right, separated by " ["
    wxArrayString packageLR = wxSplit(package, '[');
    for (int i=0; i< packageLR.size(); i++)
        packageLR[i].Trim(true); // trim right

    // Further split left side with separator ", "
    // name, EFP, PP, SL, SB
    wxArrayString leftArray = wxSplit(packageLR[0], ',');
    for (int i=0; i< leftArray.size(); i++)
        leftArray[i].Trim(false); // trim left

    // 1351 Third line
    {
        m_testFont.SetWeight(wxFONTWEIGHT_BOLD);
        dc->SetFont(m_testFont);
        dc->DrawText(leftArray[0], xPos, yPos);
        m_testFont.SetWeight(wxFONTWEIGHT_NORMAL);
        dc->SetFont(m_testFont);
        yPos += dy;
    }

    // 1359 Fourth line
    {
        wxString comment = prescriptionBasket[row]->comment;
        int n = myTextWrapper(comment, 40);
        dc->DrawText(comment, xPos, yPos);
        yPos += (1+n)*dy;
    }

    // Fixed vertical position for bottom line
    yPos = 7.5*dy;

    // 1361 bottom line left
    if (packageLR.size() > 1) { // is there a right side ?
        wxString labelSwissmed = wxString::Format("[%s", packageLR[1]);
        dc->DrawText(labelSwissmed, xPos, yPos);
    }
    
    // 1367 bottom line right
    if (leftArray.size() > 2) {
        wxArrayString pricePP_Array = wxSplit(leftArray[2], ' ');
        if (pricePP_Array.size() > 1 &&
            pricePP_Array[0] == "PP")
        {
            wxString labelPrice = wxString::Format("CHF\t%s", pricePP_Array[1]);
            dc->DrawText(labelPrice, 50*mmToLogical, yPos);
        }
    }
    
    delete d;
}

void MainWindow::OnDraw_Prescription(wxPrintout *printout,
                                      wxDC *dc,
                                      float mmToLogical,
                                      int page)
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

    {
        int row = mySectionTitles->GetSelectedRow();
        if (row != wxNOT_FOUND)
            dc->DrawText(mListOfSectionTitles[row], xPos, yPos); // "RZ_2020-12-10T153736"
    }

    {
        wxString pageCountLabel = wxString::Format("Page %d of %d", page, printPrescriptionNumPages());
        dc->DrawText(pageCountLabel, xPos+xOffset, yPos);
        yPos += 2*lineHeight;
    }
    
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

    // Handle multiple pages: simple pagination
    int firstItem, lastItem;
    getprintItemRangeForPage(page, &firstItem, &lastItem);

    for (int i=firstItem; i<=lastItem; i++) {
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

#define ITEMS_PER_PAGE  5
// Return item index 0 based
void MainWindow::getprintItemRangeForPage(const int page, int *firstItem, int *lastItem)
{
    const int cartNo = 0;
    int cartSize = mPrescriptionsCart[ cartNo].cart.size();
    int lastIndex = cartSize - 1; // zero based

    *firstItem = (page-1)*ITEMS_PER_PAGE;
    *lastItem = page*ITEMS_PER_PAGE - 1;
    
    *firstItem = wxMin(*firstItem, lastIndex);
    *lastItem = wxMin(*lastItem, lastIndex);
}

int MainWindow::printPrescriptionNumPages()
{
    const int cartNo = 0;
    int cartSize = mPrescriptionsCart[ cartNo].cart.size();
    if (cartSize == 0)
        return 0;

    // Simple pagination: each page shows 5 items
    int nPages= (cartSize-1)/ITEMS_PER_PAGE + 1;
    return nPages;
}

// 1071
// Reference wxWidgets sample printing.app printing.cpp:384
void MainWindow::printPrescription()
{
    int nPages = printPrescriptionNumPages();
    if (nPages == 0)
        return; // No medicines in this prescription
    
    initPrint(wxEmptyString);
    
    wxPrintDialogData printDialogData(* g_printData);
    PrescriptionPrintout *printout = new PrescriptionPrintout(this);
    PrescriptionPrintout *printoutForPrinting = new PrescriptionPrintout(this);
 
    printout->m_numPages = printoutForPrinting->m_numPages = nPages;

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
    // TODO: get actual name programmatically
#ifdef __linux__
    initPrint("DYMO-LabelWriter-450");
#else
    initPrint("DYMO LabelWriter 450");
#endif
    
    wxPrintDialogData printDialogData(* g_printData);
    wxPrintout *printout = new LabelPrintout(this);
    wxPrintout *printoutForPrinting = new LabelPrintout(this);

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
    //delete preview; // Don't do this !
}
