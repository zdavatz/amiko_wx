//  InteractionsHtmlView.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 9 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <wx/wx.h>

#include "InteractionsHtmlView.hpp"
#include "Medication.hpp"
#include "InteractionsAdapter.hpp"

InteractionsHtmlView::InteractionsHtmlView()
{
    
}

// 43
void InteractionsHtmlView::pushToMedBasket(Medication *med)
{
    if (!med)
        return;

    wxString title = med->title;
#if 1
    std::clog << __FUNCTION__ << " TODO" << std::endl;
#else
    title = [title stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
    if (title.length() > 30) {
        title = [title substringToIndex:30];
        title += wxT("...");
    }
    
    // Add med to medication basket
    [medCart.cart setObject:med forKey:title];
#endif
}

// 68
// Create full interactions HTML
wxString InteractionsHtmlView::fullInteractionsHtml(InteractionsAdapter *interactions)
{
    // --> OPTIMIZE!! Pre-load the following files!
    
    wxString htmlStr;
    
#if 1
    std::clog << __FUNCTION__ << " TODO" << std::endl;
#else
    NSString *colorCss = [MLUtilities getColorCss];

    // Load style sheet from file
    NSString *interactionsCssPath = [[NSBundle mainBundle] pathForResource:@"interactions_css" ofType:@"css"];
    NSString *interactionsCss = [NSString stringWithContentsOfFile:interactionsCssPath encoding:NSUTF8StringEncoding error:nil];
    
    // Load javascript from file
    NSString *jscriptPath = [[NSBundle mainBundle] pathForResource:@"interactions_callbacks" ofType:@"js"];
    NSString *jscriptStr = [NSString stringWithContentsOfFile:jscriptPath encoding:NSUTF8StringEncoding error:nil];
    
    // Generate main interaction table
    htmlStr = [NSString stringWithFormat:@"<html><head><meta charset=\"utf-8\" />"];
    htmlStr = [htmlStr stringByAppendingFormat:@"<script type=\"text/javascript\">%s</script><style type=\"text/css\">%s</style><style type=\"text/css\">%s</style></head><body><div id=\"interactions\">%s<br><br>%s<br>%s</div></body></html>",
               jscriptStr,
               colorCss,
               interactionsCss,
               [self medBasketHtml],
               [self interactionsHtml:interactions],
               [self footNoteHtml]];
    
#if 0 //def DEBUG
    // Create an HTML file of the Fachinfo, so it can be tested with Safari and inspected with an editor
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0]; // Get documents directory
    NSError *error;
    NSString *path = [documentsDirectory stringByAppendingPathComponent:@"interactions.html"];
    BOOL succeed = [htmlStr writeToFile:path
                             atomically:YES
                               encoding:NSUTF8StringEncoding
                                  error:&error];
    if (succeed)
        NSLog(@"Created file: %s", path);
    else
        NSLog(@"%s", error.localizedDescription);
#endif
    
#endif
    
    return htmlStr;
}
