//  Medication.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 18 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <map>

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
//, title(nullptr)
//, auth(nullptr)
//, atccode(nullptr)
, substances(nullptr)
//, regnrs(nullptr)
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
    return wxSplit(wxString(sectionIds, wxConvUTF8), ','); // wxSplit can only take a single char
}

// 61
wxArrayString Medication::listOfSectionTitles()
{
    wxArrayString titles = wxSplit(wxString(sectionTitles, wxConvUTF8), ';');
    int n = titles.size();
    //std::clog << __FUNCTION__ << " Line " << __LINE__ << " # sectionTitles: " << n << std::endl;
    for (int i=0; i<n; ++i)
        titles[i] = shortTitle(titles[i]);

    return titles;
}

// 71
std::map<wxString, wxString>
Medication::indexToTitlesDict()
{
    std::map<wxString, wxString> dict;
    
    wxArrayString ids = listOfSectionIds();
    wxArrayString titles = listOfSectionTitles();
    
    int n1 = ids .size();
    int n2 = titles.size();
    int n = n1 < n2 ? n1 : n2;
    for (int i=0; i<n; ++i) {
        wxString id = ids[i];
        id.Replace("section", "");
        id.Replace("Section", "");
        if (id.length() > 0) {
            dict[id] = shortTitle(titles[i]);
        }
    }
    
    return dict;
}

// 93
const wxString &Medication::shortTitle(wxString &longTitle)
{
    wxString t = longTitle.Lower();

    //std::clog << "longTitle: " << longTitle << ", t: " << t << std::endl;

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

        //std::clog << "i: " << i << " compareString:" << compareString << std::endl;

        if (t.Contains(compareString))
            return SectionTitle[i];
    }

    return longTitle;
}

// 120
void Medication::importFromDict(std::map<std::string, std::string> &dict)
{
    title   = dict[KEY_AMK_MED_TITLE];
    auth    = dict[KEY_AMK_MED_OWNER];
    regnrs  = dict[KEY_AMK_MED_REG_N];
    atccode = dict[KEY_AMK_MED_ATC];

#if 0 //ndef NDEBUG
    std::clog << __PRETTY_FUNCTION__
    << ", title: " << title
    << ", auth: " << auth
    << ", reg: " << regnrs
    << ", ATC: " << atccode
    << std::endl;
#endif
}
