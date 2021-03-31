///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.9.0 Mar 11 2021)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
class SignatureView;

#include <wx/string.h>
#include <wx/srchctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/tglbtn.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/sizer.h>
#include "TableViewDelegate.hpp"
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/webview.h>
#include <wx/textctrl.h>
#include <wx/statbmp.h>
#include <wx/treectrl.h>
#include <wx/simplebook.h>
#include <wx/dataview.h>
#include <wx/splitter.h>
#include <wx/toolbar.h>
#include <wx/menu.h>
#include <wx/timer.h>
#include <wx/frame.h>
#include <wx/radiobox.h>
#include <wx/valtext.h>
#include <wx/listctrl.h>
#include <wx/dialog.h>
#include <wx/checkbox.h>

///////////////////////////////////////////////////////////////////////////

#define wxID_MY_SEARCH_FIELD 1000
#define wxID_BTN_PREPARATION 1001
#define wxID_BTN_REGISTRATION_OWNER 1002
#define wxID_BTN_ACTIVE_SUBSTANCE 1003
#define wxID_BTN_REGISTRATION_NUMBER 1004
#define wxID_BTN_THERAPY 1005
#define wxID_BTN_FULL_TEXT 1006
#define wxID_MY_TV 1007
#define wxID_TABVIEW 1008
#define wxID_FI_SEARCH_FIELD 1009
#define wxID_FI_FIND_PREVIOUS 1010
#define wxID_FI_FIND_NEXT 1011
#define wxID_FI_FIND_DONE 1012
#define wxID_FI_WEBVIEW 1013
#define wxID_PATIENT_SEARCH 1014
#define wxID_MEDICINE_LIST 1015
#define wxID_NEW_PRESCRIPTION 1016
#define wxID_CHECK_INTERACTIONS 1017
#define wxID_SAVE_PRESCRIPTION 1018
#define wxID_SEND_PRESCRIPTION 1019
#define wxID_DELETE_PRESCRIPTION 1020
#define wxID_SECTION_TITLES 1021
#define wxID_TB_COMPENDIUM 1022
#define wxID_TB_FAVORITES 1023
#define wxID_TB_INTERACTIONS 1024
#define wxID_TB_PRESCRIPTION 1025
#define wxID_EXPORT_WORDLIST 1026
#define wxID_FI_FIND_SHOW 1027
#define wxID_UPDATE_DB 1028
#define wxID_LOAD_DB 1029
#define wxID_LOAD_PRESCRIPTION 1030
#define wxID_SET_OPERATOR_ID 1031
#define wxID_SMART_CARD_TICK 1032
#define wxID_RB_SEX 1033
#define wxID_SAVE_PATIENT 1034
#define wxID_PAT_SEARCH_FIELD 1035
#define wxID_PATIENT_LIST 1036
#define wxID_ADD_PATIENT 1037
#define wxID_DELETE_PATIENT 1038
#define wxID_CLEAR_SIGNATURE 1039
#define wxID_LOAD_SIGNATURE 1040
#define wxID_SAVE_OPERATOR 1041

///////////////////////////////////////////////////////////////////////////////
/// Class MainWindowBase
///////////////////////////////////////////////////////////////////////////////
class MainWindowBase : public wxFrame
{
	private:

	protected:
		wxSplitterWindow* m_splitter1;
		wxPanel* m_panelLeft;
		wxSearchCtrl* mySearchField;
		wxToggleButton* m_toggleBtn1;
		wxToggleButton* m_toggleBtn2;
		wxToggleButton* m_toggleBtn3;
		wxToggleButton* m_toggleBtn4;
		wxToggleButton* m_toggleBtn5;
		wxToggleButton* m_toggleBtn6;
		TableViewDelegate* myTableView;
		wxPanel* m_panelRight;
		wxSplitterWindow* m_splitter3;
		wxPanel* m_panel10;
		wxSimplebook* myTabView;
		wxPanel* panel_komp;
		wxSearchCtrl* fiSearchField;
		wxButton* m_button20;
		wxButton* m_button21;
		wxButton* m_button22;
		wxWebView* myWebView;
		wxPanel* panel_inter;
		wxPanel* panel_rezept;
		wxButton* m_button7;
		wxStaticText* mySignLabel;
		wxTreeCtrl* myPrescriptionsTableView;
		wxButton* btnNewPrescription;
		wxButton* btnCheckInter;
		wxPanel* m_panel12;
		wxToolBarToolBase* m_tool1;
		wxToolBarToolBase* m_tool2;
		wxToolBarToolBase* m_tool3;
		wxToolBarToolBase* m_tool4;
		wxToolBarToolBase* m_tool7;
		wxToolBarToolBase* m_tool5;
		wxMenuBar* m_menubar1;
		wxMenu* m_menu11;
		wxMenu* m_menuEdit;
		wxMenu* m_menuPrescr;
		wxMenu* m_menuContacts;
		wxMenu* m_menuWindow;
		wxMenu* m_menuHelp;
		wxTimer m_cardTimer;

