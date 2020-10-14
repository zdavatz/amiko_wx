//  SmartCard.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 9 Oct 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <iostream>
#include <iomanip>
#include <string>
#include <cstring> // for strlen

#include "SmartCard.hpp"

#define CHECK(f, rv) \
 if (SCARD_S_SUCCESS != rv) \
 { \
  printf(f ": %s\n", pcsc_stringify_error(rv)); \
  return; \
 }

// The ony difference is the return statement
#define CHECK_RV(f, rv) \
 if (SCARD_S_SUCCESS != rv) \
 { \
  printf(f ": %s\n", pcsc_stringify_error(rv)); \
  return rv; \
 }

#define TIMEOUT 1000    /* 1 second timeout */

SmartCard::SmartCard()
: readers(nullptr)
, pnp(true)
, rgReaderStates_t(nullptr)
, mszReaders(nullptr)
{
    std::clog << __PRETTY_FUNCTION__ << std::endl;
    start();

#if 0 //ndef NDEBUG
    //start();
    
    // APDU commands

    //scSelectMF();
    
    std::vector<BYTE> dataResponse;

    {
        std::vector<BYTE> cmd = {
            0x00,
            INS_SELECT_FILE,
            0x04, 0x00, // P1 P2
            2,          // Lc
            0x2f, 0x06
        };
        sendIns(cmd, dataResponse);
        // Response 6A 82
    }
    
    { // See scSelectMF
        std::vector<BYTE> cmd = {
            0x00,
            INS_SELECT_FILE,
            0, 0,       // P1 P2
            2,          // Lc
            0x3F, 0     // Select Master File
        };
        sendIns(cmd, dataResponse);
        // Response 90 00
    }

    {
        std::vector<BYTE> cmd = {
            0x00,
            INS_SELECT_FILE,
            0x04, 0x00, // P1 P2
            10,         // Lc
            0xA0, 0x00, 0x00, 0x00, 0x62,
            0x03, 0x01, 0x0C, 0x06, 0x01
        };
        sendIns(cmd, dataResponse);
        // Response 6A 82
    }

    {
        std::vector<BYTE> cmd = {
            0x00,
            INS_SELECT_FILE,
            0x08, 0,    // P1 P2
            2,
            0x2f, 0x06  // Selection by path from Master File
        };
        sendIns(cmd, dataResponse);
        // Response 90 00
    }

    {
        std::vector<BYTE> cmd = {
            0x00, INS_INVALID, 0x00, 0x00
        };
        sendIns(cmd, dataResponse);
        // Response 6D 00
    }

    //stop();
#endif // NDEBUG
}

SmartCard::~SmartCard()
{
    stop();
}

