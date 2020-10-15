#include <iostream>

#include <wx/filefn.h>

#include "PatientSheet.h"
#include "PatientDBAdapter.hpp"
#include "Contacts.hpp"
#include "MainWindow.h"
#include "Utilities.hpp"
#include "HealthCard.hpp"

// 46
PatientSheet::PatientSheet( wxWindow* parent )
: PatientSheetBase( parent )
, mSelectedPatient(nullptr)
, mPatientDb(nullptr)
, mABContactsVisible(false)
, mSearchFiltered(false)
, mFemale(false)
, healthCard(nullptr)
{
    // 54
    mPatientDb = PatientDBAdapter::sharedInstance();

    mBirthDate->SetHint(_("DD.MM.YYYY"));
    wxTextValidator tv(wxFILTER_INCLUDE_CHAR_LIST);
    wxString allowedChars = wxS("0123456789.");
    tv.SetCharIncludes(allowedChars);
    mBirthDate->SetValidator(tv);

    mWeight_kg->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
    mHeight_cm->SetValidator(wxTextValidator(wxFILTER_NUMERIC));

#if 0
    // TODO:
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(controlTextDidChange:)
                                                 name:NSControlTextDidChangeNotification
                                               object:nil];
#endif
    
    // 88
    // amiko-osx show
    // Retrieves contacts from local patient database
    updateAmiKoAddressBookTableView();
    mNotification->SetLabel(wxEmptyString);
    
    healthCard = new HealthCard;
}

