//  HealthCard.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 9 Oct 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <wx/wx.h>

#include "SmartCard.hpp"

class HealthCard : public SmartCard
{
public:
    HealthCard();
    virtual ~HealthCard() {}
    
    virtual void processValidCard(SCARDCONTEXT &hContext);
    virtual void parseCardData(const std::vector<BYTE> & data);

    uint8_t parseTLV(const std::vector<BYTE> & data);
    
    wxString familyName;
    wxString givenName;
    wxString birthDate;
    wxString gender;
};

