///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.9.0 Jun  5 2020)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/srchctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include "TableViewDelegate.hpp"
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/webview.h>
#include <wx/textctrl.h>
#include <wx/bmpbuttn.h>
#include <wx/dataview.h>
#include <wx/simplebook.h>
#include <wx/splitter.h>
#include <wx/toolbar.h>
#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/radiobox.h>
#include <wx/listctrl.h>
#include <wx/spinbutt.h>
#include <wx/dialog.h>
#include <wx/statbmp.h>

///////////////////////////////////////////////////////////////////////////

#define wxID_MY_SEARCH_FIELD 1000
#define wxID_BTN_PREPARATION 1001
#define wxID_BTN_REGISTRATION_OWNER 1002
#define wxID_BTN_ACTIVE_SUBSTANCE 1003
#define wxID_BTN_REGISTRATION_NUMBER 1004
#define wxID_BTN_THERAPY 1005
#define wxID_BTN_FULL_TEXT 1006
#define wxID_MY_TV 1007
#define wxID_FI_SEARCH_FIELD 1008
#define wxID_FI_FIND_PREVIOUS 1009
#define wxID_FI_FIND_NEXT 1010
#define wxID_FI_FIND_DONE 1011
#define wxID_FI_WEBVIEW 1012
#define wxID_PATIENT_SEARCH 1013
#define wxID_NEW_PRESCRIPTION 1014
#define wxID_CHECK_INTERACTIONS 1015
#define wxID_SAVE_PRESCRIPTION 1016
#define wxID_SEND_PRESCRIPTION 1017
#define wxID_SECTION_TITLES 1018
#define wxID_TB_COMPENDIUM 1019
#define wxID_TB_FAVORITES 1020
#define wxID_TB_INTERACTIONS 1021
#define wxID_TB_PRESCRIPTION 1022
#define wxID_EXPORT_WORDLIST 1023
#define wxID_FI_FIND_SHOW 1024
#define wxID_UPDATE_DB 1025
#define wxID_LOAD_DB 1026
#define wxID_LOAD_PRESCRIPTION 1027
#define wxID_SET_OPERATOR_ID 1028
#define wxID_RB_SEX 1029
#define wxID_SAVE_PATIENT 1030

///////////////////////////////////////////////////////////////////////////////
/// Class MainWindowBase
///////////////////////////////////////////////////////////////////////////////
class MainWindowBase : public wxFrame
{
	DECLARE_EVENT_TABLE()
	private:

		// Private event handlers
		void _wxFB_OnSearchNow( wxCommandEvent& event ){ OnSearchNow( event ); }
		void _wxFB_OnButtonPressed( wxCommandEvent& event ){ OnButtonPressed( event ); }
		void _wxFB_OnSearchFiNow( wxCommandEvent& event ){ OnSearchFiNow( event ); }
		void _wxFB_OnPerformFindAction( wxCommandEvent& event ){ OnPerformFindAction( event ); }
		void _wxFB_OnSearchPatient( wxCommandEvent& event ){ OnSearchPatient( event ); }
		void _wxFB_OnNewPrescription( wxCommandEvent& event ){ OnNewPrescription( event ); }
		void _wxFB_OnCheckForInteractions( wxCommandEvent& event ){ OnCheckForInteractions( event ); }
		void _wxFB_OnSavePrescription( wxCommandEvent& event ){ OnSavePrescription( event ); }
		void _wxFB_OnSendPrescription( wxCommandEvent& event ){ OnSendPrescription( event ); }
		void _wxFB_OnSelectionDidChange( wxDataViewEvent& event ){ OnSelectionDidChange( event ); }
		void _wxFB_OnToolbarAction( wxCommandEvent& event ){ OnToolbarAction( event ); }
		void _wxFB_OnExportWordListSearchResults( wxCommandEvent& event ){ OnExportWordListSearchResults( event ); }
		void _wxFB_OnPrintDocument( wxCommandEvent& event ){ OnPrintDocument( event ); }
		void _wxFB_OnShowAboutPanel( wxCommandEvent& event ){ OnShowAboutPanel( event ); }
		void _wxFB_OnUpdateAipsDatabase( wxCommandEvent& event ){ OnUpdateAipsDatabase( event ); }
		void _wxFB_OnLoadAipsDatabase( wxCommandEvent& event ){ OnLoadAipsDatabase( event ); }
		void _wxFB_OnLoadPrescription( wxCommandEvent& event ){ OnLoadPrescription( event ); }
		void _wxFB_OnManagePatients( wxCommandEvent& event ){ OnManagePatients( event ); }
		void _wxFB_OnSetOperatorIdentity( wxCommandEvent& event ){ OnSetOperatorIdentity( event ); }


