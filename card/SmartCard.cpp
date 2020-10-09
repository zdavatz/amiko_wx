//  SmartCard.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 9 Oct 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <iostream>
#include <iomanip>
#include <string>

#ifdef __APPLE__
#include <PCSC/winscard.h>
#include <PCSC/wintypes.h>
#else
#include <winscard.h>
#endif

#include "SmartCard.hpp"

#define CHECK(f, rv) \
 if (SCARD_S_SUCCESS != rv) \
 { \
  printf(f ": %s\n", pcsc_stringify_error(rv)); \
  return; \
 }

SmartCard::SmartCard()
{
#if 1
    // https://ludovicrousseau.blogspot.com/2010/04/pcsc-sample-in-c.html
    SCARDCONTEXT hContext;
    LPTSTR mszReaders;
    SCARDHANDLE hCard;
    DWORD dwReaders, dwActiveProtocol, dwRecvLength;
    
    SCARD_IO_REQUEST pioSendPci;
    BYTE pbRecvBuffer[258];
    BYTE cmd1[] = { 0x00, 0xA4, 0x04, 0x00, 0x0A, 0xA0, 0x00, 0x00, 0x00, 0x62, 0x03, 0x01, 0x0C, 0x06, 0x01 };
    BYTE cmd2[] = { 0x00, 0x00, 0x00, 0x00 };

    unsigned int i;

    LONG rv = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
    CHECK("SCardEstablishContext", rv)
    
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
    
    std::clog << __PRETTY_FUNCTION__ << " reader name: " << mszReaders << std::endl;

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
    }
    
    // Command 1

    dwRecvLength = sizeof(pbRecvBuffer);
    rv = SCardTransmit(hCard, &pioSendPci, cmd1, sizeof(cmd1), NULL, pbRecvBuffer, &dwRecvLength);
    CHECK("SCardTransmit", rv)

    std::clog << "first response: "; // 6A 82

    for (i=0; i<dwRecvLength; i++)
        std::clog << std::hex << std::setw(2) << std::setfill('0') << pbRecvBuffer[i];

    std::clog << std::endl;

    // Command 2

    dwRecvLength = sizeof(pbRecvBuffer);
    rv = SCardTransmit(hCard, &pioSendPci, cmd2, sizeof(cmd2), NULL, pbRecvBuffer, &dwRecvLength);
    CHECK("SCardTransmit", rv)

    std::clog << "second response: "; // 6D 00
    for (i=0; i<dwRecvLength; i++)
        std::clog << std::hex << std::setw(2) << std::setfill('0') << pbRecvBuffer[i];

    std::clog << std::endl;

    rv = SCardDisconnect(hCard, SCARD_LEAVE_CARD);
    CHECK("SCardDisconnect", rv)

#ifdef SCARD_AUTOALLOCATE
     rv = SCardFreeMemory(hContext, mszReaders);
     CHECK("SCardFreeMemory", rv)
#else
     free(mszReaders);
#endif

     rv = SCardReleaseContext(hContext);
     CHECK("SCardReleaseContext", rv)

#else
    self.mngr = [TKSmartCardSlotManager defaultManager];
    assert(self.mngr);
    
    // Observe readers joining and leaving.
    [self.mngr addObserver:self
                forKeyPath:@"slotNames"
                   options:NSKeyValueObservingOptionNew | NSKeyValueObservingOptionOld | NSKeyValueObservingOptionInitial
                   context:nil];
#endif
}
