//  Utilities.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 17 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <wx/wx.h>

namespace UTI
{

const char * appLanguage();
wxString currentTime();
wxString getColorCss();
wxString sha256(const wxString str);

}
