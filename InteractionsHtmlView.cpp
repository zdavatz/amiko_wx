//  InteractionsHtmlView.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 9 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <wx/wx.h>
#include <wx/stdpaths.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/filename.h>

#include "InteractionsHtmlView.hpp"
#include "Medication.hpp"
#include "InteractionsAdapter.hpp"
#include "Utilities.hpp"
#include "InteractionsCart.hpp"

InteractionsHtmlView::InteractionsHtmlView()
{
    medCart = new InteractionsCart;
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
    
    wxString color_Style = wxString::Format("<style type=\"text/css\">%s</style>", UTI::getColorCss());

    wxString interactions_Style;
    {
        // Load style sheet from file
#ifdef __linux__
        wxFileName f(wxStandardPaths::Get().GetExecutablePath());
        wxString interactionsCssPath(f.GetPath());
#else
        // TODO: use GetResourcesDir()
        wxString interactionsCssPath = wxStandardPaths::Get().GetUserDataDir();
#endif
        interactionsCssPath += wxFILE_SEP_PATH + wxString("interactions.css");

        wxString interactionsCss;
        if (wxFileName::Exists(interactionsCssPath)) {
            wxFileInputStream input( interactionsCssPath );
            wxTextInputStream text(input, wxT("\x09"), wxConvUTF8 );
            while (input.IsOk() && !input.Eof() )
                interactionsCss += text.ReadLine() + wxT("\n");
        }
        
        //
        interactions_Style = wxString::Format("<style type=\"text/css\">%s</style>", interactionsCss);
    }

    wxString jscriptStr;
    {
        // Load javascript from file
#ifdef __linux__
        wxFileName f(wxStandardPaths::Get().GetExecutablePath());
        wxString jscriptPath(f.GetPath());
#else
        // TODO: use GetResourcesDir()
        wxString jscriptPath = wxStandardPaths::Get().GetUserDataDir();
#endif
        jscriptPath += wxFILE_SEP_PATH + wxString("interactions_callbacks.js");

        if (wxFileName::Exists(jscriptPath)) {
            wxFileInputStream input( jscriptPath );
            wxTextInputStream text(input, wxT("\x09"), wxConvUTF8 );
            while (input.IsOk() && !input.Eof() )
                jscriptStr += text.ReadLine() + wxT("\n");
        }
    }
    
    // Generate main interaction table
    htmlStr = "<html><head><meta charset=\"utf-8\" />";
    htmlStr += wxString::Format("<script type=\"text/javascript\">%s</script>%s\n%s</head><body><div id=\"interactions\">%s<br><br>%s<br>%s</div></body></html>",
               jscriptStr,
               color_Style,
               interactions_Style,
            medBasketHtml(),
            interactionsHtml(interactions),
            footNoteHtml());
    
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
    
    return htmlStr;
}

// 117
// Create interaction basket html string
wxString InteractionsHtmlView::medBasketHtml()
{
    wxString medBasketStr;
        
    // basket_html_str + delete_all_button_str + "<br><br>" + top_note_html_str
    int medCnt = 0;
    wxString title = _("Medicines"); // "Medikamentenkorb" "Panier des Médicaments"
        
    medBasketStr += wxString ::Format("<div id=\"Medikamentenkorb\"><fieldset><legend>%s</legend></fieldset></div><table id=\"InterTable\" width=\"100%25\">", title);
    
    // Check if there are meds in the "Medikamentenkorb"
    if (medCart->size() > 0) {
        // First sort them alphabetically
        std::clog << __FUNCTION__ << " line " << __LINE__ << " TODO" << std::endl;
        wxArrayString sortedNames; // TODO = [[medCart.cart allKeys] sortedArrayUsingSelector: @selector(compare:)];
        // Loop through all meds
        for (wxString name : sortedNames) {
            std::clog << __FUNCTION__ << " line " << __LINE__ << " TODO" << std::endl;
            Medication *med;// TODO = [medCart.cart valueForKey:name];
            wxArrayString m_code = wxSplit(med->atccode, ';');
            wxString atc_code = "k.A.";
            wxString active_ingredient = "k.A";

            if (m_code.size()>1) {
                atc_code = m_code[0];
                active_ingredient = m_code[1];
            }

            // Increment med counter
            medCnt++;
            // Update medication basket
            medBasketStr += wxString::Format("<tr>"
                            "<td>%d</td>"
                            "<td>%s</td>"
                            "<td>%s</td>"
                            "<td>%s</td>"
                            "<td align=\"right\"><input type=\"image\" src=\"217-trash.png\" onclick=\"deleteRow('InterTable',this)\" />"
                            "</tr>", medCnt, name, atc_code, active_ingredient);
        }

        // 152
        // Add delete all button
        wxString btnRemove = _("Remove all"); // "Korb leeren" "Tout supprimer"
        medBasketStr += wxString::Format("</table><div id=\"Delete_all\"><input type=\"button\" value=\"%s\" onclick=\"deleteRow('Delete_all',this)\" /></div>", btnRemove);
    }
    else {
        medBasketStr = wxString::Format("<div>%s<br><br></div>",
                                        _("Your medicine basket is empty"));
    }
    
    return medBasketStr;
}