		// Virtual event handlers, override them in your derived class
		virtual void OnUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnSearchNow( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonPressed( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSimplebookPageChanged( wxBookCtrlEvent& event ) { event.Skip(); }
		virtual void OnSimplebookPageChanging( wxBookCtrlEvent& event ) { event.Skip(); }
		virtual void OnSearchFiNow( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPerformFindAction( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSearchPatient( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTreeBeginLabelEdit( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnTreeEndLabelEdit( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnTreeItemMenu( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnNewPrescription( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCheckForInteractions( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSavePrescription( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSendPrescription( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDeletePrescription( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSelectionDidChange( wxDataViewEvent& event ) { event.Skip(); }
		virtual void OnToolbarAction( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnExportWordListSearchResults( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPrintDocument( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnShowAboutPanel( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUpdateAipsDatabase( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnLoadAipsDatabase( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnLoadPrescription( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnManagePatients( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSetOperatorIdentity( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSmartCardTick( wxTimerEvent& event ) { event.Skip(); }


	public:
		wxBoxSizer* bSizerLeft;
		wxBoxSizer* fiSizer;
		wxPanel* myTextFinder;
		wxStaticText* fiSearchCount;
		wxTextCtrl* myPatientAddressTextField;
		wxStaticText* myPlaceDateField;
		wxBoxSizer* bSizer12_doctor;
		wxTextCtrl* myOperatorIDTextField;
		SignatureView* mySignView;
		wxButton* saveButton;
		wxButton* sendButton;
		wxButton* btnDelAmk;
		wxDataViewListCtrl* mySectionTitles;
		wxToolBar* myToolbar;
		wxToolBarToolBase* m_toolAbout;
		wxMenu* m_menuFile;

		MainWindowBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1033,767 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~MainWindowBase();

		void m_splitter1OnIdle( wxIdleEvent& )
		{
			m_splitter1->SetSashPosition( 300 );
			m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( MainWindowBase::m_splitter1OnIdle ), NULL, this );
		}

		void m_splitter3OnIdle( wxIdleEvent& )
		{
			m_splitter3->SetSashPosition( 500 );
			m_splitter3->Disconnect( wxEVT_IDLE, wxIdleEventHandler( MainWindowBase::m_splitter3OnIdle ), NULL, this );
		}

};

///////////////////////////////////////////////////////////////////////////////
/// Class SplashWindowBase
///////////////////////////////////////////////////////////////////////////////
class SplashWindowBase : public wxFrame
{
	private:

	protected:
		wxStaticBitmap* m_slashBitmap;

	public:

		SplashWindowBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1000,670 ), long style = 0|wxTAB_TRAVERSAL );

		~SplashWindowBase();

};

///////////////////////////////////////////////////////////////////////////////
/// Class PatientSheetBase
///////////////////////////////////////////////////////////////////////////////
class PatientSheetBase : public wxDialog
{
	private:

	protected:
		wxSplitterWindow* m_splitter2;
		wxPanel* m_panel6Left;
		wxStaticText* m_staticText2;
		wxStaticText* m_staticText3;
		wxStaticText* m_staticText4;
		wxStaticText* m_staticText5;
		wxStaticText* m_staticText6;
		wxStaticText* m_staticText9;
		wxStaticText* m_staticText10;
		wxStaticText* m_staticText11;
		wxStaticText* m_staticText12;
		wxStaticText* m_staticText23;
		wxStaticText* m_staticText24;
		wxStaticText* m_staticText25;
		wxStaticText* m_staticText26;
		wxStaticText* m_staticText27;
		wxButton* m_button14;
		wxButton* m_button15;
		wxPanel* m_panel7Right;
		wxButton* m_button22;
		wxButton* m_button23;
		wxButton* m_button21;

		// Virtual event handlers, override them in your derived class
		virtual void OnActivate( wxActivateEvent& event ) { event.Skip(); }
		virtual void OnUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnSelectSex( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCancel( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSavePatient( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSearchDatabase( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSelectPatient( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnListItemSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnNewPatient( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDeletePatient( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnShowContacts( wxCommandEvent& event ) { event.Skip(); }


	public:
		wxTextCtrl* mFamilyName;
		wxTextCtrl* mGivenName;
		wxTextCtrl* mPostalAddress;
		wxTextCtrl* mCity;
		wxTextCtrl* mZipCode;
		wxTextCtrl* mCountry;
		wxTextCtrl* mBirthDate;
		wxRadioBox* mSex;
		wxTextCtrl* mWeight_kg;
		wxTextCtrl* mHeight_cm;
		wxTextCtrl* mPhone;
		wxTextCtrl* mEmail;
		wxStaticText* mNotification;
		wxSearchCtrl* mSearchKey;
		wxListCtrl* mTableView;
		wxStaticText* mNumPatients;

		PatientSheetBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Patient Management"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 700,594 ), long style = wxDEFAULT_DIALOG_STYLE );
		~PatientSheetBase();

		void m_splitter2OnIdle( wxIdleEvent& )
		{
			m_splitter2->SetSashPosition( 0 );
			m_splitter2->Disconnect( wxEVT_IDLE, wxIdleEventHandler( PatientSheetBase::m_splitter2OnIdle ), NULL, this );
		}

};

///////////////////////////////////////////////////////////////////////////////
/// Class OperatorIDSheetBase
///////////////////////////////////////////////////////////////////////////////
class OperatorIDSheetBase : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText14;
		wxStaticText* m_staticText15;
		wxStaticText* m_staticText16;
		wxStaticText* m_staticText17;
		wxStaticText* m_staticText22;
		wxStaticText* m_staticText18;
		wxStaticText* m_staticText27;
		wxStaticText* m_staticText19;
		wxStaticText* m_staticText20;
		wxButton* m_button20;
		wxButton* m_button16;
		wxButton* m_button18;
		wxButton* m_button17;

		// Virtual event handlers, override them in your derived class
		virtual void OnClearSignature( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnLoadSignature( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSaveOperator( wxCommandEvent& event ) { event.Skip(); }


	public:
		wxTextCtrl* mTitle;
		wxTextCtrl* mFamilyName;
		wxTextCtrl* mGivenName;
		wxTextCtrl* mPostalAddress;
		wxTextCtrl* mCity;
		wxTextCtrl* mZipCode;
		wxTextCtrl* mPhoneNumber;
		wxTextCtrl* mEmailAddress;
		SignatureView* mSignView;

		OperatorIDSheetBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Doctor"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 337,608 ), long style = wxDEFAULT_DIALOG_STYLE );
		~OperatorIDSheetBase();

};

///////////////////////////////////////////////////////////////////////////////
/// Class GoogleAuthSheetBase
///////////////////////////////////////////////////////////////////////////////
class GoogleAuthSheetBase : public wxDialog
{
	private:

	protected:
		wxWebView* myWebView;
		wxButton* m_closeButton;

		// Virtual event handlers, override them in your derived class
		virtual void OnCloseClicked( wxCommandEvent& event ) { event.Skip(); }


	public:
		wxBoxSizer* fiSizer;

		GoogleAuthSheetBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 800,600 ), long style = wxDEFAULT_DIALOG_STYLE );
		~GoogleAuthSheetBase();

};

///////////////////////////////////////////////////////////////////////////////
/// Class SyncPreferencesBase
///////////////////////////////////////////////////////////////////////////////
class SyncPreferencesBase : public wxDialog
{
	private:

	protected:
		wxCheckBox* syncCheckbox;
		wxButton* closeButton;

		// Virtual event handlers, override them in your derived class
		virtual void OnCheckboxClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCloseClicked( wxCommandEvent& event ) { event.Skip(); }


	public:

		SyncPreferencesBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~SyncPreferencesBase();

};

///////////////////////////////////////////////////////////////////////////////
/// Class AboutDialogBase
///////////////////////////////////////////////////////////////////////////////
class AboutDialogBase : public wxDialog
{
	private:

	protected:
		wxStaticBitmap* mLogoBitmap;
		wxStaticText* m_staticText29;
		wxStaticText* mVersionLabel;
		wxButton* mUpdateButton;
		wxButton* mCloseButton;

		// Virtual event handlers, override them in your derived class
		virtual void OnUpdateClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCloseClick( wxCommandEvent& event ) { event.Skip(); }


	public:

		AboutDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 450,171 ), long style = wxCAPTION );
		~AboutDialogBase();

};

