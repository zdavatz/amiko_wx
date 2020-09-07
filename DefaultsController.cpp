//  DefaultsController.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 7 Sep 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include "DefaultsController.hpp"

#include <stdio.h>
#include <iostream>
#include <wx/wx.h>

#include "DefaultsController.hpp"

DefaultsController* DefaultsController::m_pInstance;

// Singleton
DefaultsController* DefaultsController::Instance()
{
    if (!m_pInstance) {
        m_pInstance = new DefaultsController;
    }

    return m_pInstance;
}

void DefaultsController::setBool(bool value, const wxString & key)
{
    Write(key, value);
}

bool DefaultsController::getBool(const wxString & key, bool defVal)
{
    return ReadBool(key, defVal);
}

void DefaultsController::setInt(int value, const wxString & key)
{
    Write(key, value);
}

int DefaultsController::getInt(const wxString & key, int defVal)
{
    int value;
    Read(key, &value, defVal);
    return value;
}

void DefaultsController::setString(const wxString& value, const wxString& key)
{
    Write(key, value);
}

wxString DefaultsController::getString(const wxString& key, const char* defVal)
{
    wxString value;
    Read(key, &value, defVal);
    return value;
}
