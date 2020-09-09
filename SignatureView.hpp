//  SignatureView.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 22 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <wx/statbmp.h>
#include <wx/image.h>

class SignatureView : public wxStaticBitmap
{
public:
    SignatureView(wxWindow *parent,
                  wxWindowID id,
                  const wxBitmap& bitmap,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  const wxString& name = wxASCII_STR(wxStaticBitmapNameStr));

    virtual ~SignatureView() {}
    wxImage getSignaturePNG();
};