void SmartCard::detectChanges()
{
#ifndef NDEBUG
    //std::clog << __FUNCTION__ << std::endl;
#endif

#ifdef TEST_POLLING_SC
    
#if 0
    LONG rv = getReaders();

    std::clog << __FUNCTION__ << " line: " << __LINE__
    << ", nbReaders: " << nbReaders
    << ", rv: " << rv
    << std::endl;
#endif

    /// 640
    /* Wait endlessly for all events in the list of readers
     * We only stop in case of an error
     */

    /// 643
    // The function will block until the timeout expires or a change is reported.
    LONG rv = SCardGetStatusChange(hContext, TIMEOUT, rgReaderStates_t, nbReaders);
    /// 644
    while ((rv == SCARD_S_SUCCESS) || (rv == SCARD_E_TIMEOUT))
    {
        /// 648
        if (pnp)
        {
            /// 660
            if (rgReaderStates_t[nbReaders-1].dwEventState & SCARD_STATE_CHANGED)
            {
                //goto get_readers; // 476
                getReaders();
            }
        }
        else {
            // 670
            /* A new reader appeared? */
            if ((SCardListReaders(hContext, NULL, NULL, &dwReaders)
                == SCARD_S_SUCCESS) && (dwReaders != dwReadersOld))
            {
                // goto get_readers;
                getReaders();
            }
        }
        
        // 687
        /* Now we have an event, check all the readers in the list to see what
         * happened */
        for (int current_reader=0; current_reader < nbReaders; current_reader++)
        {
//            std::clog << __FUNCTION__ << " line: " << __LINE__
//            << ", reader: " << current_reader
//            << std::hex
//            << ", c state: 0x" << rgReaderStates_t[current_reader].dwCurrentState
//            << ", e state: 0x" << rgReaderStates_t[current_reader].dwEventState
//            << std::dec << std::endl;

#if defined(__APPLE__) || defined(WIN32)
            if (rgReaderStates_t[current_reader].dwCurrentState ==
                rgReaderStates_t[current_reader].dwEventState)
            {
                continue;
            }
#endif
            
            // 697
            if (rgReaderStates_t[current_reader].dwEventState &
                SCARD_STATE_CHANGED)
            {
#if 1 // @@@
                if (rgReaderStates_t[current_reader].dwEventState &
                    SCARD_STATE_EMPTY)
                    printf("Card index %i removed\n", current_reader);

                if (rgReaderStates_t[current_reader].dwEventState &
                    SCARD_STATE_PRESENT)
                {
                    printf("Card index %i inserted\n", current_reader);
                    
                    // Get valid handle
                    connectCard();
                    
                    // Not available in the constructor
                    processValidCard(hContext);     // handled by subclass

                    disconnectCard();
                }
#endif

                /* If something has changed the new state is now the current
                 * state */
                rgReaderStates_t[current_reader].dwCurrentState =
                    rgReaderStates_t[current_reader].dwEventState;
            }
            else
            {
                /* If nothing changed then skip to the next reader */
                continue;
            }
            
            // 744
//            if (rgReaderStates_t[current_reader].dwEventState &
//                SCARD_STATE_EMPTY)
//                printf("SCARD_STATE_EMPTY\n");
//
//            if (rgReaderStates_t[current_reader].dwEventState &
//                SCARD_STATE_PRESENT) {
//                printf("SCARD_STATE_PRESENT\n");
//            }
        }
        
        break; // @@@ TODO: only one loop for now
    } // while
    
 
#else
    // Not available in the constructor
    processValidCard(hContext);     // handled by subclass
#endif
}

void SmartCard::start()
{
    std::clog << __PRETTY_FUNCTION__ << std::endl;
    
    // 453
    LONG rv = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
    CHECK("SCardEstablishContext", rv)
    
#if 0
//    {
//#ifdef SCARD_AUTOALLOCATE
//    dwReaders = SCARD_AUTOALLOCATE;
//
//    rv = SCardListReaders(hContext, NULL, (LPTSTR)&mszReaders, &dwReaders);
//    CHECK("SCardListReaders", rv)
//#else
//    rv = SCardListReaders(hContext, NULL, NULL, &dwReaders);
//    CHECK("SCardListReaders", rv)
//
//    mszReaders = (LPTSTR)calloc(dwReaders, sizeof(char));
//    rv = SCardListReaders(hContext, NULL, mszReaders, &dwReaders);
//    CHECK("SCardListReaders", rv)
//#endif
//
//    std::clog << "Reader name: " << mszReaders << std::endl;
//    }
#endif
    
#if 1
    /// 456
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
    
#ifdef TEST_POLLING_SC
    getReaders();
    
    return;
    

#endif // TEST_POLLING_SC

#if 1
    connectCard();
#else
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
#endif
}

void SmartCard::stop()
{
    std::clog << __PRETTY_FUNCTION__ << std::endl;
    
    LONG rv;
    
#ifndef TEST_POLLING_SC
    rv = SCardDisconnect(hCard, SCARD_LEAVE_CARD);
    CHECK("SCardDisconnect", rv)
#endif

#ifdef SCARD_AUTOALLOCATE
     rv = SCardFreeMemory(hContext, mszReaders);
     CHECK("SCardFreeMemory", rv)
#else
     free(mszReaders);
#endif

    rv = SCardReleaseContext(hContext);
    CHECK("SCardReleaseContext", rv)
}

// ISO 7816 commands

