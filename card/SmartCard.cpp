//  SmartCard.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 9 Oct 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <iostream>
#include <iomanip>
#include <string>

#include "SmartCard.hpp"

#define CHECK(f, rv) \
 if (SCARD_S_SUCCESS != rv) \
 { \
  printf(f ": %s\n", pcsc_stringify_error(rv)); \
  return; \
 }

#define TIMEOUT 1000    /* 1 second timeout */

SmartCard::SmartCard()
{
    std::clog << __PRETTY_FUNCTION__ << std::endl;

#ifndef NDEBUG
    start();
    
    // APDU commands

    {
        std::vector<BYTE> cmd = {
            0x00,
            INS_SELECT_FILE,
            0x04, 0x00, // P1 P2
            0x02,
            0x2f, 0x06
        };
        sendIns(cmd);
        // Response 6A 82
    }
    
    {
        std::vector<BYTE> cmd = {
            0x00,
            INS_SELECT_FILE,
            0, 0,       // P1 P2
            2, 0x3F, 0  // Select Master File
        };
        sendIns(cmd);
        // Response 90 00
    }

    {
        std::vector<BYTE> cmd = {
            0x00,
            INS_SELECT_FILE,
            0x04, 0x00, // P1 P2
            0x0A,       // data length (TBC)
            0xA0, 0x00, 0x00, 0x00, 0x62, 0x03, 0x01, 0x0C, 0x06, 0x01
        };
        sendIns(cmd);
        // Response 6A 82
    }

    {
        std::vector<BYTE> cmd = {
            0x00,
            INS_SELECT_FILE,
            0x08, 0,    // P1 P2
            0x2f, 0x06  // Selection by path from Master File
        };
        sendIns(cmd);
        // Response 67 00
    }

    {
        std::vector<BYTE> cmd = {
            0x00, INS_INVALID, 0x00, 0x00
        };
        sendIns(cmd);
        // Response 6D 00
    }

    stop();
#endif // NDEBUG
}

void SmartCard::detectChanges()
{
    static unsigned int count = 0;
    
    if (count++ > 5)
        return;  // temporary

#ifndef NDEBUG
    std::clog << __PRETTY_FUNCTION__ << " " << count
    << std::endl;
#endif

    // Not available in the constructor
    processValidCard(hContext);     // handled by subclass
}

void SmartCard::start()
{
    std::clog << __PRETTY_FUNCTION__ << std::endl;
    
    LONG rv = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
    CHECK("SCardEstablishContext", rv)
    
    {
#ifdef SCARD_AUTOALLOCATE
    dwReaders = SCARD_AUTOALLOCATE;

    rv = SCardListReaders(hContext, NULL, (LPTSTR)&mszReaders, &dwReaders);
    CHECK("SCardListReaders", rv)
#else
    rv = SCardListReaders(hContext, NULL, NULL, &dwReaders);
    CHECK("SCardListReaders", rv)

    mszReaders = (LPTSTR)calloc(dwReaders, sizeof(char));
    rv = SCardListReaders(hContext, NULL, mszReaders, &dwReaders);
    CHECK("SCardListReaders", rv)
#endif
    
    std::clog << "Reader name: " << mszReaders << std::endl;
    }
    
#if 1
    int pnp = true;
    rgReaderStates[0].szReader = "\\\\?PnP?\\Notification";
    rgReaderStates[0].dwCurrentState = SCARD_STATE_UNAWARE;

    rv = SCardGetStatusChange(hContext, 0, rgReaderStates, 1);
    if (rgReaderStates[0].dwEventState & SCARD_STATE_UNKNOWN)
    {
        printf("Plug'n play reader name not supported. Using polling every %d ms.\n", TIMEOUT);

        pnp = false;
    }
    else
    {
        printf("Using reader plug'n play mechanism\n");
    }
#endif

    DWORD dwActiveProtocol;

    rv = SCardConnect(hContext, mszReaders, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &hCard, &dwActiveProtocol);
    CHECK("SCardConnect", rv)

    switch (dwActiveProtocol)
    {
        case SCARD_PROTOCOL_T0:
            std::clog << "\tprotocol T0\n";
            pioSendPci = *SCARD_PCI_T0;
            break;

        case SCARD_PROTOCOL_T1:
            std::clog << "\tprotocol T1\n";
            pioSendPci = *SCARD_PCI_T1;
            break;
            
        case SCARD_PROTOCOL_RAW:
            std::clog << "\tprotocol raw\n";
            pioSendPci = *SCARD_PCI_RAW;
            break;

        default:
            fprintf(stderr, "Unknown protocol\n");
            break;
    }
}

void SmartCard::stop()
{
    std::clog << __PRETTY_FUNCTION__ << std::endl;
    
    LONG rv = SCardDisconnect(hCard, SCARD_LEAVE_CARD);
    CHECK("SCardDisconnect", rv)

#ifdef SCARD_AUTOALLOCATE
     rv = SCardFreeMemory(hContext, mszReaders);
     CHECK("SCardFreeMemory", rv)
#else
     free(mszReaders);
#endif

    rv = SCardReleaseContext(hContext);
    CHECK("SCardReleaseContext", rv)
}

void SmartCard::scSelectFile(const std::vector<BYTE> & ef_id)
{
    std::clog << __FUNCTION__ << std::hex << std::endl;

    std::clog << "ed_if:\n";
    for (int i=0; i < ef_id.size(); i++)
    std::clog << " " << std::setw(2) << std::setfill('0') << (int)ef_id[i];
    std::clog << std::dec << std::endl;

    std::vector<BYTE> cmd = {
        0x00,
        INS_SELECT_FILE,
        8,      // P1: Selection by path from MF
        0       // P2
    };
    
    cmd.insert(cmd.end(), ef_id.begin(), ef_id.end());
    
    std::clog << "cmd:\n" << std::hex;
    for (int i=0; i < cmd.size(); i++)
    std::clog << " " << std::setw(2) << std::setfill('0') << (int)cmd[i];

    std::clog << std::dec << std::endl;
    
    sendIns(cmd);
}

//void SmartCard::sendIns2(const std::vector<BYTE> &cmd)
//{
//    sendIns(cmd.data(), cmd.size());
//}

void SmartCard::sendIns(const std::vector<BYTE> &cmd2)
{
    const unsigned char *cmd = cmd2.data();
    DWORD cmdLength = cmd2.size();

//    std::clog << __FUNCTION__
//    << " " << sizeof(cmd)
//    << " " << cmdLength << "\n";

    BYTE pbRecvBuffer[256 + 2];
    DWORD dwRecvLength = sizeof(pbRecvBuffer);
    LONG rv = SCardTransmit(hCard, &pioSendPci, cmd, cmdLength, NULL, pbRecvBuffer, &dwRecvLength);
    CHECK("SCardTransmit", rv)

    std::clog << __FUNCTION__
    << std::hex
    << " INS: " << (int)cmd2[1]
    << ", response: ";
    for (int i=0; i<dwRecvLength; i++)
        std::clog << std::setw(2) << std::setfill('0') << (int)pbRecvBuffer[i] << " ";

    std::clog << std::dec << std::endl;
}
