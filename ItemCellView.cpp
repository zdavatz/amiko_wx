//  ItemCellView.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 16 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <iostream>

#include <wx/wx.h>
#include <wx/window.h>

#include "ItemCellView.hpp"
#include "PrescriptionItem.hpp"
#include "DataObject.hpp"
#include "MainWindow.h"

ItemCellView* ItemCellView::m_pInstance;

// Singleton (not in amiko-osx)
ItemCellView* ItemCellView::Instance()
{
    if (!m_pInstance) {
        m_pInstance = new ItemCellView;
    }

    return m_pInstance;
}

// 148
void ItemCellView::tableViewSelectionDidChange(int row, int packageIndex, DataObject *dobj) // NSNotification *notification
{
    std::clog << __FUNCTION__
    << " row:" << row
    << " packageIndex:" << packageIndex
    << std::endl;
    
//    if ([notification object] != self.packagesView)
//        return;

    if (!showContextualMenu)
        return;

    // 156
    listOfPackages = wxSplit(wxString(dobj->subTitle), '\n');

    // Validate index
    if (packageIndex >= listOfPackages.size()) {
        std::clog << __FUNCTION__ << " Select cell first" << std::endl;
        return;
    }

    // 154
    // Generates contextual menu
    wxMenu menu;
    menu.SetTitle(_("Contextual Menu")); // Not visible

    selectedPackage = listOfPackages[packageIndex];

    // 159
    // Populate all menu items
    menu.Append(wxID_HIGHEST+0, wxString::Format("%s", selectedPackage));
    menu.Append(wxID_HIGHEST+1, _("Prescription"));
    // TODO: maybe add 2 more prescription carts

    // 171
    // Place menu on the screen
    wxWindow* win = wxTheApp->GetTopWindow();
    const int rc = win->GetPopupMenuSelectionFromUser(menu, wxDefaultPosition);
    if ( rc != wxID_NONE )
    {
        std::clog << "You have selected \"%d\"" << rc - wxID_HIGHEST << std::endl;
        ItemCellView::Instance()->selectBasket(1);
    }

    // TODO: [self.packagesView reloadData];
}

// 179
void ItemCellView::selectBasket(int cartNumber)
{
    std::clog << __PRETTY_FUNCTION__ << std::endl;

    if (selectedPackage.length() > 0 && selectedMedi)
    {
        // 183
        int n = 0; // TODO: confirm always 0 ? See line 164

        // 190
        MainWindow* vc = (MainWindow *)wxTheApp->GetTopWindow();

        // 192
        PrescriptionItem *item = new PrescriptionItem;
        item->fullPackageInfo = selectedPackage;
        item->mid = selectedMedi->medId;

        // 196
        // Extract EAN/GTIN
        Medication *m = vc->getShortMediWithId(selectedMedi->medId);
        if (m) {
            // 199
            wxArrayString listOfPackInfos = wxSplit(wxString(m->packInfo), '\n'); // componentsSeparatedByString
            wxArrayString listOfPacks = wxSplit(wxString(m->packages), '\n');

            wxString eanCode;
            int row = 0;
            for (wxString s : listOfPackInfos) {
                if (s.Contains(selectedPackage)) {
                    wxString package = listOfPacks[row];

                    wxArrayString p = wxSplit(package, '|');

                    //NSArray *p = [package componentsSeparatedByString:@"|"];
                    eanCode = p[9];
                    break;
                }
                row++;
            }
            item->eanCode = eanCode;
        }

        // 215
        wxArrayString titleComponents = wxSplit(selectedPackage, '[');
        titleComponents = wxSplit(titleComponents[0], ',');

        //titleComponents = [titleComponents[0] componentsSeparatedByString:@","];
        if (titleComponents.size() > 0) {
            item->title = titleComponents[0];
            if (titleComponents.size() > 2) {
                wxString price = wxString::Format("%s CHF", titleComponents[2]);
                price.Replace("ev.nn.i.H. ", ""); // stringByReplacingOccurrencesOfString
                price.Replace("PP ", "");
                item->price = price;
            }
            else {
                item->price.clear();
            }

            // 226
            vc->addItem_toPrescriptionCartWithId(item, n);
        }

#if 0
        // TODO: (not in amiko-osx) deallocate objects
        delete item;
#endif
    }
}
