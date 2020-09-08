//  SignatureView.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 22 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include "SignatureView.hpp"

SignatureView::SignatureView(wxWindow *parent,
                      wxWindowID id,
                      const wxBitmap& bitmap,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxString& name)
: wxStaticBitmap(parent, id, bitmap, pos, size, style, name)
{
    
}
