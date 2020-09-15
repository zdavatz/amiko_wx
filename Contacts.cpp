//  Contacts.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 1 Sep 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <vector>
#include <iostream>
#include <fstream>

#include <wx/wx.h>
#include <wx/textfile.h>
#include <wx/filedlg.h>

#include "Contacts.hpp"

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
// Read Google CSV file
// See issue #30
void Contacts::addAllContactsToArray(std::vector<Patient *> &arrayOfContacts)
{
#if 1
    wxFileDialog fdlog(wxTheApp->GetTopWindow(),
                       _("Select Google CSV file"),
                       wxEmptyString, // defaultDir
                       wxEmptyString, // defaultFile
                       _("CSV files (*.csv)|*.CSV"),
                       wxFD_OPEN | wxFD_FILE_MUST_EXIST); // no wxFD_MULTIPLE

    if (fdlog.ShowModal() != wxID_OK)
        return;

    wxTextFile tfile;
    tfile.Open( fdlog.GetPath());

    wxString header = tfile.GetFirstLine();
    wxArrayString columns = wxSplit(header, ',');
    std::clog << "Num columns: " << columns.size() << std::endl;

    wxString str;
    while (!tfile.Eof()) {
        str = tfile.GetNextLine();
        std::clog << " Line: <" << str << ">\n";
    }
#else
    CNAuthorizationStatus status = [CNContactStore authorizationStatusForEntityType:CNEntityTypeContacts];
    if (status == CNAuthorizationStatusDenied) {
        NSLog(@"This app was refused permissions to contacts.");
    }
    
    if ([CNContactStore class]) {
        CNContactStore *addressBook = [[CNContactStore alloc] init];
        
        NSArray *keys = @[CNContactIdentifierKey,
                          CNContactFamilyNameKey,
                          CNContactGivenNameKey,
                          CNContactBirthdayKey,
                          CNContactPostalAddressesKey,
                          CNContactPhoneNumbersKey,
                          CNContactEmailAddressesKey];
        
        CNContactFetchRequest *request = [[CNContactFetchRequest alloc] initWithKeysToFetch:keys];
        
        NSError *error;
        [addressBook enumerateContactsWithFetchRequest:request
                                                 error:&error
                                            usingBlock:^(CNContact * __nonnull contact, BOOL * __nonnull stop) {
                                                if (error) {
                                                    NSLog(@"error fetching contacts %@", error);
                                                } else {
                                                    MLPatient *patient = [[MLPatient alloc] init];
                                                    patient.familyName = contact.familyName;
                                                    patient.givenName = contact.givenName;
                                                    // Postal address
                                                    patient.postalAddress = @"";
                                                    patient.zipCode = @"";
                                                    patient.city = @"";
                                                    patient.country = @"";
                                                    if ([contact.postalAddresses count]>0) {
                                                        CNPostalAddress *pa = [contact.postalAddresses[0] value];
                                                        patient.postalAddress = pa.street;
                                                        patient.zipCode = pa.postalCode;
                                                        patient.city = pa.city;
                                                        patient.country = pa.country;
                                                    }
                                                    // Email address
                                                    patient.emailAddress = @"";
                                                    if ([contact.emailAddresses count]>0)
                                                        patient.emailAddress = [contact.emailAddresses[0] value];
                                                    // Birthdate
                                                    patient.birthDate = @"";
                                                    if (contact.birthday.year>1900)
                                                        patient.birthDate = [NSString stringWithFormat:@"%ld-%ld-%ld", contact.birthday.day, contact.birthday.month, contact.birthday.year];
                                                    // Phone number
                                                    patient.phoneNumber = @"";
                                                    if ([contact.phoneNumbers count]>0)
                                                        patient.phoneNumber = [[contact.phoneNumbers[0] value] stringValue];
                                                    // Add only if patients names are meaningful
                                                    if ([patient.familyName length]>0 && [patient.givenName length]>0) {
                                                        patient.databaseType = eAddressBook;
                                                        [arrayOfContacts addObject:patient];
                                                    }
                                                }
                                            }];
        // Sort alphabetically
        if ([arrayOfContacts count]>0) {
            NSSortDescriptor *nameSort = [NSSortDescriptor sortDescriptorWithKey:@"familyName" ascending:YES];
            [arrayOfContacts sortUsingDescriptors:[NSArray arrayWithObject:nameSort]];
        }
    }
#endif

    //return arrayOfContacts;
}
