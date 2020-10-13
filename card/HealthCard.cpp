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
    std::clog << __PRETTY_FUNCTION__ << " TODO:" << std::endl;
    if (data.size() < 2)
        return 0;
    
    uint8_t tag = data[0];
    uint8_t length = data[1];
    
    // TODO: switch tag
    return length + 2;
}

// 101
void HealthCard::parseCardData(const std::vector<BYTE> & data)
{
    std::clog << __PRETTY_FUNCTION__ << " TODO:" << std::endl;

    if (data.size() < 2)
        return;
    
    uint8_t offset = 0;
    uint8_t packetType = data[0];
    uint8_t packetSize = data[1];

    if (packetType == 0x65) {
        
        // TODO: while loop
        parseTLV(data);
    }
}

// 157
void HealthCard::processValidCard(SCARDCONTEXT &hContext)
{
    std::clog << __PRETTY_FUNCTION__ << std::endl;
    
    std::vector<BYTE> ef_id = { 0x2F, 0x06 };
    scSelectFile(ef_id);

    std::vector<BYTE> cmdReadBinary = {
        0,
        INS_READ_BIN,
        0, 0, // P1 P2
        //0x10
        // no data
        84 // Le
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
        // no data
        95 // Le
    };
    sendIns(cmdReadBinary2, dataResponse);
    parseCardData(dataResponse);
}
