//  HealthCard.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 9 Oct 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <iostream>
#include <iomanip>
#include <sstream>

#include <wx/wx.h>
#include <wx/msgdlg.h>

#include "HealthCard.hpp"
#include "MainWindow.h"
#include "PatientSheet.h"
#include "Patient.hpp"

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
    
    switch (tag) {
        case 0x80:  // UTF8InternationalString
        {
            std::string s(value.begin(), value.end());
            wxString ws(s.c_str(), wxConvUTF8);
            wxArrayString a = wxSplit(ws, ',');
            familyName = a[0];
            if (a.size() > 1) {
                givenName = a[1];
                givenName.Trim(true);
                givenName.Trim(false);
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
            wxString ws(s.c_str(), wxConvUTF8);

            // http://www.cplusplus.com/reference/ctime/strftime/
            wxDateTime dt;
            wxString::const_iterator end;
            dt.ParseFormat(ws,"%Y%m%d", &end);      // convert from this
            birthDate = dt.Format(wxT("%d.%m.%Y")); // to this
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
            std::clog << "Sex: <" << gender << ">\n";
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
            std::clog << "Name Of The Institution: " << s << std::endl;
        }
            break;
            
        case 0x92:  // NUMERIC STRING
        {
            std::string s(value.begin(), value.end());
            std::clog << "IdentificationNumber Of The Institution: " << s << std::endl;
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
            //std::clog << "ExpiryDate yyyyMMdd: " << s << std::endl;
            wxString ws(s.c_str(), wxConvUTF8);

            wxDateTime dt;
            wxString::const_iterator end;
            dt.ParseFormat(ws,"%Y%m%d", &end);      // convert from this

            expired = false;
            if (dt < wxDateTime::Now())
                expired = true;
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

// 126
bool HealthCard::validAtr(const std::vector<uint8_t> & atr)
{
    // Swiss Health Insurance Card
    std::vector<uint8_t> mutuelBytes = {
        0x3b, 0x9f, 0x13, 0x81, 0xb1, 0x80, 0x37, 0x1f,
        0x03, 0x80, 0x31, 0xf8, 0x69, 0x4d, 0x54, 0x43,
        0x4f, 0x53, 0x70, 0x02, 0x01, 0x02, 0x81, 0x07, 0x86};

    if (atr == mutuelBytes)
        return true;

    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < atr.size(); i++)
        ss << std::setw(2) << (unsigned)atr[i];

    wxString caption = _("This card can not be read"); // title, top line
    wxString mainMessage = wxString::Format("%s Zeno Davatz\nzdavatz@ywesee.com\n+41 43 540 05 50\nATR: %s", _("Please contact"), ss.str()); // bottom line
    wxMessageDialog alert(nullptr,
                          mainMessage,
                          caption,
                          wxOK | wxICON_ERROR);
    
#if 0 // TODO: test in Linux
    // macOS: top line not shown, 2nd becomes 1st, ext shown as 2nd
    wxString extMessage = wxString::Format("ATR: %s", ss.str());
    alert.SetExtendedMessage(extMessage);
#endif

    alert.ShowModal(); // don't care about return code
    
#ifndef NDEBUG
    std::clog  << caption << std::endl << mainMessage << std::endl;
#endif

    return false;
}

// 157
void HealthCard::processValidCard(SCARDCONTEXT &hContext)
{
    // Elementary file - identification data
    // pdf section 4.8
    std::vector<BYTE> ef_id = { 0x2F, 0x06 };
    scSelectFile(ef_id);

    std::vector<BYTE> cmdReadBinary = {
        0,
        INS_READ_BIN,
        0, 0,   // P1 P2
        84      // Lc
                // no data
    };
    std::vector<BYTE> dataResponse;
    sendIns(cmdReadBinary, dataResponse);
    parseCardData(dataResponse);
    
    // Elementary file - administrative data
    // pdf section 4.9
    std::vector<BYTE> ef_ad = { 0x2F, 0x07 };
    scSelectFile(ef_ad);

    std::vector<BYTE> cmdReadBinary2 = {
        0,
        INS_READ_BIN,
        0, 0,   // P1 P2
        95      // Lc
                // no data
    };
    sendIns(cmdReadBinary2, dataResponse);
    parseCardData(dataResponse);
    
    // 201
    // Post notification 'smartCardDataAcquired'
    MainWindow* vc = (MainWindow *)wxTheApp->GetTopWindow();
    
    PAT_DICT dict;
    dict[KEY_AMK_PAT_BIRTHDATE] = birthDate;
    dict[KEY_AMK_PAT_SURNAME] = familyName;
    dict[KEY_AMK_PAT_GENDER] = gender;
    dict[KEY_AMK_PAT_NAME] = givenName;

    if (vc->mPatientSheet &&
        vc->mPatientSheet->IsVisible())
    {
        // Notification to PatientSheetController
        vc->mPatientSheet->newHealthCardData(dict);
        if (expired)
            vc->mPatientSheet->mNotification->SetLabel(_("This card expired"));
    }
    else
    {
        // Notification to MainWindow
        vc->newHealthCardData(dict);
    }
}
