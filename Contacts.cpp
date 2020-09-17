//  Contacts.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 1 Sep 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <istream>

#include <wx/wx.h>
#include <wx/textfile.h>
#include <wx/filedlg.h>

#include "Contacts.hpp"
#include "Patient.hpp"

enum {
    CONTACT_IMPORT_CSV_GOOGLE,
    CONTACT_IMPORT_CSV_OUTLOOK,
    CONTACT_IMPORT_VCARD
};

// /////////////////////////////////////////////////////////////////////////////
// Not in amiko-osx
// Adapted from http://www.zedwood.com/article/cpp-csv-parser
std::vector<std::string>
Contacts::csv_read_row(std::istream &in, char delimiter)
{
    std::stringstream ss;
    bool inquotes = false;
    std::vector<std::string> row; //relying on RVO
    while (in.good())
    {
        char c = in.get();
        if (!inquotes && c=='"') // begin quotechar
        {
            inquotes = true;
        }
        else if (inquotes && c=='"') // quotechar
        {
            if ( in.peek() == '"') // 2 consecutive quotes resolve to 1
            {
                ss << (char)in.get();
            }
            else // end quotechar
            {
                inquotes = false;
            }
        }
        else if (!inquotes && c==delimiter) // end of field
        {
            row.push_back( ss.str() );
            ss.str("");
        }
        else if (!inquotes && (c=='\r' || c=='\n') )
        {
            if (in.peek()=='\n')
                in.get();

            row.push_back( ss.str() );
            return row;
        }
        else
        {
            ss << c;
        }
    }
}

// 29
static std::vector<Patient *> groupOfContacts;

Contacts::Contacts()
{
    
}

// 40
std::vector<Patient *> Contacts::getAllContacts()
{
    addAllContactsToArray(groupOfContacts);
    return groupOfContacts;
}

// 49
// Read CSV file Google/Outlook
// See issue #30
void Contacts::addAllContactsToArray(std::vector<Patient *> &arrayOfContacts)
{
    wxString wildCard = wxString::Format("CSV %s (*.csv;*.CSV)|*.csv;*.CSV", _("files"));
    wxFileDialog fdlog(wxTheApp->GetTopWindow(),
                       _("Select Google/Outlook CSV file"),
                       wxEmptyString, // defaultDir
                       wxEmptyString, // defaultFile
                       wildCard,
                       wxFD_OPEN | wxFD_FILE_MUST_EXIST); // no wxFD_MULTIPLE

    if (fdlog.ShowModal() != wxID_OK)
        return;
    
    std::ifstream in( fdlog.GetPath().c_str() );
    if (in.fail()) {
        std::cerr << "CSV file not found" << std::endl;
        return;
    }
    
    std::vector<std::string> header;
    if (in.good())
        header = csv_read_row(in, ',');

    int nCol = header.size();
    
    // Create map to associate header cells with indexes
    std::map<wxString, int> headerMap;
    for (int i=0; i<nCol; i++)
        headerMap[header[i]] = i;
    
    // Pre-calculate the needed indexes for faster access later in the while loop
    int idx_name = headerMap["Given Name"];
    int idx_surname = headerMap["Family Name"];
    int idx_birthdate = headerMap["Birthday"];
    int idx_gender = headerMap["Gender"];
    int idx_address = headerMap["Address 1 - Street"];
    int idx_city = headerMap["Address 1 - City"];
    int idx_zip = headerMap["Address 1 - Postal Code"];
    int idx_country = headerMap["Address 1 - Country"];
    
    // Extra checks for optional fields:
    std::map<wxString, int>::iterator it;

    it = headerMap.find("E-mail 1 - Value");
    int idx_email = (it != headerMap.end()) ? it->second : -1;

    it = headerMap.find("Phone 1 - Value");
    int idx_phone = (it != headerMap.end()) ? it->second : -1;

    // We cannog decide between Google or Outlook based on the number of columns,
    // because the Google CSV format has a variable number of columns, depending on the data.
    // The solution is to decide from the first header cell.
    int csvType = (header[0] == "Name") ? CONTACT_IMPORT_CSV_GOOGLE : CONTACT_IMPORT_CSV_OUTLOOK;

    switch (csvType) {
        case CONTACT_IMPORT_CSV_GOOGLE:
            arrayOfContacts.clear();                
            while (in.good())
            {
                std::vector<std::string> row = csv_read_row(in, ',');
                if (row.size() != nCol)
                    continue;

                Patient *patient = new Patient;

                patient->givenName = row[idx_name];
                patient->familyName = row[idx_surname];
                //patient->gender = row[idx_gender]; // TODO:
                patient->birthDate = row[idx_birthdate];
                patient->postalAddress = row[idx_address];
                patient->city = row[idx_city];
                patient->zipCode = row[idx_zip];
                patient->country = row[idx_country];
                // Extra checks for optional fields:
                if (idx_email >= 0) patient->emailAddress = row[idx_email];
                if (idx_phone >= 0) patient->phoneNumber = row[idx_phone];

                patient->databaseType = eAddressBook;
                arrayOfContacts.push_back(patient);
            }
            break;
            
        case CONTACT_IMPORT_CSV_OUTLOOK:
            arrayOfContacts.clear();
            while (in.good())
            {
                std::vector<std::string> row = csv_read_row(in, ',');

                // row.size() is 89 because there is an extra trailing ','
                if (row.size() < nCol)
                    continue;

                Patient *patient = new Patient;

                patient->givenName = row[0];        // A
                patient->familyName = row[2];       // C
                //patient->gender = row[7];         // H  TODO:
                patient->birthDate = row[8];        // I
                patient->emailAddress = row[14];    // O
                patient->phoneNumber = row[17];     // R TODO: fallback S,U
                patient->postalAddress = row[61];   // X TODO: fallback X,BJ
                patient->city = row[65];            // BN
                patient->zipCode = row[67];         // BP
                patient->country = row[68];         // BQ

                patient->databaseType = eAddressBook;
                arrayOfContacts.push_back(patient);
            }
            break;
            
        default:
            std::cerr << "Not a valid Google/Outlook CSV file, " << nCol << " columns\n";
            break;
    }

    std::clog << __FUNCTION__ << " Line " << __LINE__
    << ", # contacts:" << arrayOfContacts.size()
    << std::endl;

    in.close();
}
