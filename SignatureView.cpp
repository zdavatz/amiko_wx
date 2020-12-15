//  SignatureView.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 22 Jul 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <stdio.h>
#include <iostream>

#include <wx/dcmemory.h>

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

// 144
wxImage SignatureView::getSignaturePNG()
{
    std::clog << __PRETTY_FUNCTION__ << " Line:" << __LINE__ << " TODO" << std::endl;

    if (GetBitmap().IsOk())
        return GetBitmap().ConvertToImage();

    return wxNullImage;
}
