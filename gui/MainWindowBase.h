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
#include <wx/notebook.h>
#include <wx/splitter.h>
#include <wx/toolbar.h>
#include <wx/menu.h>
#include <wx/frame.h>

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
#define wxID_SECTION_TITLES 1009
#define wxID_TB_COMPENDIUM 1010
#define wxID_TB_FAVORITES 1011
#define wxID_TB_INTERACTIONS 1012
#define wxID_TB_PRESCRIPTION 1013
#define wxID_UPDATE_DB 1014
#define wxID_LOAD_DB 1015

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
		void _wxFB_OnSelectionDidChange( wxDataViewEvent& event ){ OnSelectionDidChange( event ); }
		void _wxFB_OnToolbarAction( wxCommandEvent& event ){ OnToolbarAction( event ); }
		void _wxFB_OnPrintDocument( wxCommandEvent& event ){ OnPrintDocument( event ); }
		void _wxFB_OnShowAboutPanel( wxCommandEvent& event ){ OnShowAboutPanel( event ); }
		void _wxFB_OnUpdateAipsDatabase( wxCommandEvent& event ){ OnUpdateAipsDatabase( event ); }
		void _wxFB_OnLoadAipsDatabase( wxCommandEvent& event ){ OnLoadAipsDatabase( event ); }


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
		wxPanel* m_panel3;
		wxPanel* m_panel4;
		wxPanel* m_panel5;
		wxButton* m_button7;
		wxTextCtrl* m_textCtrl1;
		wxStaticText* m_staticText1;
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
		virtual void OnSelectionDidChange( wxDataViewEvent& event ) { event.Skip(); }
		virtual void OnToolbarAction( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPrintDocument( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnShowAboutPanel( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUpdateAipsDatabase( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnLoadAipsDatabase( wxCommandEvent& event ) { event.Skip(); }


	public:
		wxNotebook* myTabView;
		wxHtmlWindow* myWebView;
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

