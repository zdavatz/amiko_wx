//  ItemCellView.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 16 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <iostream>

#include "ItemCellView.hpp"
#include "PrescriptionItem.hpp"

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
void ItemCellView::tableViewSelectionDidChange() // NSNotification *)notification
{
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
    
#if 0
    // TODO: if ([notification object] == self.packagesView)
    {
        if (showContextualMenu) {
            NSInteger row = [[notification object] selectedRow];
            if (row < [listOfPackages count]) {
                // Generates contextual menu
                NSMenu *ctxtMenu = [[NSMenu alloc] initWithTitle:@"Contextual Menu"];
                selectedPackage = listOfPackages[row];
                [ctxtMenu insertItemWithTitle:selectedPackage action:nil keyEquivalent:@"" atIndex:0];

                // Populate all menu items
                NSMenuItem *menuItem = [ctxtMenu insertItemWithTitle: NSLocalizedString(@"Prescription", nil)
                                                              action: @selector(selectBasket:)
                                                       keyEquivalent: @""
                                                             atIndex: 1];
                [menuItem setRepresentedObject:[NSNumber numberWithInt:0]];
                /*
                 menuItem = [ctxtMenu insertItemWithTitle:@"Rezept 2" action:@selector(selectBasket:) keyEquivalent:@"" atIndex:2];
                 [menuItem setRepresentedObject:[NSNumber numberWithInt:1]];
                 menuItem = [ctxtMenu insertItemWithTitle:@"Rezept 3" action:@selector(selectBasket:) keyEquivalent:@"" atIndex:3];
                 [menuItem setRepresentedObject:[NSNumber numberWithInt:2]];
                 */
                // Place menu on the screen
                [ctxtMenu popUpMenuPositioningItem:nil atLocation:[NSEvent mouseLocation] inView:nil];
            }
        }
        [self.packagesView reloadData];
    }
#endif
}

// 179
void ItemCellView::selectBasket(int cartNumber)
{
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;

#if 0
    if (selectedPackage.length() > 0 && selectedMedi)
    {
        NSNumber *selectedBasket = (NSNumber *)[sender representedObject];
        int n = [selectedBasket intValue];
        
        // Note: could be replaced by a target-action design pattern (less clear, IMHO)
        NSResponder* r = [self nextResponder];
        while (![r isKindOfClass: [MLMainWindowController class]])
            r = [r nextResponder];

        MainWindowController* vc = (MLMainWindowController*)r;
        
        // 192
        PrescriptionItem *item = new PrescriptionItem;
        item->fullPackageInfo = selectedPackage;
        item->mid = selectedMedi.medId;

        // 196
        // Extract EAN/GTIN
        Medication *m = getShortMediWithId(selectedMedi->medId); //[vc getShortMediWithId:[selectedMedi medId]];
        if (m) {
            NSArray *listOfPackInfos = [[m packInfo] componentsSeparatedByString:@"\n"];
            NSArray *listOfPacks = [[m packages] componentsSeparatedByString:@"\n"];
            wxString eanCode;
            NSInteger row = 0;
            for (wxString s : listOfPackInfos) {
                if ([s containsString:selectedPackage]) {
                    wxString package = [listOfPacks objectAtIndex:row];
                    NSArray *p = [package componentsSeparatedByString:@"|"];
                    eanCode = [p objectAtIndex:9];
                    break;
                }
                row++;
            }
            item.eanCode = eanCode;
        }
        
        NSArray *titleComponents = [selectedPackage componentsSeparatedByString:@"["];
        titleComponents = [titleComponents[0] componentsSeparatedByString:@","];
        if ([titleComponents count]>0) {
            item.title = titleComponents[0];
            if ([titleComponents count]>2) {
                item.price = [NSString stringWithFormat:@"%@ CHF", titleComponents[2]];
                item.price = [item.price stringByReplacingOccurrencesOfString:@"ev.nn.i.H. " withString:@""];
                item.price = [item.price stringByReplacingOccurrencesOfString:@"PP " withString:@""];
            } else {
                item.price = @"";
            }
            [vc addItem:item toPrescriptionCartWithId:n];
        }
    }
#endif
    
#if 0
    // TODO: (not in amiko-osx) deallocate objects
    delete item;
#endif
}
