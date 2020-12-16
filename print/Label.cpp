///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.9.0 Nov 11 2020)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Label.h"

///////////////////////////////////////////////////////////////////////////

myLabelPanel2::myLabelPanel2( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );

	labelDoctor = new wxStaticText( this, wxID_ANY, wxT("Doctor"), wxDefaultPosition, wxDefaultSize, 0 );
	labelDoctor->Wrap( -1 );
	labelDoctor->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	bSizer5->Add( labelDoctor, 0, wxALL, 5 );

	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxSize( -1,2 ), wxLI_HORIZONTAL );
	m_staticline1->SetForegroundColour( wxColour( 0, 0, 0 ) );
	m_staticline1->SetBackgroundColour( wxColour( 0, 0, 0 ) );

	bSizer5->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );

	labelPatient = new wxStaticText( this, wxID_ANY, wxT("Patient"), wxDefaultPosition, wxDefaultSize, 0 );
	labelPatient->Wrap( -1 );
	bSizer5->Add( labelPatient, 0, wxALL, 5 );

	labelMedicine = new wxStaticText( this, wxID_ANY, wxT("Medicine"), wxDefaultPosition, wxDefaultSize, 0 );
	labelMedicine->Wrap( -1 );
	labelMedicine->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	bSizer5->Add( labelMedicine, 0, wxALL, 5 );

	labelComment = new wxStaticText( this, wxID_ANY, wxT("Comment"), wxDefaultPosition, wxDefaultSize, 0 );
	labelComment->Wrap( -1 );
	bSizer5->Add( labelComment, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );

	labelSwissmed = new wxStaticText( this, wxID_ANY, wxT("Swissmed"), wxDefaultPosition, wxDefaultSize, 0 );
	labelSwissmed->Wrap( -1 );
	bSizer6->Add( labelSwissmed, 0, wxALL, 5 );


	bSizer6->Add( 0, 0, 1, wxEXPAND, 5 );

	labelPrice = new wxStaticText( this, wxID_ANY, wxT("Price"), wxDefaultPosition, wxDefaultSize, 0 );
	labelPrice->Wrap( -1 );
	bSizer6->Add( labelPrice, 0, wxALL, 5 );


	bSizer6->Add( 0, 0, 1, wxEXPAND, 5 );


	bSizer5->Add( bSizer6, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer5 );
	this->Layout();
}

myLabelPanel2::~myLabelPanel2()
{
}

myLabelPanel::myLabelPanel( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );

	labelDoctor = new wxStaticText( this, wxID_ANY, wxT("Doctor"), wxDefaultPosition, wxDefaultSize, 0 );
	labelDoctor->Wrap( -1 );
	labelDoctor->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );

	bSizer5->Add( labelDoctor, 0, wxALL, 5 );

	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxSize( -1,2 ), wxLI_HORIZONTAL );
	m_staticline1->SetForegroundColour( wxColour( 0, 0, 0 ) );
	m_staticline1->SetBackgroundColour( wxColour( 0, 0, 0 ) );

	bSizer5->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );

	labelPatient = new wxStaticText( this, wxID_ANY, wxT("Patient"), wxDefaultPosition, wxDefaultSize, 0 );
	labelPatient->Wrap( -1 );
	bSizer5->Add( labelPatient, 0, wxALL, 5 );

	labelMedicine = new wxStaticText( this, wxID_ANY, wxT("Medicine"), wxDefaultPosition, wxDefaultSize, 0 );
	labelMedicine->Wrap( -1 );
	labelMedicine->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	bSizer5->Add( labelMedicine, 0, wxALL, 5 );

	labelComment = new wxStaticText( this, wxID_ANY, wxT("Comment"), wxDefaultPosition, wxDefaultSize, 0 );
	labelComment->Wrap( -1 );
	bSizer5->Add( labelComment, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );

	labelSwissmed = new wxStaticText( this, wxID_ANY, wxT("Swissmed"), wxDefaultPosition, wxDefaultSize, 0 );
	labelSwissmed->Wrap( -1 );
	bSizer6->Add( labelSwissmed, 0, wxALL, 5 );


	bSizer6->Add( 0, 0, 1, wxEXPAND, 5 );

	labelPrice = new wxStaticText( this, wxID_ANY, wxT("Price"), wxDefaultPosition, wxDefaultSize, 0 );
	labelPrice->Wrap( -1 );
	bSizer6->Add( labelPrice, 0, wxALL, 5 );


	bSizer6->Add( 0, 0, 1, wxEXPAND, 5 );


	bSizer5->Add( bSizer6, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer5 );
	this->Layout();

	this->Centre( wxBOTH );
}

myLabelPanel::~myLabelPanel()
{
}
