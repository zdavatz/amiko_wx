//  Contacts.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 1 Sep 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma once

class Patient;

class Contacts
{
public:
    Contacts();
    virtual ~Contacts() {}
    
    std::vector<Patient *> getAllContacts();
    void addAllContactsToArray(std::vector<Patient *> &arrayOfContacts);

private:
    std::vector<std::string> csv_read_row(std::istream &in, char delimiter);
};
