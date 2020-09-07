#include <iostream>

#include "PatientSheet.h"
#include "Patient.hpp"
#include "PatientDBAdapter.hpp"
#include "Contacts.hpp"
#include "MainWindow.h"

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

// 151
void PatientSheet::resetAllFields()
{
    resetFieldsColors();
    
    mFamilyName->SetValue(wxEmptyString);
    mGivenName->SetValue(wxEmptyString);
    mBirthDate->SetValue(wxEmptyString);
    mCity->SetValue(wxEmptyString);
    mZipCode->SetValue(wxEmptyString);
    mWeight_kg->SetValue(wxEmptyString);
    mHeight_cm->SetValue(wxEmptyString);
    mPostalAddress->SetValue(wxEmptyString);
    mZipCode->SetValue(wxEmptyString);
    mCity->SetValue(wxEmptyString);
    mCountry->SetValue(wxEmptyString);
    mPhone->SetValue(wxEmptyString);
    mEmail->SetValue(wxEmptyString);

    mSex->SetSelection(0);

    mPatientUUID = wxEmptyString;
    
    mNotification->SetLabel(wxEmptyString);
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
    mNotification->SetLabel(wxString::Format(_("The contact was saved in the %s address book"), wxString(APP_NAME)));
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

// 277
wxString PatientSheet::retrievePatientAsString()
{
    if (mSelectedPatient)
        return mSelectedPatient->asString();

    return wxEmptyString;
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
    mArrayOfPatients = mPatientDb->getAllPatients();
    mABContactsVisible = false;
    reloadData(); // mTableView
    setNumPatients(mArrayOfPatients.size());
}

// 484
void PatientSheet::deletePatientFolder_withBackup(Patient *patient, bool backup)
{
#if 1
    std::clog << __FUNCTION__ << " Line " << __LINE__ << " TODO" << std::endl;
#else
    wxString documentsDir = UTI::documentsDirectory();
    NSString *patientDir = [documentsDir stringByAppendingString:[NSString stringWithFormat:@"/%@", patient.uniqueId]];
    
    if (backup) {
        NSString *backupDir = [documentsDir stringByAppendingString:[NSString stringWithFormat:@"/.%@", patient.uniqueId]];
        [[NSFileManager defaultManager] moveItemAtPath:patientDir toPath:backupDir error:nil];
    }
    else {
        [[NSFileManager defaultManager] removeItemAtPath:patientDir error:nil];
    }
#endif
}

// 537
void PatientSheet::setNumPatients(int numPatients)
{
    wxString label;
    if (mABContactsVisible) {
        label = wxString::Format(_("Address Book Mac (%d)"), numPatients);
    }
    else {
        label = wxString::Format(_("Address Book %s (%d)"),
                                 wxString(APP_NAME),
                                 numPatients);
    }
                               
    mNumPatients->SetLabel(label);
    // TODO: center
}

Patient * PatientSheet::getContactAtRow(int row)
{
    if (mSearchFiltered)
        return mFilteredArrayOfPatients[row];

    if (mArrayOfPatients.size() > 0)
        return mArrayOfPatients[row];

    return nullptr;
}

// 562
int PatientSheet::numberOfRowsInTableView() // (NSTableView *)tableView
{
    if (mSearchFiltered)
        return mFilteredArrayOfPatients.size();

    if (mArrayOfPatients.size() > 0)
        return mArrayOfPatients.size();

    return 0;
}

// In amiko-osx the framework calls
//  numberOfRowsInTableView
//  tableView:viewForTableColumn:row:
void PatientSheet::reloadData()
{
    // TODO: clear all data
    mTableView->ClearAll();

    int n = numberOfRowsInTableView();
    for (int i=0; i<n; i++) {
        // 580
        Patient *p = getContactAtRow(i);
        
        if (p) {
            wxString cellStr = wxString::Format("%s %s", p->familyName, p->givenName);
#if 1
            mTableView->InsertItem(i, cellStr);
#else
            wxListItem item;
            item.m_itemId = i;
            item.SetText(cellStr);
            if (p->databaseType == eAddressBook) {
                item.SetTextColour(*wxLIGHT_GREY); // [NSColor grayColor]
            }
            #if 0
            else {
                item.SetTextColour(*wxGREEN); // [NSColor textColor]
            }
            #endif
            mTableView->SetItem(item);
#endif
        }
    }
    
    mTableView->wxWindow::Refresh();
}

// /////////////////////////////////////////////////////////////////////////////

void PatientSheet::OnSelectSex( wxCommandEvent& event )
{
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
}

// 404
void PatientSheet::OnCancel( wxCommandEvent& event )
{
    EndModal(wxID_OK);
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

    // 420
    if (mPatientUUID.length() > 0)
        patient->uniqueId = mPatientUUID;

    if (mPatientDb->getPatientWithUniqueID(mPatientUUID) == nullptr) {
        mPatientUUID = mPatientDb->addEntry(patient);
    }
    else {
        mPatientUUID = mPatientDb->insertEntry(patient); // Update row
    }

    mSearchFiltered = false;
    mSearchKey->SetValue(wxEmptyString);

    // 433
    updateAmiKoAddressBookTableView();
    friendlyNote();
}

// 517
// Double clicked a patient in the table view
void PatientSheet::OnSelectPatient( wxMouseEvent& event )
{
    int row = mTableView->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    std::clog << __FUNCTION__ << " row " << row << std::endl;

    mSelectedPatient = nullptr;
    if (mSearchFiltered) {
        mSelectedPatient = mFilteredArrayOfPatients[row];
    }
    else {
        mSelectedPatient = mArrayOfPatients[row];
    }

#if 0 // TODO: notification
    if (mSelectedPatient) {
        [[NSNotificationCenter defaultCenter] postNotificationName:"MLPrescriptionPatientChanged" object:self];
    }

    EndModal(wxID_OK); // remove
#else
    EndModal(wxID_OK); // remove

    // Call the notification target directly
    MainWindow* vc = (MainWindow *)wxTheApp->GetTopWindow();
    vc->prescriptionPatientChanged();
#endif

}

// 437
void PatientSheet::OnNewPatient( wxCommandEvent& event )
{
    resetAllFields();
    updateAmiKoAddressBookTableView();
}

// 443
void PatientSheet::OnDeletePatient( wxCommandEvent& event )
{
    if (mTableView->GetSelectedItemCount() == 0) // not in amiko-osx
        return;
    
    // 446
    if (mABContactsVisible)
        return;

    int row = mTableView->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    Patient *p;

    if (mSearchFiltered)
        p = mFilteredArrayOfPatients[row];
    else
        p = mArrayOfPatients[row];

    wxMessageDialog alert(this,
                          _("Delete contact?"),
                          _("Are you sure you want to delete this contact from the %s Address Book?"),
                          wxNO_DEFAULT | wxYES | wxCANCEL | wxICON_INFORMATION);
    switch( alert.ShowModal() )
    {
        case wxID_YES:
            // 472
            if (mPatientDb->deleteEntry(p)) {
                deletePatientFolder_withBackup(p, true);
                resetAllFields();
                updateAmiKoAddressBookTableView();
                mNotification->SetLabel(wxString::Format(_("The contact has been removed from the %s Address Book"), wxString(APP_NAME)));
#if 1
                std::clog << __FUNCTION__ << " Line " << __LINE__ << " TODO: postNotification" << std::endl;
#else
                [[NSNotificationCenter defaultCenter] postNotificationName:@"MLPrescriptionPatientDeleted" object:self];
#endif
            }
            break;

        default:
        case wxID_CANCEL:
            break;
    }
}

// 496
// Toggle button on bottom-right corner
void PatientSheet::OnShowContacts( wxCommandEvent& event )
{
    resetAllFields();
    mSearchFiltered = false;
    mSearchKey->SetValue(wxEmptyString);

    if (!mABContactsVisible) {
        Contacts *contacts = new Contacts;
        
        // 505
        // Retrieves contacts from address book
        mArrayOfPatients = contacts->getAllContacts();
        reloadData(); // mTableView
        mABContactsVisible = true;
        setNumPatients(mArrayOfPatients.size());
    }
    else {
        // Retrieves contacts from local patient database
        updateAmiKoAddressBookTableView();
    }
}
