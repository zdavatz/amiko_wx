//  SmartCard.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 9 Oct 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

// Reference: pcsc_scan source code

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

// The ony difference is in the return statement
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
    start();
}

SmartCard::~SmartCard()
{
    stop();
}

// return true if new card data has been acquired
bool SmartCard::detectChanges()
{
    //std::clog << __FUNCTION__ << std::endl;
    bool newCardDataAcquired = false;

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
                //goto get_readers; /// 476
                getReaders();
            }
        }
        else {
            /// 670
            /* A new reader appeared? */
            if ((SCardListReaders(hContext, NULL, NULL, &dwReaders) == SCARD_S_SUCCESS) &&
                (dwReaders != dwReadersOld))
            {
                // goto get_readers;
                getReaders();
            }
        }
        
        /// 687
        /* Now we have an event, check all the readers in the list to see what
         * happened */
        for (int current_reader=0; current_reader < nbReaders; current_reader++)
        {
#if defined(__APPLE__) || defined(WIN32)
            if (rgReaderStates_t[current_reader].dwCurrentState ==
                rgReaderStates_t[current_reader].dwEventState)
            {
                continue;
            }
#endif
            
            /// 697
            if (rgReaderStates_t[current_reader].dwEventState &
                SCARD_STATE_CHANGED)
            {
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
            
#ifndef NDEBUG
            // 726
            /* Dump the full current state */
            std::clog << "Card reader at index " << current_reader;
            dumpState( rgReaderStates_t[current_reader].dwEventState);
#endif

            /// 773
#if 0
            /* Also dump the ATR if available */
            if (rgReaderStates_t[current_reader].cbAtr > 0)
            {
                char atr[MAX_ATR_SIZE*3+1];    /* ATR in ASCII */
                std::vector<uint8_t> atrVector;
                printf("  ATR: ");

                if (rgReaderStates_t[current_reader].cbAtr)
                {
                    unsigned int j;
                    for (j=0; j<rgReaderStates_t[current_reader].cbAtr; j++) {
                        sprintf(&atr[j*3], "%02X ", rgReaderStates_t[current_reader].rgbAtr[j]);
                    }

                    atr[j*3-1] = '\0';
                }
                else
                    atr[0] = '\0';

                std::clog << atr << std::endl;

    #ifndef __APPLE__
                // Analyse ATR
                {
                printf("\n");

    // Command used to parse (on screen) the ATR:
    // ATR_analysis '3B 9F 13 81 B1 80 37 1F 03 80 31 F8 69 4D 54 43 4F 53 70 02 01 02 81 07 86'
                #define ATR_PARSER "ATR_analysis"
                char atr_command[sizeof(atr)+sizeof(ATR_PARSER)+2+1];
                sprintf(atr_command, ATR_PARSER " '%s'", atr);
                if (system(atr_command))
                    perror(atr_command);
                }
    #endif // __APPLE__
            } // if cbAtr > 0
#else
            if (rgReaderStates_t[current_reader].cbAtr > 0) {
                std::vector<uint8_t> atrVector;
                for (int j=0; j<rgReaderStates_t[current_reader].cbAtr; j++)
                    atrVector.push_back(rgReaderStates_t[current_reader].rgbAtr[j]);
                
                if (!validAtr( atrVector))
                    return false;
            }
#endif

            if (rgReaderStates_t[current_reader].dwEventState &
                SCARD_STATE_PRESENT)
            {
                if (connectCard()) {            // Get valid handle
                    processValidCard(hContext); // Processed by subclass
                    disconnectCard();
                    newCardDataAcquired = true;
                }
            }
        } // for reader
        
#if 1
        break; // @@@ TODO: only one loop for now
#else
        /// 820
        rv = SCardGetStatusChange(hContext, TIMEOUT, rgReaderStates_t,
                    nbReaders);
#endif
    } // while
    
    return newCardDataAcquired;
}

