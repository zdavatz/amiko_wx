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
#if 1
    std::clog << __PRETTY_FUNCTION__ << " Line:" << __LINE__ << " TODO" << std::endl;
    return GetBitmap().ConvertToImage();
#else
    NSData *data = [self dataWithPDFInsideRect:[self bounds]];
    NSImage *img = [[NSImage alloc] initWithData:data];
    NSData *tiffPresentation = [img TIFFRepresentation];
    NSBitmapImageRep *rep = [NSBitmapImageRep imageRepWithData:tiffPresentation];

    NSDictionary* props = [NSDictionary dictionaryWithObject:[NSNumber numberWithFloat:1.0] forKey:NSImageCompressionFactor];
    NSData *pngData = [rep representationUsingType:NSPNGFileType properties:props];
    
    return pngData;
#endif
}
