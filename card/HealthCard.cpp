//  HealthCard.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 9 Oct 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <iostream>

#include "HealthCard.hpp"

HealthCard::HealthCard()
{
    
}

// 14
uint8_t HealthCard::parseTLV(const std::vector<BYTE> & data)
{
    if (data.size() < 2)
        return 0;
    
    uint8_t tag = data[0];
    uint8_t length = data[1];
    
    auto first = data.cbegin() + 2;
    auto last = first + length;
    std::vector<BYTE> value(first, last);

#if 0
    std::clog << __FUNCTION__
    << std::hex << ", tag:0x" << (int)tag
    << std::dec << ", length:" << (int)length;
    std::clog << ", value: <";
    for (auto c : value)
        std::clog << c;
    std::clog << ">" << std::endl;
#endif
    
    switch (tag) {
        case 0x80:  // UTF8InternationalString
        {
            std::string s(value.begin(), value.end());
            wxString ws(s.c_str(), wxConvUTF8);
            wxArrayString a = wxSplit(ws, ',');
            familyName = a[0];
            if (a.size() > 1) {
                givenName = a[1];
                givenName.Trim();
            }
            std::clog
            << "Family name: " << familyName
            << "\nGiven name: " << givenName
            << std::endl;
        }
            break;
            
        case 0x82:  // NUMERIC STRING
        {
            std::string s(value.begin(), value.end());
            std::clog << "DOB: " << s << std::endl;
        }
            break;
            
        case 0x83:  // UTF8InternationalString
        {
            std::string s(value.begin(), value.end());
            std::clog << "Card holder ID: " << s << std::endl;
        }
            break;
            
        case 0x84:  // ENUMERATED
        {
            // 58
            BYTE sexEnum = value[0];
            if (sexEnum == 1)
                gender = wxT("man");
            else if (sexEnum == 2)
                gender = wxT("woman");
            else
                gender = wxEmptyString;

            // 1=male, 2=female, 0=not known, 9=not appl.
            std::clog << "Sex: "
            //<< (int)sexEnum << " "
            << gender << std::endl;
        }
            break;

        case 0x90:  // UTF8InternationalString
        {
            std::string s(value.begin(), value.end());
            std::clog << "Issuing State ID Number: " << s << std::endl;
        }
            break;
            
        case 0x91:  // UTF8InternationalString
        {
            std::string s(value.begin(), value.end());
            std::clog << "name Of The Institution: " << s << std::endl;
        }
            break;
            
        case 0x92:  // NUMERIC STRING
        {
            std::string s(value.begin(), value.end());
            std::clog << "identificationNumber Of The Institution: " << s << std::endl;
        }
            break;

        case 0x93:  // UTF8InternationalString
        {
            std::string s(value.begin(), value.end());
            std::clog << "Insured Person Number: " << s << std::endl;
        }
            break;
            
        case 0x94:  // NUMERIC STRING
        {
            std::string s(value.begin(), value.end());
            std::clog << "ExpiryDate yyyyMMdd: " << s << std::endl;
        }
            break;

        default:
            std::clog << __FUNCTION__
            << " tag:" << tag
            << " length:" << length
            //<< " value:" << value
            << std::endl;
            break;
    }

    return length + 2;
}

// 101
void HealthCard::parseCardData(const std::vector<BYTE> & data)
{
    if (data.size() < 2)
        return;
    
    uint8_t offset = 0;
    uint8_t packetType = data[0];
    uint8_t packetSize = data[1];
    
    auto first = data.cbegin() + 2;
    auto last = first + packetSize;
    std::vector<BYTE> payload(first, last);

    if (packetType != 0x65)
        return;

    while (offset < packetSize) {
//        std::clog << __FUNCTION__
//        << ", offset:" << (int)offset
//        << ", packetSize:" << (int)packetSize;

        auto first = payload.cbegin() + offset;
        auto last = first + packetSize - offset;
        std::vector<BYTE> dataRange(first, last);
        
//        std::clog << ", dataRange: <"; // remaining to be parsed
//        for (auto c : dataRange)
//            std::clog << c;
//        std::clog << ">" << std::endl;

        offset += parseTLV(dataRange);
    }
}

// 157
void HealthCard::processValidCard(SCARDCONTEXT &hContext)
{
    std::clog << __PRETTY_FUNCTION__ << std::endl;
    
    // Elementary file
    std::vector<BYTE> ef_id = { 0x2F, 0x06 };
    scSelectFile(ef_id);

    std::vector<BYTE> cmdReadBinary = {
        0,
        INS_READ_BIN,
        0, 0, // P1 P2
        84 // Lc
        // no data
    };
    std::vector<BYTE> dataResponse;
    sendIns(cmdReadBinary, dataResponse);
    parseCardData(dataResponse);
    
    //
    
    std::vector<BYTE> ef_ad = { 0x2F, 0x07 };
    scSelectFile(ef_ad);

    std::vector<BYTE> cmdReadBinary2 = {
        0,
        INS_READ_BIN,
        0, 0, // P1 P2
        95 // Lc
        // no data
    };
    sendIns(cmdReadBinary2, dataResponse);
    parseCardData(dataResponse);
}
