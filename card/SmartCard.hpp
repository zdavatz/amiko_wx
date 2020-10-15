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

// https://github.com/boundary/wireshark/blob/master/epan/dissectors/packet-iso7816.c
#define INS_SELECT_FILE    0xA4
#define INS_READ_BIN       0xB0
#define INS_GET_DATA       0xCA
/* for our transaction tracking, not defined in the specification */
#define INS_INVALID        0x00

class SmartCard
{
public:
    SmartCard();
    virtual ~SmartCard();

    // 38
    virtual void parseCardData(const std::vector<BYTE> & data) = 0;
    virtual void processValidCard(SCARDCONTEXT &hContext) = 0;
    virtual bool validAtr(const std::vector<uint8_t> & data) = 0;

    bool detectChanges();
    bool connectCard();
    void disconnectCard();
    void start();
    void stop();
    void sendIns(const std::vector<BYTE> &cmd, std::vector<BYTE> &response);
    void scSelectMF();
    void scSelectFile(const std::vector<BYTE> & ef_id);
    LONG getReaders();
    void dumpState(uint32_t dwEventState);

    SCARDHANDLE hCard;

private:
    SCARDCONTEXT hContext;
    SCARD_READERSTATE *rgReaderStates_t;
    SCARD_READERSTATE rgReaderStates[1];
    DWORD dwReaders = 0, dwReadersOld;
    LPTSTR mszReaders;
    const char **readers;
    int nbReaders;
    int pnp;

    SCARD_IO_REQUEST pioSendPci;
};