// category smartCard 16
void PatientSheet::newHealthCardData(PAT_DICT &dict) //NSNotification *)notification
{
    // 23
    resetAllFields();
    
    // 26
    // Create patient from health card data (incomplete dictionary)
    Patient *incompletePatient = new Patient;
    incompletePatient->importFromDict(dict);
#ifndef NDEBUG
    std::clog << "patient " << incompletePatient->description();
#endif
    
    // 31
    // If the ID exists in the patient_db just select it
    Patient *existingPatient =  retrievePatientWithUniqueID(incompletePatient->uniqueId);
    //NSLog(@"%s existing patient from DB:%@", __FUNCTION__, existingPatient);
    if (existingPatient)
    {
        // Search the table view
        int n = numberOfRowsInTableView(); //mTableView
        for (int i=0; i<n; i++) {
            Patient *p = getContactAtRow(i);
            //NSLog(@"Line %d, %i/%ld, %@", __LINE__, i+1, (long)n, p);
            if (p->uniqueId == incompletePatient->uniqueId) {
                std::clog << "found at " << i << std::endl;

#if 0 // TODO:
                // 47
                // Select it in the table view
                NSIndexSet *indexSet = [NSIndexSet indexSetWithIndex:i];
                [mTableView selectRowIndexes:indexSet byExtendingSelection:NO];
                [mTableView scrollRowToVisible:[mTableView selectedRow]];

                {
                // Simulate double-click in mTableView to close panel
                [self setSelectedPatient:existingPatient];
                // Post a notification so that the Rezept view will be updated
                [[NSNotificationCenter defaultCenter] postNotificationName:@"MLPrescriptionPatientChanged" object:self];
                [self remove];
                }

                break;
#endif
            } // if
        } // for

        return;
    }

    // 70
    // Just pre-fill some fields with the dictionary
    setAllFields(incompletePatient);
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

// 175
void PatientSheet::setAllFields(Patient *p)
{
    if (p->familyName.length() > 0)
        mFamilyName->SetValue(p->familyName);
    if (p->givenName.length() > 0)
        mGivenName->SetValue(p->givenName);
    if (p->birthDate.length() > 0)
        mBirthDate->SetValue(p->birthDate);
    if (p->city.length() > 0)
        mCity->SetValue(p->city);
    if (p->zipCode.length() > 0)
        mZipCode->SetValue(p->zipCode);
    if (p->weightKg>0)
        mWeight_kg->SetValue(wxString::Format("%d", p->weightKg));
    if (p->heightCm>0)
        mHeight_cm->SetValue(wxString::Format("%d", p->heightCm));
    if (p->phoneNumber.length() > 0)
        mPhone->SetValue(p->phoneNumber);
    if (p->country.length() > 0)
        mCity->SetValue(p->city);
    if (p->country.length() > 0)
        mCountry->SetValue(p->country);
    if (p->postalAddress.length() > 0)
        mPostalAddress->SetValue(p->postalAddress);
    if (p->emailAddress.length() > 0)
        mEmail->SetValue(p->emailAddress);
    if (p->phoneNumber.length() > 0)
        mPhone->SetValue(p->phoneNumber);
    if (p->uniqueId.length() > 0)
        mPatientUUID = p->uniqueId;
    if (p->gender.length() > 0) {
        if (p->gender == "woman") {
            mFemale = true;
            mSex->SetSelection(0);
        } else {
            mFemale = false;
            mSex->SetSelection(1);
        }
    }
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
    patient->gender = (mSex->GetSelection() == 0) ? "woman" : "man";
    
    return patient;
}

// 241
// friendlyNote
void PatientSheet::friendlyNoteAdded()
{
    mNotification->SetLabel(wxString::Format(_("The contact was added to the %s address book"),
                                             wxString(APP_NAME)));
}
void PatientSheet::friendlyNoteUpdated()
{
    mNotification->SetLabel(wxString::Format(_("The contact was updated in the %s address book"),
                                             wxString(APP_NAME)));
}
void PatientSheet::friendlyNoteDeleted()
{
    mNotification->SetLabel(wxString::Format(_("The contact has been removed from the %s address book"),
                                             wxString(APP_NAME)));
}

// 246
void PatientSheet::addPatient(Patient *patient)
{
    mSelectedPatient = patient;
    mPatientUUID = mPatientDb->addEntry(patient);
    mSearchFiltered = false;
    mSearchKey->SetValue(wxEmptyString);
    updateAmiKoAddressBookTableView();
    friendlyNoteAdded();
}

// 256
void PatientSheet::setSelectedPatient(Patient *patient)
{
    mSelectedPatient = patient;
}

// 261
Patient * PatientSheet::retrievePatient()
{
    return mSelectedPatient;
}

// 266
Patient * PatientSheet::retrievePatientWithUniqueID(wxString uniqueID)
{
    return mPatientDb->getPatientWithUniqueID(uniqueID);
}

// 271
bool PatientSheet::patientExistsWithID(wxString uniqueID)
{
    Patient *p = retrievePatientWithUniqueID(uniqueID);
    return (p != nullptr);
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
    if (patient->birthDate.Length() != 10) {
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
    wxFileName documentsDir( UTI::documentsDirectory(), wxEmptyString); // important for directory names: specify empty filename

    wxFileName patientDir = documentsDir;
    patientDir.AppendDir(patient->uniqueId);

    if (!wxDirExists( patientDir.GetFullPath() ))
        return; // nothing to do

    if (backup) {
        wxFileName backupDir = documentsDir;
        backupDir.AppendDir(wxT(".") + patient->uniqueId);
        wxRenameFile( patientDir.GetFullPath(), backupDir.GetFullPath());
    }
    else {
        wxRmdir( patientDir.GetFullPath());
    }
}

// 537
void PatientSheet::setNumPatients(int numPatients)
{
    wxString label;
    if (mABContactsVisible) {
        label = wxString::Format(_("Address Book Google (%d)"), numPatients);
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
//  - numberOfRowsInTableView
//  - tableView:viewForTableColumn:row:
void PatientSheet::reloadData()
{
    // Clear all data
    mTableView->ClearAll(); // also delete the columns
    wxListItem itemCol;
    itemCol.SetImage(-1);
//    itemCol.SetWidth(mTableView->GetClientRect().GetWidth()); // TODO:
//    itemCol.SetAlign(wxLIST_FORMAT_LEFT);
    mTableView->InsertColumn(0, itemCol); // wxLIST_FORMAT_LEFT, autosize

    int n = numberOfRowsInTableView();
    for (int i=0; i<n; i++) {
        // 580
        Patient *p = getContactAtRow(i);
        
        if (p) {
            wxString cellStr = wxString::Format("%s %s", p->familyName, p->givenName);
#if 1
            long tmp = mTableView->InsertItem(i, cellStr);
            if (p->databaseType == eAddressBook)
                mTableView->SetItemTextColour(tmp, *wxLIGHT_GREY);
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
    
    mTableView->SetColumnWidth(0, -1); // autosize
    mTableView->wxWindow::Refresh();
}

// /////////////////////////////////////////////////////////////////////////////

// Note: we get here also when the Cancel button is clicked
void PatientSheet::OnActivate( wxActivateEvent& event )
{
    //std::clog << __PRETTY_FUNCTION__ << " " << event.GetActive() << std::endl;
}

void PatientSheet::OnIdle( wxIdleEvent& event )
{
    if (!IsActive())
        return;

    if (healthCard->detectChanges())
    {
#ifndef NDEBUG
        std::clog << "Inserted card: "
        << healthCard->familyName << " "
        << healthCard->givenName << std::endl;
#endif
        
        PAT_DICT dict;
        dict[KEY_AMK_PAT_BIRTHDATE] = healthCard->birthDate;
        dict[KEY_AMK_PAT_SURNAME] = healthCard->familyName;
        dict[KEY_AMK_PAT_GENDER] = healthCard->gender;
        dict[KEY_AMK_PAT_NAME] = healthCard->givenName;
        newHealthCardData(dict);
        
        if (healthCard->expired)
            mNotification->SetLabel(_("This card expired"));
    }
}

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
        // Not found in DB
        mPatientUUID = mPatientDb->addEntry(patient);
        friendlyNoteAdded();
    }
    else {
        mPatientUUID = mPatientDb->insertEntry(patient); // Update row
        friendlyNoteUpdated();
    }

    mSearchFiltered = false;
    mSearchKey->SetValue(wxEmptyString);

    // 433
    updateAmiKoAddressBookTableView();
}

// 352
void PatientSheet::OnSearchDatabase( wxCommandEvent& event )
{
    wxString searchKey = mSearchKey->GetValue();
    mFilteredArrayOfPatients.clear(); // removeAllObjects();
    if (searchKey.length() > 0) {
        for (auto p : mArrayOfPatients) {
            wxString searchKeyLower = searchKey.Lower();
            if (p->familyName.Lower().starts_with( searchKeyLower) ||
                p->givenName.Lower().starts_with( searchKeyLower) ||
                p->postalAddress.Lower().starts_with(searchKeyLower) ||
                p->zipCode.starts_with(searchKeyLower))
            {
                mFilteredArrayOfPatients.push_back(p);
            }
        }
    }

    //if (mFilteredArrayOfPatients != nil)
    {
        if (mFilteredArrayOfPatients.size() > 0) {
            setNumPatients(mFilteredArrayOfPatients.size());
            mSearchFiltered = true;
        }
        else {
            if (searchKey.length() > 0) {
                setNumPatients(0);
                mSearchFiltered = true;
            }
            else {
                setNumPatients(mArrayOfPatients.size());
                mSearchFiltered = false;
            }
        }
    }
//    else {
//        setNumPatients(mArrayOfPatients.size());
//        mSearchFiltered = false;
//    }

    reloadData(); // mTableView
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

// 594
// In amiko-osx the framework calls 'tableViewSelectionDidChange'
// Double clicked a patient in the table view
void PatientSheet::OnListItemSelected( wxListEvent& event )
{
    Patient *p = getContactAtRow( event.GetIndex() );
    mPatientUUID = p->uniqueId;
    resetAllFields();
    setAllFields(p);
    mNotification->SetLabel(wxEmptyString);
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
    if (mTableView->GetSelectedItemCount() == 0)  { // not in amiko-osx
#ifndef NDEBUG
        std::clog << __FUNCTION__ << " No patient selected, nothing to do" << std::endl;
#endif
        return;
    }
    
    // 446
    if (mABContactsVisible)
        return;

    int row = mTableView->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    Patient *p;

    if (mSearchFiltered)
        p = mFilteredArrayOfPatients[row];
    else
        p = mArrayOfPatients[row];
    
    wxString caption = wxString::Format(_("Are you sure you want to delete this contact from the %s Address Book?"), APP_NAME);

    wxMessageDialog alert(this,
                          _("Delete contact?"),
                          caption,
                          wxYES_NO | wxICON_INFORMATION);
    switch ( alert.ShowModal() )
    {
        case wxID_YES:
            // 472
            if (mPatientDb->deleteEntry(p)) {
                deletePatientFolder_withBackup(p);
                resetAllFields();
                updateAmiKoAddressBookTableView();
                friendlyNoteDeleted();

                // Call the notification 'PrescriptionPatientDeleted' target directly
                MainWindow* vc = (MainWindow *)wxTheApp->GetTopWindow();
                vc->prescriptionPatientDeleted();
            }
            break;

        default:
        case wxID_NO:
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