void SmartCard::start()
{
    //std::clog << __PRETTY_FUNCTION__ << std::endl;
    
    // 453
    LONG rv = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
    CHECK("SCardEstablishContext", rv)

    /// 456
    rgReaderStates[0].szReader = "\\\\?PnP?\\Notification";
    rgReaderStates[0].dwCurrentState = SCARD_STATE_UNAWARE;

    rv = SCardGetStatusChange(hContext, 0, rgReaderStates, 1);
    if (rgReaderStates[0].dwEventState & SCARD_STATE_UNKNOWN)
    {
        std::clog << "Plug'n play reader name not supported. Using polling"
        //<< " every " << TIMEOUT << " ms."
        << std::endl;
        pnp = false;
    }
    else
    {
        std::clog << "Using reader plug'n play mechanism\n";
    }

    getReaders();
}

void SmartCard::stop()
{
    //std::clog << __PRETTY_FUNCTION__ << std::endl;
    
    LONG rv;

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
    //std::clog << __FUNCTION__ << std::endl;
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
#ifndef NDEBUG
    std::clog << __FUNCTION__ << " filePath:" << std::hex;
    for (int i=0; i < filePath.size(); i++) {
        std::clog << " " << std::setw(2) << std::setfill('0')
        << (int)filePath[i];
    }
    std::clog << std::dec << std::endl;
#endif

    std::vector<BYTE> cmd = {
        0x00,
        INS_SELECT_FILE,
        8,      // P1: Selection by path from MF
        0       // P2
    };
    
    cmd.push_back(filePath.size());
    cmd.insert(cmd.end(), filePath.begin(), filePath.end());
    // no Le
    
    std::vector<BYTE> dataResponse;
    sendIns(cmd, dataResponse);
}

// CLA INS P1 P2 - Lc Data Le
void SmartCard::sendIns(const std::vector<BYTE> &cmd2, std::vector<BYTE> &response)
{
    const unsigned char *cmd = cmd2.data();
    DWORD cmdLength = cmd2.size();

    BYTE pbRecvBuffer[256 + 2];
    DWORD dwRecvLength = sizeof(pbRecvBuffer);
    LONG rv = SCardTransmit(hCard, &pioSendPci, cmd, cmdLength, NULL, pbRecvBuffer, &dwRecvLength);
    CHECK("SCardTransmit", rv)

#ifndef NDEBUG
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
    }
    
    std::clog << std::dec << std::endl;
#endif

    response.clear();
    for (int i=0; i < dwRecvLength; i++)
        response.push_back(pbRecvBuffer[i]);
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
#ifndef NDEBUG
    std::clog << "Scanning present readers...\n";
#endif

#ifdef SCARD_AUTOALLOCATE
    dwReaders = SCARD_AUTOALLOCATE;
    LONG rv = SCardListReaders(hContext, NULL, (LPTSTR)&mszReaders, &dwReaders);
    CHECK_RV("SCardListReaders", rv)

    dwReadersOld = dwReaders;
#else
    LONG rv = SCardListReaders(hContext, NULL, NULL, &dwReaders);
    if (rv != SCARD_E_NO_READERS_AVAILABLE)
        CHECK_RV("SCardListReaders", rv)
        
    /// 504
    dwReadersOld = dwReaders;
    
    /// 506
    /* if non NULL we came back so free first */
    if (mszReaders) {
        free(mszReaders);
        mszReaders = nullptr;
    }
    
    /// 513
    mszReaders = (LPTSTR)malloc(sizeof(char)*dwReaders);

    /// 520
    *mszReaders = '\0';
    rv = SCardListReaders(hContext, NULL, mszReaders, &dwReaders);