// 170
// Create html displaying interactions between drugs
wxString InteractionsHtmlView::interactionsHtml(InteractionsAdapter *interactions)
{
    wxString interactionStr;
            
#if 1
    std::clog << __FUNCTION__ << " TODO" << std::endl;
#else
    NSMutableString *interactionStr = [[NSMutableString alloc] initWithString:@""];
    NSMutableArray *sectionIds = [[NSMutableArray alloc] initWithObjects:@"Medikamentenkorb", nil];
    NSMutableArray *sectionTitles = nil;
    
    if ([medCart size]>0) {
        if ([MLUtilities isGermanApp])
            sectionTitles = [[NSMutableArray alloc] initWithObjects:@"Medikamentenkorb", nil];
        else if ([MLUtilities isFrenchApp])
            sectionTitles = [[NSMutableArray alloc] initWithObjects:@"Panier des médicaments", nil];
    }
    
    // Check if there are meds in the "Medikamentenkorb"
    NSString *html = [medCart interactionsAsHtmlForAdapter:interactions withTitles:sectionTitles andIds:sectionIds];
    [interactionStr appendString:html];
    
    if ([medCart size]>1) {
        if ([sectionTitles count]<2) {
            [interactionStr appendString:[self topNoteHtml]];
        } else if ([sectionTitles count]>2) {
            [interactionStr appendString:@"<br>"];
        }
    }
    
    if ([medCart size]>0) {
        [sectionIds addObject:@"Farblegende"];
        if ([MLUtilities isGermanApp])
            [sectionTitles addObject:@"Farblegende"];
        else if ([MLUtilities isFrenchApp])
            [sectionTitles addObject:@"Légende des couleurs"];
    }
    
    // Update section title anchors
    listofSectionIds = [NSArray arrayWithArray:sectionIds];
    // Update section titles (here: identical to anchors)
    listofSectionTitles = [NSArray arrayWithArray:sectionTitles];
#endif
    
    return interactionStr;
}

// 228
// TODO: localize
wxString InteractionsHtmlView::footNoteHtml()
{
    wxString legend;
                
    /*
     Risikoklassen
     -------------
     A: Keine Massnahmen notwendig (grün)
     B: Vorsichtsmassnahmen empfohlen (gelb)
     C: Regelmässige Überwachung (orange)
     D: Kombination vermeiden (pinky)
     X: Kontraindiziert (hellrot)
     0: Keine Angaben (grau)
     */
    if (medCart && medCart->size() > 0)
    {
        //if ([MLUtilities isGermanApp])
        {
            wxString legend = {
                "<fieldset><legend>Fussnoten</legend></fieldset>"
                "<p class=\"footnote\">1. Farblegende: </p>"
                "<table id=\"Farblegende\" style=\"background-color:transparent;\" cellpadding=\"3px\" width=\"100%25\">"
                "  <tr bgcolor=\"#caff70\"><td align=\"center\">A</td><td>Keine Massnahmen notwendig</td></tr>"
                "  <tr bgcolor=\"#ffec8b\"><td align=\"center\">B</td><td>Vorsichtsmassnahmen empfohlen</td></tr>"
                "  <tr bgcolor=\"#ffb90f\"><td align=\"center\">C</td><td>Regelmässige Überwachung</td></tr>"
                "  <tr bgcolor=\"#ff82ab\"><td align=\"center\">D</td><td>Kombination vermeiden</td></tr>"
                "  <tr bgcolor=\"#ff6a6a\"><td align=\"center\">X</td><td>Kontraindiziert</td></tr>"
                "</table>"
                "<p class=\"footnote\">2. Datenquelle: Public Domain Daten von EPha.ch.</p>"
                "<p class=\"footnote\">3. Unterstützt durch:  IBSA Institut Biochimique SA.</p>"
            };
            return legend;
        }
        
        /*
        //if ([MLUtilities isFrenchApp])
        {
            wxString legend = {
                "<fieldset><legend>Notes</legend></fieldset>"
                "<p class=\"footnote\">1. Légende des couleurs: </p>"
                "<table id=\"Farblegende\" style=\"background-color:transparent;\" cellpadding=\"3px\" width=\"100%25\">"
                "  <tr bgcolor=\"#caff70\"><td align=\"center\">A</td><td>Aucune mesure nécessaire</td></tr>"
                "  <tr bgcolor=\"#ffec8b\"><td align=\"center\">B</td><td>Mesures de précaution sont recommandées</td></tr>"
                "  <tr bgcolor=\"#ffb90f\"><td align=\"center\">C</td><td>Doit être régulièrement surveillée</td></tr>"
                "  <tr bgcolor=\"#ff82ab\"><td align=\"center\">D</td><td>Eviter la combinaison</td></tr>"
                "  <tr bgcolor=\"#ff6a6a\"><td align=\"center\">X</td><td>Contre-indiquée</td></tr>"
                "</table>"
                "<p class=\"footnote\">2. Source des données : données du domaine publique de EPha.ch.</p>"
                "<p class=\"footnote\">3. Soutenu par : IBSA Institut Biochimique SA.</p>"
            };
            return legend;
        }
         */
    }
    
    return wxEmptyString;
}

