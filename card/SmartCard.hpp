//  SmartCard.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 9 Oct 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

#include <vector>

#ifdef __APPLE__
#include <PCSC/winscard.h>
#include <PCSC/wintypes.h>
#else
#include <winscard.h>
#endif

#define INS_SELECT_FILE    0xA4
#define INS_READ_BIN       0xB0
/* for our transaction tracking, not defined in the specification */
#define INS_INVALID        0x00

class SmartCard
{
public:
    SmartCard();
    virtual ~SmartCard() {}

    virtual void processValidCard(SCARDCONTEXT &hContext) = 0;
    virtual void parseCardData(const std::vector<BYTE> & data) = 0;

    void detectChanges();
    void start();
    void stop();
    void sendIns(const std::vector<BYTE> &cmd);
    //void sendIns(const unsigned char *cmd, DWORD cmdLength);
    void scSelectFile(const std::vector<BYTE> & ef_id);

    SCARDHANDLE hCard;

private:
    SCARDCONTEXT hContext;
    SCARD_READERSTATE *rgReaderStates_t = nullptr;
    SCARD_READERSTATE rgReaderStates[1];
    DWORD dwReaders = 0, dwReadersOld;
    LPTSTR mszReaders;

    SCARD_IO_REQUEST pioSendPci;
};
