//
//  main.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 12 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <stdio.h>
#include <iostream>

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "main.hpp"
#include "MainWindow.h"

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;
 
    MainWindow* frame = new MainWindow(nullptr);

    frame->Show();
    SetTopWindow( frame );
    return true;
}

int MyApp::OnExit()
{
#if 0
    delete wxConfig::Set(NULL);
#endif
    return EXIT_SUCCESS;
}



