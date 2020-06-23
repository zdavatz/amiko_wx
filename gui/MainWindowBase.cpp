///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.9.0 Jun  5 2020)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "MainWindowBase.h"

#include "../res/xpm/AmiKo.xpm"
#include "../res/xpm/compendium.xpm"
#include "../res/xpm/favorites.xpm"
#include "../res/xpm/interactions.xpm"
#include "../res/xpm/prescription.xpm"
#include "../res/xpm/print.xpm"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( MainWindowBase, wxFrame )
	EVT_TEXT( wxID_MY_SEARCH_FIELD, MainWindowBase::_wxFB_OnSearchNow )
	EVT_BUTTON( wxID_BTN_PREPARATION, MainWindowBase::_wxFB_OnButtonPressed )
	EVT_BUTTON( wxID_BTN_REGISTRATION_OWNER, MainWindowBase::_wxFB_OnButtonPressed )
	EVT_BUTTON( wxID_BTN_ACTIVE_SUBSTANCE, MainWindowBase::_wxFB_OnButtonPressed )
	EVT_BUTTON( wxID_BTN_REGISTATION_NUMBER, MainWindowBase::_wxFB_OnButtonPressed )
	EVT_BUTTON( wxID_BTN_THERAPY, MainWindowBase::_wxFB_OnButtonPressed )
	EVT_BUTTON( wxID_BTN_FULL_TEXT, MainWindowBase::_wxFB_OnButtonPressed )
	EVT_BUTTON( wxID_PATIENT_SEARCH, MainWindowBase::_wxFB_OnSearchPatient )
	EVT_DATAVIEW_SELECTION_CHANGED( wxID_SECTION_TITLES, MainWindowBase::_wxFB_OnSelectionDidChange )
	EVT_TOOL( wxID_TB_COMPENDIUM, MainWindowBase::_wxFB_OnToolbarAction )
	EVT_TOOL( wxID_TB_FAVORITES, MainWindowBase::_wxFB_OnToolbarAction )
	EVT_TOOL( wxID_TB_INTERACTIONS, MainWindowBase::_wxFB_OnToolbarAction )
	EVT_TOOL( wxID_TB_PRESCRIPTION, MainWindowBase::_wxFB_OnToolbarAction )
	EVT_TOOL( wxID_PRINT, MainWindowBase::_wxFB_OnPrintDocument )
	EVT_TOOL( wxID_ABOUT, MainWindowBase::_wxFB_OnShowAboutPanel )
	EVT_MENU( wxID_UPDATE_DB, MainWindowBase::_wxFB_OnUpdateAipsDatabase )
	EVT_MENU( wxID_LOAD_DB, MainWindowBase::_wxFB_OnLoadAipsDatabase )
	EVT_MENU( wxID_PATIENT_SEARCH, MainWindowBase::_wxFB_OnManagePatients )
	EVT_MENU( wxID_SET_OPERATOR_ID, MainWindowBase::_wxFB_OnSetOperatorIdentity )
END_EVENT_TABLE()

