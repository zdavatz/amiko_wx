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
	EVT_TOOL( wxID_TB_COMPENDIUM, MainWindowBase::_wxFB_OnToolbarAction )
	EVT_TOOL( wxID_TB_FAVORITES, MainWindowBase::_wxFB_OnToolbarAction )
	EVT_TOOL( wxID_TB_INTERACTIONS, MainWindowBase::_wxFB_OnToolbarAction )
	EVT_TOOL( wxID_TB_PRESCRIPTION, MainWindowBase::_wxFB_OnToolbarAction )
	EVT_TOOL( wxID_PRINT, MainWindowBase::_wxFB_OnPrintDocument )
	EVT_TOOL( wxID_ABOUT, MainWindowBase::_wxFB_OnShowAboutPanel )
	EVT_MENU( wxID_UPDATE_DB, MainWindowBase::_wxFB_OnUpdateAipsDatabase )
	EVT_MENU( wxID_LOAD_DB, MainWindowBase::_wxFB_OnLoadAipsDatabase )
END_EVENT_TABLE()

MainWindowBase::MainWindowBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );

	bSizerLeft = new wxBoxSizer( wxVERTICAL );

	mySearchField = new wxSearchCtrl( this, wxID_MY_SEARCH_FIELD, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	#ifndef __WXMAC__
	mySearchField->ShowSearchButton( true );
	#endif
	mySearchField->ShowCancelButton( false );
	bSizerLeft->Add( mySearchField, 0, wxALL|wxEXPAND, 5 );

	m_button1 = new wxButton( this, wxID_BTN_PREPARATION, _("Preparation"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerLeft->Add( m_button1, 0, wxALL|wxEXPAND, 5 );

	m_button2 = new wxButton( this, wxID_BTN_REGISTRATION_OWNER, _("Registration Owner"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerLeft->Add( m_button2, 0, wxALL|wxEXPAND, 5 );

	m_button3 = new wxButton( this, wxID_BTN_ACTIVE_SUBSTANCE, _("Active Substance / ATC Code"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerLeft->Add( m_button3, 0, wxALL|wxEXPAND, 5 );

	m_button4 = new wxButton( this, wxID_BTN_REGISTATION_NUMBER, _("Registration Number"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerLeft->Add( m_button4, 0, wxALL|wxEXPAND, 5 );

	m_button5 = new wxButton( this, wxID_BTN_THERAPY, _("Therapy"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerLeft->Add( m_button5, 0, wxALL|wxEXPAND, 5 );

	m_button6 = new wxButton( this, wxID_BTN_FULL_TEXT, _("Full Text"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerLeft->Add( m_button6, 0, wxALL|wxEXPAND, 5 );

	m_hlbox = new TableViewDelegate(this, false);
	bSizerLeft->Add( m_hlbox, 2, wxEXPAND, 0 );


	bSizer1->Add( bSizerLeft, 1, wxEXPAND, 5 );

	fiPanel = new wxHtmlWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO );
	bSizer1->Add( fiPanel, 1, wxALL|wxEXPAND, 5 );

	mySectionTitles = new wxDataViewListCtrl( this, wxID_ANY, wxDefaultPosition, wxSize( 200,-1 ), wxDV_HORIZ_RULES|wxDV_NO_HEADER );
	bSizer1->Add( mySectionTitles, 0, wxALL|wxEXPAND, 5 );


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
	m_menuItem12 = new wxMenuItem( m_menuContacts, wxID_ANY, wxString( _("Patient management...") ) , wxEmptyString, wxITEM_NORMAL );
	m_menuContacts->Append( m_menuItem12 );

	wxMenuItem* m_menuItem13;
	m_menuItem13 = new wxMenuItem( m_menuContacts, wxID_ANY, wxString( _("Doctor signature") ) , wxEmptyString, wxITEM_NORMAL );
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
