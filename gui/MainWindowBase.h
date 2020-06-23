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
#include <wx/html/htmlwin.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/bmpbuttn.h>
#include <wx/dataview.h>
#include <wx/simplebook.h>
#include <wx/splitter.h>
#include <wx/toolbar.h>
#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/listctrl.h>
#include <wx/spinbutt.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

#define wxID_MY_SEARCH_FIELD 1000
#define wxID_BTN_PREPARATION 1001
#define wxID_BTN_REGISTRATION_OWNER 1002
#define wxID_BTN_ACTIVE_SUBSTANCE 1003
#define wxID_BTN_REGISTATION_NUMBER 1004
#define wxID_BTN_THERAPY 1005
#define wxID_BTN_FULL_TEXT 1006
#define wxID_MY_TV 1007
#define wxID_FI_WEBVIEW 1008
#define wxID_PATIENT_SEARCH 1009
#define wxID_SECTION_TITLES 1010
#define wxID_TB_COMPENDIUM 1011
#define wxID_TB_FAVORITES 1012
#define wxID_TB_INTERACTIONS 1013
#define wxID_TB_PRESCRIPTION 1014
#define wxID_UPDATE_DB 1015
#define wxID_LOAD_DB 1016
#define wxID_SET_OPERATOR_ID 1017

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
		void _wxFB_OnSearchPatient( wxCommandEvent& event ){ OnSearchPatient( event ); }
		void _wxFB_OnSelectionDidChange( wxDataViewEvent& event ){ OnSelectionDidChange( event ); }
		void _wxFB_OnToolbarAction( wxCommandEvent& event ){ OnToolbarAction( event ); }
		void _wxFB_OnPrintDocument( wxCommandEvent& event ){ OnPrintDocument( event ); }
		void _wxFB_OnShowAboutPanel( wxCommandEvent& event ){ OnShowAboutPanel( event ); }
		void _wxFB_OnUpdateAipsDatabase( wxCommandEvent& event ){ OnUpdateAipsDatabase( event ); }
		void _wxFB_OnLoadAipsDatabase( wxCommandEvent& event ){ OnLoadAipsDatabase( event ); }
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
		wxPanel* m_panel9;
		wxPanel* m_panel10;
		wxPanel* m_panel11;
		wxButton* m_button7;
		wxTextCtrl* m_textCtrl1;
		wxTextCtrl* m_textCtrl2;
		wxBitmapButton* m_bpButton1;
		wxButton* m_button8;
		wxButton* m_button9;
		wxButton* m_button10;
		wxButton* m_button11;
		wxToolBarToolBase* m_tool1;
		wxToolBarToolBase* m_tool2;
		wxToolBarToolBase* m_tool3;
		wxToolBarToolBase* m_tool4;
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
		virtual void OnSearchPatient( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSelectionDidChange( wxDataViewEvent& event ) { event.Skip(); }
		virtual void OnToolbarAction( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPrintDocument( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnShowAboutPanel( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUpdateAipsDatabase( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnLoadAipsDatabase( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnManagePatients( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSetOperatorIdentity( wxCommandEvent& event ) { event.Skip(); }


	public:
		wxHtmlWindow* myWebView;
		wxStaticText* myPlaceDateField;
		wxDataViewCtrl* myPrescriptionsTable;
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
	private:

	protected:
		wxSplitterWindow* m_splitter2;
		wxPanel* m_panel6Left;
		wxStaticText* m_staticText2;
		wxStaticText* m_staticText3;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textCtrl3;
		wxStaticText* m_staticText5;
		wxTextCtrl* m_textCtrl4;
		wxStaticText* m_staticText6;
		wxTextCtrl* m_textCtrl5;
		wxStaticText* m_staticText9;
		wxTextCtrl* m_textCtrl8;
		wxStaticText* m_staticText10;
		wxTextCtrl* m_textCtrl9;
		wxStaticText* m_staticText11;
		wxTextCtrl* m_textCtrl10;
		wxStaticText* m_staticText12;
		wxTextCtrl* m_textCtrl11;
		wxButton* m_button14;
		wxButton* m_button15;
		wxPanel* m_panel7Right;
		wxSearchCtrl* m_searchCtrl2;
		wxListCtrl* m_listCtrl1;
		wxSpinButton* m_spinBtn1;
		wxStaticText* m_staticText13;
		wxBitmapButton* m_bpButton2;

	public:

		PatientSheetBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 589,387 ), long style = wxDEFAULT_DIALOG_STYLE );
		~PatientSheetBase();

		void m_splitter2OnIdle( wxIdleEvent& )
		{
			m_splitter2->SetSashPosition( 0 );
			m_splitter2->Disconnect( wxEVT_IDLE, wxIdleEventHandler( PatientSheetBase::m_splitter2OnIdle ), NULL, this );
		}

};

