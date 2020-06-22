//  Medication.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 18 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <wx/wx.h>
#include <wx/arrstr.h>

#include "Medication.hpp"
#include "Utilities.hpp"

// 27
static const wxString SectionTitle_DE[] = {
    wxT("Zusammensetzung"),
    wxT("Galenische Form"),
    wxT("Kontraindikationen"),
    wxT("Indikationen"),
    wxT("Dosierung/Anwendung"),
    wxT("Vorsichtsmassnahmen"),
    wxT("Interaktionen"),
    wxT("Schwangerschaft"),
    wxT("Fahrtüchtigkeit"),
    wxT("Unerwünschte Wirk."),
    wxT("Überdosierung"),
    wxT("Eig./Wirkung"),
    wxT("Kinetik"),
    wxT("Präklinik"),
    wxT("Sonstige Hinweise"),
    wxT("Zulassungsnummer"),
    wxT("Packungen"),
    wxT("Inhaberin"),
    wxT("Stand der Information") };

static wxString SectionTitle_FR[] = {
    wxT("Composition"),
    wxT("Forme galénique"),
    wxT("Contre-indications"),
    wxT("Indications"),
    wxT("Posologie"),
    wxT("Précautions"),
    wxT("Interactions"),
    wxT("Grossesse/All."),
    wxT("Conduite"),
    wxT("Effets indésir."),
    wxT("Surdosage"),
    wxT("Propriétés/Effets"),
    wxT("Cinétique"),
    wxT("Préclinique"),
    wxT("Remarques"),
    wxT("Numéro d'autorisation"),
    wxT("Présentation"),
    wxT("Titulaire"),
    wxT("Mise à jour") };

Medication::Medication()
: medId(0)
, title(nullptr)
, auth(nullptr)
, atccode(nullptr)
, substances(nullptr)
, regnrs(nullptr)
, atcClass(nullptr)
, therapy(nullptr)
, application(nullptr)
, indications(nullptr)
, customerId(0)
, packInfo(nullptr)
, addInfo(nullptr)
, sectionIds(nullptr)
, sectionTitles(nullptr)
, styleStr(nullptr)
, contentStr(nullptr)
, packages(nullptr)
{
    
}

// 56
wxArrayString Medication::listOfSectionIds()
{
    return wxSplit(wxString(sectionIds), ','); // wxSplit can only take a single char
}

// 61
wxArrayString Medication::listOfSectionTitles()
{
    //std::clog << "Line " << __LINE__  << " sectionTitles: " << sectionTitles << std::endl;

    wxArrayString titles = wxSplit(wxString(sectionTitles), ';');
    NSUInteger n = titles.size();
    for (int i=0; i<n; ++i)
        titles[i] = shortTitle(titles[i]);

    return titles;
}

// 93
const wxString &Medication::shortTitle(wxString &longTitle)
{
    wxString t = longTitle.Lower();

    //std::clog << "longTitle: " << longTitle.ToStdString() << ", t: " << t.ToStdString() << std::endl;

    const wxString * SectionTitle = SectionTitle_FR;
    int n = WXSIZEOF(SectionTitle_FR);

    if (strncmp(UTI::appLanguage(), "de",2) == 0) {
        SectionTitle = SectionTitle_DE;
        n = WXSIZEOF(SectionTitle_DE);
    }
 
    for (int i=0; i<n; i++)
    {
        wxString compareString = SectionTitle[i].Lower();
//        if (compareString.IsEmpty()) // WHy would it be empty ?
//            continue;

        //std::clog << "i: " << i << " compareString:" << compareString.ToStdString() << std::endl;

        if (t.Contains(compareString))
            return SectionTitle[i];
    }

    return longTitle;
}