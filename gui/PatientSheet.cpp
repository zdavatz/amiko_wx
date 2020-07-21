#include <iostream>

#include "PatientSheet.h"
#include "Patient.hpp"
#include "PatientDBAdapter.hpp"

// 46
PatientSheet::PatientSheet( wxWindow* parent )
: PatientSheetBase( parent )
, mSelectedPatient(nullptr)
, mPatientDb(nullptr)
, mABContactsVisible(false)
, mSearchFiltered(false)
, mFemale(false)
{
    // 54
    mPatientDb = PatientDBAdapter::sharedInstance();

#if 0
    // TODO:
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(controlTextDidChange:)
                                                 name:NSControlTextDidChangeNotification
                                               object:nil];
#endif
}

// 117
void PatientSheet::resetFieldsColors()
{
    // Get the normal color from a field that was not altered
    wxColour normalColor = mEmail->GetBackgroundColour();
    
    mFamilyName->SetBackgroundColour(normalColor);
    mGivenName->SetBackgroundColour(normalColor);
    mBirthDate->SetBackgroundColour(normalColor);
    mPostalAddress->SetBackgroundColour(normalColor);
    mCity->SetBackgroundColour(normalColor);
    mZipCode->SetBackgroundColour(normalColor);
}

// 219
Patient * PatientSheet::getAllFields()
{
    long largestRowId = mPatientDb->getLargestRowId();

    Patient *patient = new Patient;

    patient->rowId = largestRowId + 1;
    patient->familyName = mFamilyName->GetValue();
    patient->givenName = mGivenName->GetValue();
    patient->birthDate = mBirthDate->GetValue();
    patient->city = mCity->GetValue();
    patient->zipCode = mZipCode->GetValue();
    patient->postalAddress = mPostalAddress->GetValue();
    patient->weightKg = wxAtoi(mWeight_kg->GetValue());
    patient->heightCm = wxAtoi(mHeight_cm->GetValue());
    patient->country = mCountry->GetValue();
    patient->phoneNumber = mPhone->GetValue();
    patient->emailAddress = mEmail->GetValue();
    patient->gender = mSex->GetSelection() == 0 ? "woman" : "man";
    
    return patient;
}

// 241
void PatientSheet::friendlyNote()
{
#if 1
    std::clog << __PRETTY_FUNCTION__ << " Line " << __LINE__ << " TODO" << std::endl;
#else
    [mNotification setStringValue:[NSString stringWithFormat:NSLocalizedString(@"The contact was saved in the %@ address book", nil), [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleName"]]];
#endif
}

// 261
Patient * PatientSheet::retrievePatient()
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

    resetFieldsColors();
    
    wxColour lightRed = wxColour(255, 101, 101);
    
    if (patient->familyName.IsEmpty()) {
        mFamilyName->SetBackgroundColour(lightRed);
        valid = false;
    }
    if (patient->givenName.IsEmpty()) {
        mGivenName->SetBackgroundColour(lightRed);
        valid = false;
    }
    if (patient->birthDate.IsEmpty()) {
        mBirthDate->SetBackgroundColour(lightRed);
        valid = false;
    }
    if (patient->postalAddress.IsEmpty()) {
        mPostalAddress->SetBackgroundColour(lightRed);
        valid = false;
    }
    if (patient->city.IsEmpty()) {
        mCity->SetBackgroundColour(lightRed);
        valid = false;
    }
    if (patient->zipCode.IsEmpty()) {
        mZipCode->SetBackgroundColour(lightRed);
        valid = false;
    }
    
    return valid;
}

// 331
void PatientSheet::updateAmiKoAddressBookTableView()
{
#if 1
    std::clog << __PRETTY_FUNCTION__ << " Line " << __LINE__ << " TODO" << std::endl;
#else
    mArrayOfPatients = [mPatientDb getAllPatients];
    mABContactsVisible=NO;
    [mTableView reloadData];
    [self setNumPatients:[mArrayOfPatients count]];
#endif
}

void PatientSheet::OnSelectSex( wxCommandEvent& event )
{
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
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

    std::clog << __PRETTY_FUNCTION__ << std::endl;

    if (/*mPatientUUID &&*/ mPatientUUID.length() > 0) {
        patient->uniqueId = mPatientUUID;
    }

    if (mPatientDb->getPatientWithUniqueID(mPatientUUID) == nullptr) {
        mPatientUUID = mPatientDb->addEntry(patient);
    }
    else {
        mPatientUUID = mPatientDb->insertEntry(patient); // Update row
    }

    mSearchFiltered = false;
#if 1
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
#else
    mSearchKey.setStringValue("");
#endif
    // 433
    updateAmiKoAddressBookTableView();
    friendlyNote();
}