// 62
void SmartCard::scSelectMF()
{
    std::clog << __FUNCTION__ << std::endl;
    std::vector<BYTE> dataResponse;

    std::vector<BYTE> cmd = {
        0x00,
        INS_SELECT_FILE,
        0,          // P1: Select MF, DF or EF
        0,          // P2
        2,          // Lc
        0x3F, 0     // Select Master File
    };
    sendIns(cmd, dataResponse);
}

// 86
void SmartCard::scSelectFile(const std::vector<BYTE> & filePath)
{
    //std::clog << "Start of " << __FUNCTION__ << std::endl;

    std::clog << __FUNCTION__ << " filePath:" << std::hex;
    for (int i=0; i < filePath.size(); i++) {
        std::clog << " " << std::setw(2) << std::setfill('0')
        << (int)filePath[i];
    }
    std::clog << std::dec << std::endl;

    std::vector<BYTE> cmd = {
        0x00,
        INS_SELECT_FILE,
        8,      // P1: Selection by path from MF
        0       // P2
    };
    
    cmd.push_back(filePath.size());
    cmd.insert(cmd.end(), filePath.begin(), filePath.end());
    // no Le
    
//    std::clog << "cmd:\n" << std::hex;
//    for (int i=0; i < cmd.size(); i++)
//    std::clog << " " << std::setw(2) << std::setfill('0') << (int)cmd[i];
//    std::clog << std::dec << std::endl;
    
    std::vector<BYTE> dataResponse;
    sendIns(cmd, dataResponse);
    //std::clog << "End of " << __FUNCTION__ << std::endl;
}

// CLA INS P1 P2 - Lc Data Le
void SmartCard::sendIns(const std::vector<BYTE> &cmd2, std::vector<BYTE> &response)
{
    const unsigned char *cmd = cmd2.data();
    DWORD cmdLength = cmd2.size();
    
    response.clear();

//    std::clog << "Start of " << __FUNCTION__
//    << " " << sizeof(cmd)
//    << " " << cmdLength << "\n";

    BYTE pbRecvBuffer[256 + 2];
    DWORD dwRecvLength = sizeof(pbRecvBuffer);
    LONG rv = SCardTransmit(hCard, &pioSendPci, cmd, cmdLength, NULL, pbRecvBuffer, &dwRecvLength);
    CHECK("SCardTransmit", rv)

    if (cmdLength >= 4)
        std::clog << __FUNCTION__
            << std::hex << std::setw(2) << std::setfill('0')
            << "\n\tcmd:"
            << " CLA=" << (int)cmd2[0]
            << " INS=" << (int)cmd2[1]
            << " P1="  << (int)cmd2[2]
            << " P2="  << (int)cmd2[3];
    if (cmdLength > 4)
        std::clog  << " -";
    for (int i=4; i < cmdLength; i++) {
        std::clog << " " << std::setw(2) << std::setfill('0')
        << (int)cmd[i];
    }

    std::clog << "\n\tresponse (size 0x" << dwRecvLength << "):";
    for (int i=0; i < dwRecvLength; i++) {
        if (i%16 == 0)
            std::clog << "\n\t\t";

        std::clog << std::setw(2) << std::setfill('0')
        << (int)pbRecvBuffer[i] << " ";
        
        response.push_back(pbRecvBuffer[i]);
    }

    std::clog << std::dec << std::endl;
    //std::clog << "End of " << __FUNCTION__ << std::endl;
}