	protected:
		wxSplitterWindow* m_splitter1;
		wxPanel* m_panelLeft;
		wxSearchCtrl* mySearchField;
		wxButton* m_button1;
		wxButton* m_button2;
		wxButton* m_button3;
		wxButton* m_button4;
		wxButton* m_button5;
		wxButton* m_button6;
		TableViewDelegate* myTableView;
		wxPanel* m_panelRight;
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
		wxTextCtrl* m_textCtrl1;
		wxTextCtrl* m_textCtrl2;
		wxBitmapButton* m_bpButton1;
		wxButton* btnNewPrescription;
		wxButton* btnCheckInter;
		wxButton* btnSave;
		wxButton* btnSend;
		wxToolBarToolBase* m_tool1;
		wxToolBarToolBase* m_tool2;
		wxToolBarToolBase* m_tool3;
		wxToolBarToolBase* m_tool4;
		wxToolBarToolBase* m_tool7;
		wxToolBarToolBase* m_tool5;
		wxMenuBar* m_menubar1;
		wxMenu* m_menuFile;
		wxMenu* m_menu11;
		wxMenu* m_menuEdit;
		wxMenu* m_menuPrescr;
		wxMenu* m_menuContacts;
		wxMenu* m_menuWindow;
		wxMenu* m_menuHelp;

		// Virtual event handlers, overide them in your derived class
		virtual void OnSearchNow( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonPressed( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSearchFiNow( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPerformFindAction( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSearchPatient( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNewPrescription( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCheckForInteractions( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSavePrescription( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSendPrescription( wxCommandEvent& event ) { event.Skip(); }
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


	public:
		wxBoxSizer* bSizerLeft;
		wxBoxSizer* fiSizer;
		wxPanel* myTextFinder;
		wxStaticText* fiSearchCount;
		wxStaticText* myPlaceDateField;
		wxDataViewCtrl* myPrescriptionsTableView;
		wxDataViewListCtrl* mySectionTitles;
		wxToolBar* m_tbMain;
		wxToolBarToolBase* m_toolAbout;

		MainWindowBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1033,767 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~MainWindowBase();

		void m_splitter1OnIdle( wxIdleEvent& )
		{
			m_splitter1->SetSashPosition( 0 );
			m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( MainWindowBase::m_splitter1OnIdle ), NULL, this );
		}

};

///////////////////////////////////////////////////////////////////////////////
/// Class PatientSheetBase
///////////////////////////////////////////////////////////////////////////////
class PatientSheetBase : public wxDialog
{
	DECLARE_EVENT_TABLE()
	private:

		// Private event handlers
		void _wxFB_OnSelectSex( wxCommandEvent& event ){ OnSelectSex( event ); }
		void _wxFB_OnSavePatient( wxCommandEvent& event ){ OnSavePatient( event ); }


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
		wxRadioBox* mSex;
		wxStaticText* m_staticText24;
		wxStaticText* m_staticText25;
		wxStaticText* m_staticText26;
		wxStaticText* m_staticText27;
		wxButton* m_button14;
		wxButton* m_button15;
		wxPanel* m_panel7Right;
		wxSearchCtrl* m_searchCtrl2;
		wxListCtrl* m_listCtrl1;
		wxSpinButton* m_spinBtn1;
		wxStaticText* m_staticText13;
		wxBitmapButton* m_bpButton2;

		// Virtual event handlers, overide them in your derived class
		virtual void OnSelectSex( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSavePatient( wxCommandEvent& event ) { event.Skip(); }


	public:
		wxTextCtrl* mFamilyName;
		wxTextCtrl* mGivenName;
		wxTextCtrl* mPostalAddress;
		wxTextCtrl* mCity;
		wxTextCtrl* mZipCode;
		wxTextCtrl* mCountry;
		wxTextCtrl* mBirthDate;
		wxTextCtrl* mWeight_kg;
		wxTextCtrl* mHeight_cm;
		wxTextCtrl* mPhone;
		wxTextCtrl* mEmail;
		wxStaticText* mNotificaltion;

		PatientSheetBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Patient Management"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 700,511 ), long style = wxDEFAULT_DIALOG_STYLE );
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
		wxTextCtrl* m_textCtrl12;
		wxStaticText* m_staticText16;
		wxTextCtrl* m_textCtrl13;
		wxStaticText* m_staticText17;
		wxTextCtrl* m_textCtrl14;
		wxStaticText* m_staticText22;
		wxTextCtrl* m_textCtrl16;
		wxStaticText* m_staticText18;
		wxTextCtrl* m_textCtrl17;
		wxStaticText* m_staticText19;
		wxTextCtrl* m_textCtrl18;
		wxStaticText* m_staticText20;
		wxTextCtrl* m_textCtrl19;
		wxStaticBitmap* m_bitmap1;
		wxButton* m_button16;
		wxButton* m_button18;
		wxButton* m_button17;

	public:

		OperatorIDSheetBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Doctor"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 451,512 ), long style = wxDEFAULT_DIALOG_STYLE );
		~OperatorIDSheetBase();

};

