//  Utilities.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 17 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <wx/wx.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/stdpaths.h>
#include <wx/settings.h>
#include <wx/filename.h>

#include "Utilities.hpp"

namespace UTI
{

// 64
const char * appLanguage()
{
    if (APP_NAME == "CoMed")
        return "fr";

    return "de";
}

// 186
wxString getColorCss()
{
    wxString colorSchemeFilename = "color-scheme-light.css";
    const wxSystemAppearance appearance = wxSystemSettings::GetAppearance();

    if (appearance.IsDark())
        colorSchemeFilename = "color-scheme-dark.css";

#ifdef __linux__
    wxFileName f(wxStandardPaths::Get().GetExecutablePath());
    wxString colorCssPath(f.GetPath());
#else
    // TODO: use GetResourcesDir()
    wxString colorCssPath = wxStandardPaths::Get().GetUserDataDir();
#endif
    colorCssPath += wxFILE_SEP_PATH + colorSchemeFilename;

    // Read CSS file into string
    wxString colorCss;
    if (wxFileName::Exists(colorCssPath))
    {
        wxFileInputStream input( colorCssPath );
        wxTextInputStream text(input, wxT("\x09"), wxConvUTF8 );
        while (input.IsOk() && !input.Eof() )
            colorCss += text.ReadLine() + wxT("\n");
    }

    return colorCss;
}

}
