//  PatientSheetController.hpp
//  AmiKo-wx
//
//  Created by Alex Bettarini on 23 Jun 2020
//  Copyright © 2020 Ywesee GmbH. All rights reserved.

#pragma

class Patient;

class PatientSheetController
{
public:
    PatientSheetController();
    virtual ~PatientSheetController() {}
    
    Patient *retrievePatient();
    
private:
    // .m 35
    Patient *mSelectedPatient;
};