MainWindowBase::MainWindowBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );

	m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_LIVE_UPDATE );
	m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( MainWindowBase::m_splitter1OnIdle ), NULL, this );

	m_panelLeft = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizerLeft;
	bSizerLeft = new wxBoxSizer( wxVERTICAL );

	mySearchField = new wxSearchCtrl( m_panelLeft, wxID_MY_SEARCH_FIELD, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	#ifndef __WXMAC__
	mySearchField->ShowSearchButton( true );
	#endif
	mySearchField->ShowCancelButton( false );
	bSizerLeft->Add( mySearchField, 0, wxALL|wxEXPAND, 5 );

	m_button1 = new wxButton( m_panelLeft, wxID_BTN_PREPARATION, _("Preparation"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerLeft->Add( m_button1, 0, wxALL|wxEXPAND, 5 );

	m_button2 = new wxButton( m_panelLeft, wxID_BTN_REGISTRATION_OWNER, _("Registration Owner"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerLeft->Add( m_button2, 0, wxALL|wxEXPAND, 5 );

	m_button3 = new wxButton( m_panelLeft, wxID_BTN_ACTIVE_SUBSTANCE, _("Active Substance / ATC Code"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerLeft->Add( m_button3, 0, wxALL|wxEXPAND, 5 );

	m_button4 = new wxButton( m_panelLeft, wxID_BTN_REGISTATION_NUMBER, _("Registration Number"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerLeft->Add( m_button4, 0, wxALL|wxEXPAND, 5 );

	m_button5 = new wxButton( m_panelLeft, wxID_BTN_THERAPY, _("Therapy"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerLeft->Add( m_button5, 0, wxALL|wxEXPAND, 5 );

	m_button6 = new wxButton( m_panelLeft, wxID_BTN_FULL_TEXT, _("Full Text"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerLeft->Add( m_button6, 0, wxALL|wxEXPAND, 5 );

	myTableView = new TableViewDelegate(this, false);
	bSizerLeft->Add( myTableView, 2, wxEXPAND, 0 );


	m_panelLeft->SetSizer( bSizerLeft );
	m_panelLeft->Layout();
	bSizerLeft->Fit( m_panelLeft );
	m_panelRight = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );

	myTabView = new wxSimplebook( m_panelRight, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel9 = new wxPanel( myTabView, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );

	myWebView = new wxHtmlWindow( m_panel9, wxID_FI_WEBVIEW, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO );
	bSizer5->Add( myWebView, 1, wxALL|wxEXPAND, 5 );


	m_panel9->SetSizer( bSizer5 );
	m_panel9->Layout();
	bSizer5->Fit( m_panel9 );
	myTabView->AddPage( m_panel9, _("a page"), false );
	m_panel10 = new wxPanel( myTabView, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );


	m_panel10->SetSizer( bSizer6 );
	m_panel10->Layout();
	bSizer6->Fit( m_panel10 );
	myTabView->AddPage( m_panel10, _("a page"), false );
	m_panel11 = new wxPanel( myTabView, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer11_patient;
	bSizer11_patient = new wxBoxSizer( wxVERTICAL );

	m_button7 = new wxButton( m_panel11, wxID_PATIENT_SEARCH, _("Patient search"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11_patient->Add( m_button7, 0, wxALL, 5 );

	m_textCtrl1 = new wxTextCtrl( m_panel11, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,102 ), wxTE_MULTILINE|wxTE_NO_VSCROLL );
	bSizer11_patient->Add( m_textCtrl1, 0, wxALL|wxEXPAND, 5 );

	myPlaceDateField = new wxStaticText( m_panel11, wxID_ANY, _("Place and Date"), wxDefaultPosition, wxDefaultSize, 0 );
	myPlaceDateField->Wrap( -1 );
	bSizer11_patient->Add( myPlaceDateField, 0, wxALL, 5 );


	bSizer10->Add( bSizer11_patient, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer12_doctor;
	bSizer12_doctor = new wxBoxSizer( wxVERTICAL );

	m_textCtrl2 = new wxTextCtrl( m_panel11, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,91 ), wxTE_MULTILINE|wxTE_NO_VSCROLL );
	bSizer12_doctor->Add( m_textCtrl2, 0, wxALL|wxEXPAND, 5 );

	m_bpButton1 = new wxBitmapButton( m_panel11, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 150,57 ), wxBU_AUTODRAW|0 );
	bSizer12_doctor->Add( m_bpButton1, 0, wxALL, 5 );


	bSizer10->Add( bSizer12_doctor, 1, wxEXPAND, 5 );


	bSizer7->Add( bSizer10, 1, wxEXPAND, 5 );

	myPrescriptionsTable = new wxDataViewCtrl( m_panel11, wxID_ANY, wxDefaultPosition, wxSize( -1,340 ), 0 );
	bSizer7->Add( myPrescriptionsTable, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxHORIZONTAL );

	m_button8 = new wxButton( m_panel11, wxID_ANY, _("New Prescription"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_button8, 0, wxALL, 5 );

	m_button9 = new wxButton( m_panel11, wxID_ANY, _("Check interactions"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_button9, 0, wxALL, 5 );


	bSizer13->Add( 0, 0, 1, wxEXPAND, 5 );

	m_button10 = new wxButton( m_panel11, wxID_ANY, _("Save"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_button10, 0, wxALL, 5 );

	m_button11 = new wxButton( m_panel11, wxID_ANY, _("Send"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_button11, 0, wxALL, 5 );


	bSizer7->Add( bSizer13, 1, wxEXPAND, 5 );


	m_panel11->SetSizer( bSizer7 );
	m_panel11->Layout();
	bSizer7->Fit( m_panel11 );
	myTabView->AddPage( m_panel11, _("a page"), false );

	bSizer4->Add( myTabView, 1, wxEXPAND | wxALL, 0 );

	mySectionTitles = new wxDataViewListCtrl( m_panelRight, wxID_SECTION_TITLES, wxDefaultPosition, wxSize( 200,-1 ), wxDV_NO_HEADER );
	bSizer4->Add( mySectionTitles, 0, wxALL|wxEXPAND, 5 );


	m_panelRight->SetSizer( bSizer4 );
	m_panelRight->Layout();
	bSizer4->Fit( m_panelRight );
	m_splitter1->SplitVertically( m_panelLeft, m_panelRight, 0 );
	bSizer1->Add( m_splitter1, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();
	m_tbMain = this->CreateToolBar( wxTB_HORIZONTAL|wxTB_TEXT, wxID_ANY );
	m_tool1 = m_tbMain->AddTool( wxID_TB_COMPENDIUM, _("Compendium"), wxBitmap( compendium_xpm ), wxNullBitmap, wxITEM_NORMAL, _("AIPS Database"), wxEmptyString, NULL );

	m_tool2 = m_tbMain->AddTool( wxID_TB_FAVORITES, _("Favorites"), wxBitmap( favorites_xpm ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_tool3 = m_tbMain->AddTool( wxID_TB_INTERACTIONS, _("Interactions"), wxBitmap( interactions_xpm ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_tool4 = m_tbMain->AddTool( wxID_TB_PRESCRIPTION, _("Prescription"), wxBitmap( prescription_xpm ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_tbMain->AddSeparator();

	m_tool5 = m_tbMain->AddTool( wxID_PRINT, _("Print"), wxBitmap( print_xpm ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_toolAbout = m_tbMain->AddTool( wxID_ABOUT, _("AmiKo Desitin"), wxBitmap( AmiKo_xpm ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_tbMain->Realize();

	m_menubar1 = new wxMenuBar( 0 );
	m_menuFile = new wxMenu();
	m_menu11 = new wxMenu();
	wxMenuItem* m_menu11Item = new wxMenuItem( m_menuFile, wxID_ANY, _("Prescription Info"), wxEmptyString, wxITEM_NORMAL, m_menu11 );
	wxMenuItem* m_menuItem1;
	m_menuItem1 = new wxMenuItem( m_menu11, wxID_ANY, wxString( _("Find...") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu11->Append( m_menuItem1 );

	wxMenuItem* m_menuItem2;
	m_menuItem2 = new wxMenuItem( m_menu11, wxID_ANY, wxString( _("Find Next...") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu11->Append( m_menuItem2 );

	wxMenuItem* m_menuItem3;
	m_menuItem3 = new wxMenuItem( m_menu11, wxID_ANY, wxString( _("Find Previous...") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu11->Append( m_menuItem3 );

	m_menuFile->Append( m_menu11Item );

	m_menuFile->AppendSeparator();

	wxMenuItem* m_menuItem4;
	m_menuItem4 = new wxMenuItem( m_menuFile, wxID_ANY, wxString( _("Print technical information...") ) , wxEmptyString, wxITEM_NORMAL );
	m_menuFile->Append( m_menuItem4 );

	wxMenuItem* m_menuItem5;
	m_menuItem5 = new wxMenuItem( m_menuFile, wxID_ANY, wxString( _("Print prescription...") ) , wxEmptyString, wxITEM_NORMAL );
	m_menuFile->Append( m_menuItem5 );
	m_menuItem5->Enable( false );

	wxMenuItem* m_menuItem6;
	m_menuItem6 = new wxMenuItem( m_menuFile, wxID_ANY, wxString( _("Print search result...") ) , wxEmptyString, wxITEM_NORMAL );
	m_menuFile->Append( m_menuItem6 );

	m_menuFile->AppendSeparator();

	wxMenuItem* m_miUpdateDB;
	m_miUpdateDB = new wxMenuItem( m_menuFile, wxID_UPDATE_DB, wxString( _("Update via Internet") ) , wxEmptyString, wxITEM_NORMAL );
	m_menuFile->Append( m_miUpdateDB );

	wxMenuItem* m_miLoadDB;
	m_miLoadDB = new wxMenuItem( m_menuFile, wxID_LOAD_DB, wxString( _("Update from file") ) , wxEmptyString, wxITEM_NORMAL );
	m_menuFile->Append( m_miLoadDB );

	m_menuFile->AppendSeparator();

	wxMenuItem* m_menuItem9;
	m_menuItem9 = new wxMenuItem( m_menuFile, wxID_ANY, wxString( _("Word Analysis") ) , wxEmptyString, wxITEM_NORMAL );
	m_menuFile->Append( m_menuItem9 );

	m_menubar1->Append( m_menuFile, _("File") );

	m_menuEdit = new wxMenu();
	m_menubar1->Append( m_menuEdit, _("Edit") );

	m_menuPrescr = new wxMenu();
	wxMenuItem* m_menuItem10;
	m_menuItem10 = new wxMenuItem( m_menuPrescr, wxID_ANY, wxString( _("Load Prescription...") ) , wxEmptyString, wxITEM_NORMAL );
	m_menuPrescr->Append( m_menuItem10 );

	wxMenuItem* m_menuItem11;
	m_menuItem11 = new wxMenuItem( m_menuPrescr, wxID_ANY, wxString( _("Save Prescription...") ) , wxEmptyString, wxITEM_NORMAL );
	m_menuPrescr->Append( m_menuItem11 );

	m_menubar1->Append( m_menuPrescr, _("Prescription") );

	m_menuContacts = new wxMenu();
	wxMenuItem* m_menuItem12;
	m_menuItem12 = new wxMenuItem( m_menuContacts, wxID_PATIENT_SEARCH, wxString( _("Patient management...") ) , wxEmptyString, wxITEM_NORMAL );
	m_menuContacts->Append( m_menuItem12 );

	wxMenuItem* m_menuItem13;
	m_menuItem13 = new wxMenuItem( m_menuContacts, wxID_SET_OPERATOR_ID, wxString( _("Doctor signature") ) , wxEmptyString, wxITEM_NORMAL );
	m_menuContacts->Append( m_menuItem13 );

	m_menubar1->Append( m_menuContacts, _("Contacts") );

	m_menuWindow = new wxMenu();
	m_menubar1->Append( m_menuWindow, _("Window") );

	m_menuHelp = new wxMenu();
	wxMenuItem* m_menuItem14;
	m_menuItem14 = new wxMenuItem( m_menuHelp, wxID_ANY, wxString( _("Feedback") ) , wxEmptyString, wxITEM_NORMAL );
	m_menuHelp->Append( m_menuItem14 );

	wxMenuItem* m_menuItem15;
	m_menuItem15 = new wxMenuItem( m_menuHelp, wxID_ANY, wxString( _("Share") ) , wxEmptyString, wxITEM_NORMAL );
	m_menuHelp->Append( m_menuItem15 );

	wxMenuItem* m_menuItem16;
	m_menuItem16 = new wxMenuItem( m_menuHelp, wxID_ANY, wxString( _("Rate") ) , wxEmptyString, wxITEM_NORMAL );
	m_menuHelp->Append( m_menuItem16 );

	m_menuHelp->AppendSeparator();

	wxMenuItem* m_menuItem17;
	m_menuItem17 = new wxMenuItem( m_menuHelp, wxID_ANY, wxString( _("Report") ) , wxEmptyString, wxITEM_NORMAL );
	m_menuHelp->Append( m_menuItem17 );

	m_menubar1->Append( m_menuHelp, _("Help") );

	this->SetMenuBar( m_menubar1 );


	this->Centre( wxBOTH );
}

MainWindowBase::~MainWindowBase()
{
}

PatientSheetBase::PatientSheetBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );

	m_splitter2 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter2->Connect( wxEVT_IDLE, wxIdleEventHandler( PatientSheetBase::m_splitter2OnIdle ), NULL, this );

	m_panel6Left = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText2 = new wxStaticText( m_panel6Left, wxID_ANY, _("Patient Management"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer15->Add( m_staticText2, 0, wxALL|wxEXPAND, 5 );


	bSizer15->Add( 0, 0, 1, wxEXPAND, 5 );

	m_staticText3 = new wxStaticText( m_panel6Left, wxID_ANY, _("* Required fields"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer15->Add( m_staticText3, 0, wxALL, 5 );


	bSizer12->Add( bSizer15, 1, wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText4 = new wxStaticText( m_panel6Left, wxID_ANY, _("Surname*"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer1->Add( m_staticText4, 0, wxALL, 5 );

	m_textCtrl3 = new wxTextCtrl( m_panel6Left, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrl3, 0, wxALL|wxEXPAND, 5 );

	m_staticText5 = new wxStaticText( m_panel6Left, wxID_ANY, _("Name*"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer1->Add( m_staticText5, 0, wxALL, 5 );

	m_textCtrl4 = new wxTextCtrl( m_panel6Left, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrl4, 0, wxALL|wxEXPAND, 5 );

	m_staticText6 = new wxStaticText( m_panel6Left, wxID_ANY, _("Street*"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer1->Add( m_staticText6, 0, wxALL, 5 );

	m_textCtrl5 = new wxTextCtrl( m_panel6Left, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrl5, 0, wxALL|wxEXPAND, 5 );

	m_staticText9 = new wxStaticText( m_panel6Left, wxID_ANY, _("City*"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	fgSizer1->Add( m_staticText9, 0, wxALL, 5 );

	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxHORIZONTAL );

	m_textCtrl8 = new wxTextCtrl( m_panel6Left, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( m_textCtrl8, 0, wxALL, 5 );

	m_staticText10 = new wxStaticText( m_panel6Left, wxID_ANY, _("ZIP*"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	bSizer16->Add( m_staticText10, 0, wxALL, 5 );

	m_textCtrl9 = new wxTextCtrl( m_panel6Left, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( m_textCtrl9, 0, wxALL, 5 );


	fgSizer1->Add( bSizer16, 1, wxEXPAND, 5 );

	m_staticText11 = new wxStaticText( m_panel6Left, wxID_ANY, _("Country"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	fgSizer1->Add( m_staticText11, 0, wxALL, 5 );

	m_textCtrl10 = new wxTextCtrl( m_panel6Left, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrl10, 0, wxALL, 5 );

	m_staticText12 = new wxStaticText( m_panel6Left, wxID_ANY, _("Birthday*"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	fgSizer1->Add( m_staticText12, 0, wxALL, 5 );

	m_textCtrl11 = new wxTextCtrl( m_panel6Left, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrl11, 0, wxALL, 5 );

	m_staticText23 = new wxStaticText( m_panel6Left, wxID_ANY, _("Sex*"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	fgSizer1->Add( m_staticText23, 0, wxALL, 5 );

	wxString m_radioBox1Choices[] = { _("female"), _("male") };
	int m_radioBox1NChoices = sizeof( m_radioBox1Choices ) / sizeof( wxString );
	m_radioBox1 = new wxRadioBox( m_panel6Left, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_radioBox1NChoices, m_radioBox1Choices, 1, wxRA_SPECIFY_ROWS );
	m_radioBox1->SetSelection( 0 );
	fgSizer1->Add( m_radioBox1, 0, wxALL, 5 );

	m_staticText24 = new wxStaticText( m_panel6Left, wxID_ANY, _("Weight (kg)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText24->Wrap( -1 );
	fgSizer1->Add( m_staticText24, 0, wxALL, 5 );

	m_textCtrl20 = new wxTextCtrl( m_panel6Left, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrl20, 0, wxALL, 5 );

	m_staticText25 = new wxStaticText( m_panel6Left, wxID_ANY, _("Height (cm)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText25->Wrap( -1 );
	fgSizer1->Add( m_staticText25, 0, wxALL, 5 );

	m_textCtrl21 = new wxTextCtrl( m_panel6Left, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrl21, 0, wxALL, 5 );

	m_staticText26 = new wxStaticText( m_panel6Left, wxID_ANY, _("Phone"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText26->Wrap( -1 );
	fgSizer1->Add( m_staticText26, 0, wxALL, 5 );

	m_textCtrl22 = new wxTextCtrl( m_panel6Left, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrl22, 0, wxALL, 5 );

	m_staticText27 = new wxStaticText( m_panel6Left, wxID_ANY, _("Email"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText27->Wrap( -1 );
	fgSizer1->Add( m_staticText27, 0, wxALL, 5 );

	m_textCtrl23 = new wxTextCtrl( m_panel6Left, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrl23, 0, wxALL, 5 );


	bSizer12->Add( fgSizer1, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxHORIZONTAL );

	m_button14 = new wxButton( m_panel6Left, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer18->Add( m_button14, 0, wxALL, 5 );


	bSizer18->Add( 0, 0, 1, wxEXPAND, 5 );

	m_button15 = new wxButton( m_panel6Left, wxID_ANY, _("Save"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer18->Add( m_button15, 0, wxALL, 5 );


	bSizer12->Add( bSizer18, 1, wxEXPAND, 5 );


	m_panel6Left->SetSizer( bSizer12 );
	m_panel6Left->Layout();
	bSizer12->Fit( m_panel6Left );
	m_panel7Right = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );

	m_searchCtrl2 = new wxSearchCtrl( m_panel7Right, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	#ifndef __WXMAC__
	m_searchCtrl2->ShowSearchButton( true );
	#endif
	m_searchCtrl2->ShowCancelButton( false );
	bSizer13->Add( m_searchCtrl2, 0, wxALL|wxEXPAND, 5 );

	m_listCtrl1 = new wxListCtrl( m_panel7Right, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_ICON );
	bSizer13->Add( m_listCtrl1, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxHORIZONTAL );

	m_spinBtn1 = new wxSpinButton( m_panel7Right, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_HORIZONTAL );
	bSizer17->Add( m_spinBtn1, 0, wxALL, 5 );


	bSizer17->Add( 0, 0, 1, wxEXPAND, 5 );

	m_staticText13 = new wxStaticText( m_panel7Right, wxID_ANY, _("Label"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13->Wrap( -1 );
	bSizer17->Add( m_staticText13, 0, wxALL, 5 );


	bSizer17->Add( 0, 0, 1, wxEXPAND, 5 );

	m_bpButton2 = new wxBitmapButton( m_panel7Right, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 24,24 ), wxBU_AUTODRAW|0 );
	bSizer17->Add( m_bpButton2, 0, wxALL, 5 );


	bSizer13->Add( bSizer17, 1, wxEXPAND, 5 );


	m_panel7Right->SetSizer( bSizer13 );
	m_panel7Right->Layout();
	bSizer13->Fit( m_panel7Right );
	m_splitter2->SplitVertically( m_panel6Left, m_panel7Right, 0 );
	bSizer11->Add( m_splitter2, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer11 );
	this->Layout();

	this->Centre( wxBOTH );
}

PatientSheetBase::~PatientSheetBase()
{
}

OperatorIDSheetBase::OperatorIDSheetBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxVERTICAL );

	m_staticText14 = new wxStaticText( this, wxID_ANY, _("Doctor"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText14->Wrap( -1 );
	bSizer19->Add( m_staticText14, 0, wxALL, 5 );

	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText15 = new wxStaticText( this, wxID_ANY, _("Title"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	fgSizer2->Add( m_staticText15, 0, wxALL, 5 );

	m_textCtrl12 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textCtrl12, 0, wxALL, 5 );

	m_staticText16 = new wxStaticText( this, wxID_ANY, _("Surname"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	fgSizer2->Add( m_staticText16, 0, wxALL, 5 );

	m_textCtrl13 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textCtrl13, 0, wxALL|wxEXPAND, 5 );

	m_staticText17 = new wxStaticText( this, wxID_ANY, _("Name"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	fgSizer2->Add( m_staticText17, 0, wxALL, 5 );

	m_textCtrl14 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textCtrl14, 0, wxALL, 5 );

	m_staticText22 = new wxStaticText( this, wxID_ANY, _("Street"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText22->Wrap( -1 );
	fgSizer2->Add( m_staticText22, 0, wxALL, 5 );

	m_textCtrl16 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textCtrl16, 0, wxALL, 5 );

	m_staticText18 = new wxStaticText( this, wxID_ANY, _("City"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18->Wrap( -1 );
	fgSizer2->Add( m_staticText18, 0, wxALL, 5 );

	m_textCtrl17 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textCtrl17, 0, wxALL, 5 );

	m_staticText19 = new wxStaticText( this, wxID_ANY, _("Phone"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText19->Wrap( -1 );
	fgSizer2->Add( m_staticText19, 0, wxALL, 5 );

	m_textCtrl18 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textCtrl18, 0, wxALL, 5 );

	m_staticText20 = new wxStaticText( this, wxID_ANY, _("Email"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText20->Wrap( -1 );
	fgSizer2->Add( m_staticText20, 0, wxALL, 5 );

	m_textCtrl19 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textCtrl19, 0, wxALL|wxEXPAND, 5 );


	bSizer19->Add( fgSizer2, 1, wxEXPAND, 5 );

	m_bitmap1 = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 383,134 ), 0 );
	m_bitmap1->SetBackgroundColour( wxColour( 254, 254, 254 ) );

	bSizer19->Add( m_bitmap1, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxHORIZONTAL );

	m_button16 = new wxButton( this, wxID_ANY, _("Signature from file"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer20->Add( m_button16, 0, wxALL, 5 );


	bSizer20->Add( 0, 0, 1, wxEXPAND, 5 );

	m_button18 = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer20->Add( m_button18, 0, wxALL, 5 );

	m_button17 = new wxButton( this, wxID_ANY, _("Save"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer20->Add( m_button17, 0, wxALL, 5 );


	bSizer19->Add( bSizer20, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer19 );
	this->Layout();

	this->Centre( wxBOTH );
}

OperatorIDSheetBase::~OperatorIDSheetBase()
{
}
