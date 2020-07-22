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
    if (!med) {
#ifndef NDEBUG
        std::clog << __FUNCTION__ << " no med, early return\n";
#endif
        return;
    }

    wxString title = med->title;

    title.Trim(true);
    title.Trim(false);
    if (title.length() > 30) {
        title = title.Left(30);
        title += wxT("...");
    }
    
    // TODO: escape ',' because in OnTitleChanged() we split the title by comma,
    // and it would add elements, eventually causing.
    // A better way would be to use JSON for when setting the title
    std::clog << __FUNCTION__ << " title:<" << title << ">" << std::endl;
    title.Replace(",", "_", true);

    // Add med to medication basket
    medCart->cart[title] = med;
}

// 58
void InteractionsHtmlView::removeFromMedBasketForKey(wxString key)
{
    medCart->cart.erase(key); //removeObjectForKey:key
}

// 63
void InteractionsHtmlView::clearMedBasket()
{
    medCart->cart.clear(); //removeAllObjects
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
        
    medBasketStr += wxString ::Format("<div id=\"Medikamentenkorb\"><fieldset><legend>%s</legend></fieldset></div><table id=\"InterTable\" width=\"100%%25\">", title);
    
    // Check if there are meds in the "Medikamentenkorb"
    if (medCart->size() > 0) {
        // 129
        // TODO: First sort them alphabetically
        // [[medCart.cart allKeys] sortedArrayUsingSelector: @selector(compare:)];
        wxArrayString sortedNames;
        std::map<wxString, Medication *>::iterator it;
        for (it = medCart->cart.begin(); it != medCart->cart.end(); ++it)
            sortedNames.Add( it->first );

        // Loop through all meds
        for (auto name : sortedNames) {
            Medication *med = medCart->cart[name];
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
        wxString btnRemove = _("Remove all");
        medBasketStr += wxString::Format("</table><div id=\"Delete_all\"><input type=\"button\" value=\"%s\" onclick=\"deleteRow('Delete_all',this)\" /></div>", btnRemove);
    }
    else {
        medBasketStr = wxString::Format("<div>%s<br><br></div>",
                                        _("Your medicine basket is empty"));
    }
    
    return medBasketStr;
}

// 170
// Create HTML displaying interactions between drugs
wxString InteractionsHtmlView::interactionsHtml(InteractionsAdapter *interactions)
{
    wxString interactionStr;

    wxArrayString sectionIds;
    sectionIds.Add("Medikamentenkorb");  // Do not localize, because it's an HTML tag ID

    wxArrayString sectionTitles;
    if (medCart->size() > 0)
        sectionTitles.Add(_("Medicines"));
    
    // Check if there are meds in the "Medikamentenkorb"
    wxString html = medCart->interactionsAsHtmlForAdapter_withTitles_andIds(interactions, sectionTitles, sectionIds);

    interactionStr += html;
    
    // 187
    if (medCart->size() > 1) {
        if (sectionTitles.size() < 2) {
            interactionStr += topNoteHtml();
        } else if (sectionTitles.size() > 2) {
            interactionStr += "<br>";
        }
    }
    
    // 195
    if (medCart->size() > 0) {
        sectionIds.Add("Farblegende"); // Do not localize, because it's an HTML tag ID

        sectionTitles.Add(_("Colour legend"));
    }
    
    // Update section title anchors
    listofSectionIds = sectionIds;
    // Update section titles (here: identical to anchors)
    listofSectionTitles = sectionTitles;
    
    return interactionStr;
}

// 211
wxString InteractionsHtmlView::topNoteHtml()
{
    
    if (medCart->size() <= 1)
        return wxEmptyString;

    // Add note to indicate that there are no interactions

    wxString knownInteractions = _("Known Interactions");

    wxString note = _("There are currently no interactions between these drugs in the EPha.ch database. For more information, see the technical information.");
    
    wxString buttonLabel = _("Report interaction");

    wxString topNote = wxString::Format("<fieldset><legend>%s</legend></fieldset><p class=\"paragraph0\">%s</p><div id=\"Delete_all\"><input type=\"button\" value=\"%s\" onclick=\"deleteRow('Notify_interaction',this)\" /></div><br>", knownInteractions, note, buttonLabel);
    
    return topNote;
}

// 228
wxString InteractionsHtmlView::footNoteHtml()
{
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
    if (medCart && medCart->size() <= 0)
        return wxEmptyString;

    // "Fussnoten"
    // "Notes"
    wxString footNote = _("Footnotes");

    wxString note1 = _("Colour legend");

    wxString colorA = _("No measures necessary");
    wxString colorB = _("Precautionary measures recommended");
    wxString colorC = _("Regular monitoring");
    wxString colorD = _("Avoid combination");
    wxString colorX = _("Contraindicated");
    wxString note2 = _("Data source: Public domain data from EPha.ch.");
    wxString note3 = _("Supported by: IBSA Institut Biochimique SA.");

    wxString legend = wxString::Format(
                "<fieldset><legend>%s</legend></fieldset>"
                "<p class=\"footnote\">1. %s: </p>"
                "<table id=\"Farblegende\" style=\"background-color:transparent;\" cellpadding=\"3px\" width=\"100%%25\">"
                "  <tr bgcolor=\"#caff70\"><td align=\"center\">A</td><td>%s</td></tr>"
                "  <tr bgcolor=\"#ffec8b\"><td align=\"center\">B</td><td>%s</td></tr>"
                "  <tr bgcolor=\"#ffb90f\"><td align=\"center\">C</td><td>%s</td></tr>"
                "  <tr bgcolor=\"#ff82ab\"><td align=\"center\">D</td><td>%s</td></tr>"
                "  <tr bgcolor=\"#ff6a6a\"><td align=\"center\">X</td><td>%s</td></tr>"
                "</table>"
                "<p class=\"footnote\">2. %s</p>"
                "<p class=\"footnote\">3. %s</p>",
                                       footNote,
                                       note1,
                                       colorA, colorB, colorC, colorD, colorX,
                                       note2,
                                       note3);
    return legend;
}

// 280
void InteractionsHtmlView::sendInteractionNotice()
{
    std::clog << __PRETTY_FUNCTION__ << " line " << __LINE__ << " TODO" << std::endl;
}
