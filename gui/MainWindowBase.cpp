///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.9.0 Jun  5 2020)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "MainWindowBase.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( MainWindowBase, wxFrame )
	EVT_BUTTON( wxID_BTN_PREPARATION, MainWindowBase::_wxFB_OnButtonPressed )
	EVT_BUTTON( wxID_BTN_REGISTRATION_OWNER, MainWindowBase::_wxFB_OnButtonPressed )
	EVT_BUTTON( wxID_BTN_ACTIVE_SUBSTANCE, MainWindowBase::_wxFB_OnButtonPressed )
	EVT_BUTTON( wxID_BTN_REGISTATION_NUMBER, MainWindowBase::_wxFB_OnButtonPressed )
	EVT_BUTTON( wxID_BTN_THERAPY, MainWindowBase::_wxFB_OnButtonPressed )
	EVT_BUTTON( wxID_BTN_FULL_TEXT, MainWindowBase::_wxFB_OnButtonPressed )
END_EVENT_TABLE()

MainWindowBase::MainWindowBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	mySearchField = new wxSearchCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	#ifndef __WXMAC__
	mySearchField->ShowSearchButton( true );
	#endif
	mySearchField->ShowCancelButton( false );
	bSizer2->Add( mySearchField, 0, wxALL|wxEXPAND, 5 );

	m_button1 = new wxButton( this, wxID_BTN_PREPARATION, _("Preparation"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_button1, 0, wxALL|wxEXPAND, 5 );

	m_button2 = new wxButton( this, wxID_BTN_REGISTRATION_OWNER, _("Registration Owner"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_button2, 0, wxALL|wxEXPAND, 5 );

	m_button3 = new wxButton( this, wxID_BTN_ACTIVE_SUBSTANCE, _("Active Substance / ATC Code"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_button3, 0, wxALL|wxEXPAND, 5 );

	m_button4 = new wxButton( this, wxID_BTN_REGISTATION_NUMBER, _("Registration Number"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_button4, 0, wxALL|wxEXPAND, 5 );

	m_button5 = new wxButton( this, wxID_BTN_THERAPY, _("Therapy"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_button5, 0, wxALL|wxEXPAND, 5 );

	m_button6 = new wxButton( this, wxID_BTN_FULL_TEXT, _("Full Text"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_button6, 0, wxALL|wxEXPAND, 5 );


	bSizer1->Add( bSizer2, 1, wxEXPAND, 5 );

	fiPanel = new wxHtmlWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO );
	bSizer1->Add( fiPanel, 1, wxALL|wxEXPAND, 5 );

	m_dataViewListCtrl1 = new wxDataViewListCtrl( this, wxID_ANY, wxDefaultPosition, wxSize( 200,-1 ), 0 );
	bSizer1->Add( m_dataViewListCtrl1, 0, wxALL, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();

	this->Centre( wxBOTH );
}

MainWindowBase::~MainWindowBase()
{
}
