//  Utilities.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 17 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <wx/wx.h>
#include <chrono>
#include <wx/filename.h>

namespace UTI
{

const char * appLanguage();
wxString documentsDirectory();
bool checkFileIsAllowed(const wxString name);
wxString currentTime();
wxString prettyTime();
wxString getColorCss();
wxString sha256(const wxString str);
std::string timeToString(std::chrono::time_point<std::chrono::system_clock> timePoint);
std::chrono::time_point<std::chrono::system_clock> stringToTime(std:: string inputStr);
std::chrono::time_point<std::chrono::system_clock> patientStringToTime(std::string inputStr);
void ensureDirectory(wxFileName filename);
void setFileModifiedTime(std::string filepath, std::chrono::time_point<std::chrono::system_clock> timePoint);
}
