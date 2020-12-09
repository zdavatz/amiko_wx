//  Utilities.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 17 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <time.h>
#include <utime.h>
#include <sys/stat.h>

#include <wx/wx.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/stdpaths.h>
#include <wx/settings.h>
#include <wx/filename.h>
#include <wx/datetime.h>

#include <openssl/sha.h>

#include "Utilities.hpp"

namespace UTI
{

// 64
const char * appLanguage()
{
    if (wxString(APP_NAME) == "CoMed")
        return "fr";

    return "de";
}

// 104
wxString documentsDirectory()
{    
    return wxStandardPaths::Get().GetUserDataDir();
}

// 111
bool checkFileIsAllowed(const wxString name)
{
    const char * languageCode = appLanguage();
    if (wxString(languageCode) != "de" &&
        wxString(languageCode) != "fr")
    {
        return false; // language not supported
    }

    wxString file1 = wxString::Format("amiko_db_full_idx_%s.db", languageCode);
    wxString file2 = wxString::Format("amiko_report_%s.html", languageCode);
    wxString file3 = wxString::Format("drug_interactions_csv_%s.csv", languageCode);
    wxString file4 = wxString::Format("amiko_frequency_%s.db", languageCode);

    if (name == file1 ||
        name == file2 ||
        name == file3 ||
        name == file4)
    {
        return true;
    }
    
    return false;
}

// 158
wxString currentTime()
{
    wxDateTime now = wxDateTime::Now();
    wxString s = now.Format("%Y-%m-%dT%H:%M.%S"); // strftime() syntax
    return s;
}

// 166
wxString prettyTime()
{
    wxDateTime now = wxDateTime::Now();
    wxString s = now.Format("%d.%m.%Y (%H:%M:%S)"); // strftime() syntax
    return s;
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

// std::cerr << UTI::sha256("davatz.zeno.2.6.1942") << std::endl;
// 2666ae74d73f10df8674334476a66b928812c70bb47900cb7e71cb3dcb4db8fa
// https://stackoverflow.com/questions/2262386/generate-sha256-with-openssl-and-c
wxString sha256(const wxString str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

std::string timeToString(std::chrono::time_point<std::chrono::system_clock> timePoint)
{
    std::time_t timePointT = std::chrono::system_clock::to_time_t(timePoint);
    std::ostringstream ss;
    ss << std::put_time(gmtime(&timePointT), "%FT%TZ");
    return ss.str();
}

std::chrono::time_point<std::chrono::system_clock> stringToTime(std::string inputStr)
{
    std::tm tm = {};
    std::stringstream ss(inputStr);
    ss >> std::get_time(&tm, "%FT%TZ");

    const time_t timeInUTC =
    #if defined(_WIN32)
        _mkgmtime(&tm);
    #else // Assume POSIX
        timegm(&tm);
    #endif
    return std::chrono::system_clock::from_time_t(timeInUTC);
}

void ensureDirectory(wxFileName filename) {
    if (wxDirExists(filename.GetFullPath())) {
        return;
    }
    wxFileName parent = wxFileName(filename.GetPath());
    if (!wxDirExists(parent.GetPath())) {
        ensureDirectory(parent);
    } else {
    }
    wxMkdir(filename.GetFullPath());
}

void setFileModifiedTime(std::string filepath, std::chrono::time_point<std::chrono::system_clock> timePoint) {
    struct stat foo;
    struct utimbuf new_times;

    stat(filepath.c_str(), &foo);
    new_times.actime = foo.st_atime; /* keep atime unchanged */
    new_times.modtime = std::chrono::system_clock::to_time_t(timePoint);    /* set mtime to current time */
    utime(filepath.c_str(), &new_times);
}

}
