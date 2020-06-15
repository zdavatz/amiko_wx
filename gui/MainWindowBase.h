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
#include <wx/dataview.h>
#include <wx/sizer.h>
#include <wx/html/htmlwin.h>
#include <wx/bmpbuttn.h>
#include <wx/toolbar.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

#define wxID_BTN_PREPARATION 1000
#define wxID_BTN_REGISTRATION_OWNER 1001
#define wxID_BTN_ACTIVE_SUBSTANCE 1002
#define wxID_BTN_REGISTATION_NUMBER 1003
#define wxID_BTN_THERAPY 1004
#define wxID_BTN_FULL_TEXT 1005

///////////////////////////////////////////////////////////////////////////////
/// Class MainWindowBase
///////////////////////////////////////////////////////////////////////////////
class MainWindowBase : public wxFrame
{
	DECLARE_EVENT_TABLE()
	private:

		// Private event handlers
		void _wxFB_OnButtonPressed( wxCommandEvent& event ){ OnButtonPressed( event ); }


	protected:
		wxSearchCtrl* mySearchField;
		wxButton* m_button1;
		wxButton* m_button2;
		wxButton* m_button3;
		wxButton* m_button4;
		wxButton* m_button5;
		wxButton* m_button6;
		wxDataViewListCtrl* m_dataViewListCtrl2;
		wxDataViewListCtrl* m_dataViewListCtrl1;
		wxToolBar* m_toolBar1;
		wxBitmapButton* m_bpButton1;
		wxBitmapButton* m_bpButton2;
		wxBitmapButton* m_bpButton3;
		wxBitmapButton* m_bpButton4;
		wxBitmapButton* m_bpButton5;
		wxBitmapButton* m_bpButton6;

		// Virtual event handlers, overide them in your derived class
		virtual void OnButtonPressed( wxCommandEvent& event ) { event.Skip(); }


	public:
		wxHtmlWindow* fiPanel;

		MainWindowBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 739,482 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~MainWindowBase();

};