/// 476
LONG SmartCard::getReaders()
{
    //std::clog << __FUNCTION__ << " line: " << __LINE__ << std::endl;
    
    /* free memory possibly allocated in a previous loop */
    if (nullptr != readers)
    {
        free(readers);
        readers = nullptr;
    }

    if (nullptr != rgReaderStates_t)
    {
        free(rgReaderStates_t);
        rgReaderStates_t = nullptr;
    }
    
    /// 498
    /* Retrieve the available readers list.
     *
     * 1. Call with a null buffer to get the number of bytes to allocate
     * 2. malloc the necessary storage
     * 3. call with the real allocated buffer
     */
    /// 498
    printf("Scanning present readers...\n");
    LONG rv = SCardListReaders(hContext, NULL, NULL, &dwReaders);
    if (rv != SCARD_E_NO_READERS_AVAILABLE)
        CHECK_RV("SCardListReaders", rv)
        
    /// 504
    dwReadersOld = dwReaders;
    
    /// 506
    /* if non NULL we came back so free first */
    if (mszReaders)
    {
        free(mszReaders);
        mszReaders = nullptr;
    }
    
    /// 513
    mszReaders = (LPTSTR)malloc(sizeof(char)*dwReaders);

    /// 520
    *mszReaders = '\0';
    rv = SCardListReaders(hContext, NULL, mszReaders, &dwReaders);

    /// 523
    /* Extract readers from the null separated string and get the total
     * number of readers */
    nbReaders = 0;
    char *ptr = mszReaders;
    while (*ptr != '\0')
    {
        ptr += std::strlen(ptr)+1;
        nbReaders++;
    }
    
//    std::clog << __FUNCTION__ << " line: " << __LINE__
//    << " nbReaders: " << nbReaders << std::endl;

    /// 533
    if (SCARD_E_NO_READERS_AVAILABLE == rv || 0 == nbReaders)
    {
        /// 537
        printf("Waiting for the first reader...\n");

        // 541
        if (pnp)
        {
            rgReaderStates[0].szReader = "\\\\?PnP?\\Notification";
            rgReaderStates[0].dwCurrentState = SCARD_STATE_UNAWARE;
            
            // TODO: 547 do {} while();
        }
        else
        {
            /// 563
            rv = SCARD_S_SUCCESS; // 0
            
            // TODO: 565 while
            rv = SCardListReaders(hContext, NULL, NULL, &dwReaders);

            std::clog << __FUNCTION__ << " line: " << __LINE__
            << std::hex << " rv: 0x" << rv << std::dec << std::endl;

            if (SCARD_E_NO_READERS_AVAILABLE == rv) { // 0x8010002E
                printf("SCARD_E_NO_READERS_AVAILABLE\n");
                return rv;
            }
        }
    }
    else
        CHECK_RV("SCardListReaders", rv)

//    std::clog << __FUNCTION__ << " line: " << __LINE__
//        << " nbReaders: " << nbReaders << std::endl;

    /// 584
    /* allocate the readers table */
    if (readers == nullptr)
        readers = (const char **)calloc(nbReaders+1, sizeof(char *));
    
    /* fill the readers table */
    nbReaders = 0;
    ptr = mszReaders;
    while (*ptr != '\0')
    {
        readers[nbReaders] = ptr;
        ptr += std::strlen(ptr)+1;
        nbReaders++;
    }
    
    // print_readers
    for (int i = 0;i < nbReaders; i++)
        printf("\t%d: <%s>\n", i, readers[i]);
    
    /// 605
    //std::clog << __FUNCTION__ << " line: " << __LINE__ << std::endl;
    
#if 1
    /// 608
    /* allocate the ReaderStates table */
    if (rgReaderStates_t == nullptr)
        rgReaderStates_t = (SCARD_READERSTATE *)calloc(nbReaders+1, sizeof(* rgReaderStates_t));
    
    /// 619
    /* Set the initial states to something we do not know
     * The loop below will include this state to the dwCurrentState
     */
    for (int i=0; i<nbReaders; i++)
    {
        rgReaderStates_t[i].szReader = readers[i];
        rgReaderStates_t[i].dwCurrentState = SCARD_STATE_UNAWARE;
        rgReaderStates_t[i].cbAtr = sizeof rgReaderStates_t[i].rgbAtr;
    }

    /// 626
    /* If Plug and Play is supported by the PC/SC layer */
    if (pnp)
    {
        rgReaderStates_t[nbReaders].szReader = "\\\\?PnP?\\Notification";
        rgReaderStates_t[nbReaders].dwCurrentState = SCARD_STATE_UNAWARE;
        nbReaders++;
    }
#endif
    
    return SCARD_S_SUCCESS;
}

void SmartCard::connectCard()
{
    DWORD dwActiveProtocol;

    LONG rv = SCardConnect(hContext, mszReaders, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &hCard, &dwActiveProtocol);
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

void SmartCard::disconnectCard()
{
    LONG rv = SCardDisconnect(hCard, SCARD_LEAVE_CARD);
    CHECK("SCardDisconnect", rv)
}
