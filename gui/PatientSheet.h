#ifndef __PatientSheet__
#define __PatientSheet__

/**
@file
Subclass of PatientSheetBase, which is generated by wxFormBuilder.
*/

#include "MainWindowBase.h"

//// end generated include

#include <vector>

class Patient;
class PatientDBAdapter;

/** Implementing PatientSheetBase */
class PatientSheet : public PatientSheetBase
{
	protected:
		// Handlers for PatientSheetBase events.
		void OnSelectSex( wxCommandEvent& event );
		void OnCancel( wxCommandEvent& event );
		void OnSavePatient( wxCommandEvent& event );
		void OnSearchDatabase( wxCommandEvent& event );
		void OnSelectPatient( wxMouseEvent& event );
		void OnListItemSelected( wxListEvent& event );
		void OnNewPatient( wxCommandEvent& event );
		void OnDeletePatient( wxCommandEvent& event );
		void OnShowContacts( wxCommandEvent& event );
	public:
		/** Constructor */
		PatientSheet( wxWindow* parent );
	//// end generated class members

    Patient *retrievePatient();
    Patient *getAllFields();
    bool validateFields(Patient *patient);
    void updateAmiKoAddressBookTableView();
    void friendlyNote();
    void resetFieldsColors();
    wxString retrievePatientAsString();
    void addPatient(Patient *patient);
    
    // 30
    PatientDBAdapter *mPatientDb;

private:
    void setNumPatients(int numPatients);
    Patient * getContactAtRow(int row);
    void resetAllFields();
    void setAllFields(Patient *p);
    void deletePatientFolder_withBackup(Patient *patient, bool backup);
    int numberOfRowsInTableView();
    void reloadData();

    // .m 35
    Patient *mSelectedPatient;
    // .m 37
    std::vector<Patient *> mArrayOfPatients;
    std::vector<Patient *> mFilteredArrayOfPatients;
    wxString mPatientUUID;
    bool mABContactsVisible;    // These are the contacts in the address book
    bool mSearchFiltered;
    bool mFemale;
};

#endif // __PatientSheet__
