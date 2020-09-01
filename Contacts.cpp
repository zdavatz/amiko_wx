//  Contacts.cpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 1 Sep 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#include <vector>

#include <wx/wx.h>

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
void Contacts::addAllContactsToArray(std::vector<Patient *> &arrayOfContacts)
{
#ifdef __APPLE__
#if 1
    std::clog << __FUNCTION__ << " Line " << __LINE__ << " TODO" << std::endl;
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
#endif // __APPLE__
    
#ifdef __linux__
    // TODO: read CSV file, see issue #30
#endif
    
    //return arrayOfContacts;
}