#endif // SCARD_AUTOALLOCATE

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
        std::clog << "Waiting for the first reader...\n";

        // 541
        if (pnp)
        {
            rgReaderStates[0].szReader = "\\\\?PnP?\\Notification";
            rgReaderStates[0].dwCurrentState = SCARD_STATE_UNAWARE;
            
            /// 547
            do
            {
                rv = SCardGetStatusChange(hContext, TIMEOUT, rgReaderStates, 1);
                break; // @@@
            }
            while (SCARD_E_TIMEOUT == rv);

            std::clog << __FUNCTION__ << " line: " << __LINE__
            << std::hex << " rv: 0x" << rv << std::dec << std::endl;
        }
        else
        {
            /// 563
            rv = SCARD_S_SUCCESS; // 0
            while ((SCARD_S_SUCCESS == rv) && (dwReaders == dwReadersOld))
            {
                rv = SCardListReaders(hContext, NULL, NULL, &dwReaders);

                std::clog << __FUNCTION__ << " line: " << __LINE__
                << std::hex << " rv: 0x" << rv << std::dec << std::endl;

                if (SCARD_E_NO_READERS_AVAILABLE == rv)
                    rv = SCARD_S_SUCCESS;
                
                //sleep(1);
                break; // @@@ TODO: TBC return rv ?
            } // while

            if (SCARD_E_NO_READERS_AVAILABLE == rv) { // 0x8010002E
                std::clog << "SCARD_E_NO_READERS_AVAILABLE\n";
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
    
#ifndef NDEBUG
    /// 602
    // print_readers
    for (int i = 0; i < nbReaders; i++)
        printf("\t%d: <%s>\n", i, readers[i]);
#endif
    
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
    
    return SCARD_S_SUCCESS;
}

bool SmartCard::connectCard()
{
    DWORD dwActiveProtocol = SCARD_PROTOCOL_UNDEFINED;

    LONG rv = SCardConnect(hContext, mszReaders, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &hCard, &dwActiveProtocol);
    
    if (SCARD_S_SUCCESS != rv) { // SCARD_E_NO_SMARTCARD
        printf("SCardConnect: 0x%lx %s\n", rv, pcsc_stringify_error(rv));
        return false;
    }

    switch (dwActiveProtocol)
    {
        case SCARD_PROTOCOL_T0:
            pioSendPci = *SCARD_PCI_T0;
            break;

        case SCARD_PROTOCOL_T1:
            pioSendPci = *SCARD_PCI_T1;
            break;
            
        case SCARD_PROTOCOL_RAW:
            pioSendPci = *SCARD_PCI_RAW;
            break;

        default:
            std::cerr << __PRETTY_FUNCTION__ << " Unknown protocol\n";
            break;
    }

#ifndef NDEBUG
    std::clog << "\tprotocol " << dwActiveProtocol <<"\n";
#endif
    
    return true;
}

void SmartCard::disconnectCard()
{
    LONG rv = SCardDisconnect(hCard, SCARD_LEAVE_CARD);
    CHECK("SCardDisconnect", rv)
}

/// 729
void SmartCard::dumpState(uint32_t state)
{
#ifndef NDEBUG
    std::clog << ", state: ";

    if (state & SCARD_STATE_IGNORE)
        std::clog << "Ignore this reader, ";

    if (state & SCARD_STATE_UNKNOWN)
    {
        std::clog << "Unknown, ";
        //goto get_readers;
    }

    if (state & SCARD_STATE_UNAVAILABLE)
        std::clog << "Status unavailable, ";
    
    if (state & SCARD_STATE_EMPTY)
        std::clog << "Card  removed, ";

    if (state & SCARD_STATE_PRESENT)
        std::clog << "Card inserted, ";
    
    if (state & SCARD_STATE_ATRMATCH)
        std::clog << "ATR matches card, ";

    if (state & SCARD_STATE_EXCLUSIVE)
        std::clog << "Exclusive Mode, ";

    if (state & SCARD_STATE_INUSE)
        std::clog << "Shared Mode, ";

    if (state & SCARD_STATE_MUTE)
        std::clog << "Unresponsive card, ";
    
    std::clog << std::endl;
#endif
}
