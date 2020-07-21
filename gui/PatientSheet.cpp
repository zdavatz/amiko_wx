#include <iostream>

#include "PatientSheet.h"
#include "Patient.hpp"
#include "PatientDBAdapter.hpp"

PatientSheet::PatientSheet( wxWindow* parent )
: PatientSheetBase( parent )
, mSelectedPatient(nullptr)
, mPatientDb(nullptr)
, mABContactsVisible(false)
, mSearchFiltered(false)
, mFemale(false)
{
    // 750
    mPatientDb = PatientDBAdapter::sharedInstance();

#if 0
    // TODO:
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(controlTextDidChange:)
                                                 name:NSControlTextDidChangeNotification
                                               object:nil];
#endif
}

// 219
Patient * PatientSheet::getAllFields()
{
    long largetRowId = mPatientDb->getLargestRowId();

    Patient *patient = new Patient;

#if 1
    std::clog << __PRETTY_FUNCTION__ << " Line " << __LINE__ << " TODO" << std::endl;
#else
    patient->rowId = largetRowId + 1;
    patient->familyName = [mFamilyName stringValue];
    patient->givenName = [mGivenName stringValue];
    patient->birthDate = [mBirthDate stringValue];
    patient->city = [mCity stringValue];
    patient->zipCode = [mZipCode stringValue];
    patient->postalAddress = [mPostalAddress stringValue];
    patient->weightKg = [mWeight_kg intValue];
    patient->heightCm = [mHeight_cm intValue];
    patient->country = [mCountry stringValue];
    patient->phoneNumber = [mPhone stringValue];
    patient->emailAddress = [mEmail stringValue];
    patient->gender = [mFemaleButton state]==NSOnState ? @"woman" : @"man";
#endif
    
    return patient;
}

// 261
Patient *PatientSheet::retrievePatient()
{
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;

//    if (!mSelectedPatient)
//        mSelectedPatient = new Patient; // still crash

#if 0 // Issue #7
    return mSelectedPatient;
#else
    return nullptr;
#endif
}

// 297
bool PatientSheet::validateFields(Patient *patient)
{
    bool valid = true;
    
#if 1
    std::clog << __PRETTY_FUNCTION__ << " Line " << __LINE__ << " TODO" << std::endl;
#else
    resetFieldsColors();
    
    if ([self stringIsNilOrEmpty:patient.familyName]) {
        mFamilyName.backgroundColor = [NSColor lightRed];
        valid = FALSE;
    }
    if ([self stringIsNilOrEmpty:patient.givenName]) {
        mGivenName.backgroundColor = [NSColor lightRed];
        valid = FALSE;
    }
    if ([self stringIsNilOrEmpty:patient.birthDate]) {
        mBirthDate.backgroundColor = [NSColor lightRed];
        valid = FALSE;
    }
    if ([self stringIsNilOrEmpty:patient.postalAddress]) {
        mPostalAddress.backgroundColor = [NSColor lightRed];
        valid = FALSE;
    }
    if ([self stringIsNilOrEmpty:patient.city]) {
        mCity.backgroundColor = [NSColor lightRed];
        valid = FALSE;
    }
    if ([self stringIsNilOrEmpty:patient.zipCode]) {
        mZipCode.backgroundColor = [NSColor lightRed];
        valid = FALSE;
    }
#endif
    
    return valid;
}

// 409
void PatientSheet::OnSavePatient( wxCommandEvent& event )
{
    if (!mPatientDb) {
        return;
    }
    
    Patient *patient = getAllFields();
    if (!validateFields(patient)) {
        return;
    }

    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
}
