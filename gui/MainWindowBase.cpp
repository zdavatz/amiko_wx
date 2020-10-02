///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.9.0 Aug 24 2020)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "SignatureView.hpp"

#include "MainWindowBase.h"

#include "../res/xpm/AmiKo.xpm"
#include "../res/xpm/compendium.xpm"
#include "../res/xpm/export.xpm"
#include "../res/xpm/favorites.xpm"
#include "../res/xpm/interactions.xpm"
#include "../res/xpm/prescription.xpm"
#include "../res/xpm/print.xpm"

///////////////////////////////////////////////////////////////////////////

MainWindowBase::MainWindowBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );

	m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_LIVE_UPDATE );
	m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( MainWindowBase::m_splitter1OnIdle ), NULL, this );

	m_panelLeft = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	bSizerLeft = new wxBoxSizer( wxVERTICAL );

	mySearchField = new wxSearchCtrl( m_panelLeft, wxID_MY_SEARCH_FIELD, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	#ifndef __WXMAC__
	mySearchField->ShowSearchButton( true );
	#endif
	mySearchField->ShowCancelButton( true );
	bSizerLeft->Add( mySearchField, 0, wxALL|wxEXPAND, 5 );

	m_button1 = new wxButton( m_panelLeft, wxID_BTN_PREPARATION, _("Preparation"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerLeft->Add( m_button1, 0, wxALL|wxEXPAND, 5 );

	m_button2 = new wxButton( m_panelLeft, wxID_BTN_REGISTRATION_OWNER, _("Registration Owner"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerLeft->Add( m_button2, 0, wxALL|wxEXPAND, 5 );

	m_button3 = new wxButton( m_panelLeft, wxID_BTN_ACTIVE_SUBSTANCE, _("Active Substance / ATC Code"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerLeft->Add( m_button3, 0, wxALL|wxEXPAND, 5 );

	m_button4 = new wxButton( m_panelLeft, wxID_BTN_REGISTRATION_NUMBER, _("Registration Number"), wxDefaultPosition, wxDefaultSize, 0 );
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

	m_splitter3 = new wxSplitterWindow( m_panelRight, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter3->Connect( wxEVT_IDLE, wxIdleEventHandler( MainWindowBase::m_splitter3OnIdle ), NULL, this );

	m_panel10 = new wxPanel( m_splitter3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxVERTICAL );

	myTabView = new wxSimplebook( m_panel10, wxID_TABVIEW, wxDefaultPosition, wxDefaultSize, 0 );
	panel_komp = new wxPanel( myTabView, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	fiSizer = new wxBoxSizer( wxVERTICAL );

	myTextFinder = new wxPanel( panel_komp, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxHORIZONTAL );

	fiSearchField = new wxSearchCtrl( myTextFinder, wxID_FI_SEARCH_FIELD, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	#ifndef __WXMAC__
	fiSearchField->ShowSearchButton( true );
	#endif
	fiSearchField->ShowCancelButton( true );
	bSizer23->Add( fiSearchField, 1, wxALL|wxEXPAND, 5 );

	m_button20 = new wxButton( myTextFinder, wxID_FI_FIND_PREVIOUS, _("<"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	bSizer23->Add( m_button20, 0, wxALL, 5 );

	fiSearchCount = new wxStaticText( myTextFinder, wxID_ANY, _("count"), wxDefaultPosition, wxDefaultSize, 0 );
	fiSearchCount->Wrap( -1 );
	fiSearchCount->Hide();

	bSizer23->Add( fiSearchCount, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_button21 = new wxButton( myTextFinder, wxID_FI_FIND_NEXT, _(">"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	bSizer23->Add( m_button21, 0, wxALL, 5 );

	m_button22 = new wxButton( myTextFinder, wxID_FI_FIND_DONE, _("Done"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	bSizer23->Add( m_button22, 0, wxALL, 5 );


	myTextFinder->SetSizer( bSizer23 );
	myTextFinder->Layout();
	bSizer23->Fit( myTextFinder );
	fiSizer->Add( myTextFinder, 0, wxEXPAND | wxALL, 5 );

	myWebView = wxWebView::New(panel_komp, wxID_FI_WEBVIEW);
	fiSizer->Add( myWebView, 1, wxALL|wxEXPAND, 5 );


	panel_komp->SetSizer( fiSizer );
	panel_komp->Layout();
	fiSizer->Fit( panel_komp );
	myTabView->AddPage( panel_komp, _("kompendium"), false );
	panel_inter = new wxPanel( myTabView, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );


	panel_inter->SetSizer( bSizer6 );
	panel_inter->Layout();
	bSizer6->Fit( panel_inter );
	myTabView->AddPage( panel_inter, _("interaktionen"), false );
	panel_rezept = new wxPanel( myTabView, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer11_patient;
	bSizer11_patient = new wxBoxSizer( wxVERTICAL );

	m_button7 = new wxButton( panel_rezept, wxID_PATIENT_SEARCH, _("Patient search"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11_patient->Add( m_button7, 0, wxALL, 5 );

	myPatientAddressTextField = new wxTextCtrl( panel_rezept, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,102 ), wxTE_MULTILINE|wxTE_NO_VSCROLL|wxTE_READONLY );
	bSizer11_patient->Add( myPatientAddressTextField, 0, wxALL|wxEXPAND, 5 );

	myPlaceDateField = new wxStaticText( panel_rezept, wxID_ANY, _("Place and Date"), wxDefaultPosition, wxDefaultSize, 0 );
	myPlaceDateField->Wrap( -1 );
	bSizer11_patient->Add( myPlaceDateField, 0, wxALL, 5 );


	bSizer10->Add( bSizer11_patient, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer12_doctor;
	bSizer12_doctor = new wxBoxSizer( wxVERTICAL );

	myOperatorIDTextField = new wxTextCtrl( panel_rezept, wxID_ANY, _("Enter the doctor's address"), wxDefaultPosition, wxSize( -1,91 ), wxTE_MULTILINE|wxTE_NO_VSCROLL|wxTE_READONLY );
	bSizer12_doctor->Add( myOperatorIDTextField, 0, wxALL|wxEXPAND, 5 );

	mySignView = new SignatureView( panel_rezept, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 383,134 ), 0 );
	bSizer12_doctor->Add( mySignView, 0, wxALL, 5 );


	bSizer10->Add( bSizer12_doctor, 1, wxEXPAND, 5 );


	bSizer7->Add( bSizer10, 1, wxEXPAND, 5 );

	myPrescriptionsTableView = new wxTreeCtrl( panel_rezept, wxID_MEDICINE_LIST, wxDefaultPosition, wxSize( -1,340 ), wxTR_EDIT_LABELS|wxTR_FULL_ROW_HIGHLIGHT|wxTR_HIDE_ROOT|wxTR_SINGLE );
	bSizer7->Add( myPrescriptionsTableView, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxHORIZONTAL );

	btnNewPrescription = new wxButton( panel_rezept, wxID_NEW_PRESCRIPTION, _("New Prescription"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( btnNewPrescription, 0, wxALL, 5 );

	btnCheckInter = new wxButton( panel_rezept, wxID_CHECK_INTERACTIONS, _("Check interactions"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( btnCheckInter, 0, wxALL, 5 );


	bSizer13->Add( 0, 0, 1, wxEXPAND, 5 );

	saveButton = new wxButton( panel_rezept, wxID_SAVE_PRESCRIPTION, _("Save"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	saveButton->Enable( false );

	bSizer13->Add( saveButton, 0, wxALL, 5 );

	sendButton = new wxButton( panel_rezept, wxID_SEND_PRESCRIPTION, _("Send"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	sendButton->Enable( false );

	bSizer13->Add( sendButton, 0, wxALL, 5 );


	bSizer7->Add( bSizer13, 1, wxEXPAND, 5 );


	panel_rezept->SetSizer( bSizer7 );
	panel_rezept->Layout();
	bSizer7->Fit( panel_rezept );
	myTabView->AddPage( panel_rezept, _("rezept"), false );

	bSizer24->Add( myTabView, 1, wxEXPAND | wxALL, 0 );


	m_panel10->SetSizer( bSizer24 );
	m_panel10->Layout();
	bSizer24->Fit( m_panel10 );
	m_panel12 = new wxPanel( m_splitter3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer231;
	bSizer231 = new wxBoxSizer( wxVERTICAL );

	btnDelAmk = new wxButton( m_panel12, wxID_DELETE_PRESCRIPTION, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	btnDelAmk->SetToolTip( _("Delete prescription") );

	bSizer231->Add( btnDelAmk, 0, wxALL|wxEXPAND, 5 );

	mySectionTitles = new wxDataViewListCtrl( m_panel12, wxID_SECTION_TITLES, wxDefaultPosition, wxSize( 200,-1 ), wxDV_NO_HEADER );
	bSizer231->Add( mySectionTitles, 1, wxALL|wxEXPAND, 5 );


	m_panel12->SetSizer( bSizer231 );
	m_panel12->Layout();
	bSizer231->Fit( m_panel12 );
	m_splitter3->SplitVertically( m_panel10, m_panel12, 0 );
	bSizer4->Add( m_splitter3, 1, wxEXPAND, 5 );


	m_panelRight->SetSizer( bSizer4 );
	m_panelRight->Layout();
	bSizer4->Fit( m_panelRight );
	m_splitter1->SplitVertically( m_panelLeft, m_panelRight, 0 );
	bSizer1->Add( m_splitter1, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();
	myToolbar = this->CreateToolBar( wxTB_HORIZONTAL|wxTB_TEXT, wxID_ANY );
	m_tool1 = myToolbar->AddTool( wxID_TB_COMPENDIUM, _("Compendium"), wxBitmap( compendium_xpm ), wxNullBitmap, wxITEM_RADIO, _("AIPS Database"), wxEmptyString, NULL );

	m_tool2 = myToolbar->AddTool( wxID_TB_FAVORITES, _("Favorites"), wxBitmap( favorites_xpm ), wxNullBitmap, wxITEM_RADIO, wxEmptyString, wxEmptyString, NULL );

	m_tool3 = myToolbar->AddTool( wxID_TB_INTERACTIONS, _("Interactions"), wxBitmap( interactions_xpm ), wxNullBitmap, wxITEM_RADIO, wxEmptyString, wxEmptyString, NULL );

	m_tool4 = myToolbar->AddTool( wxID_TB_PRESCRIPTION, _("Prescription"), wxBitmap( prescription_xpm ), wxNullBitmap, wxITEM_RADIO, wxEmptyString, wxEmptyString, NULL );

    //myToolbar->AddSeparator();
    myToolbar->AddStretchableSpace();

	m_tool7 = myToolbar->AddTool( wxID_EXPORT_WORDLIST, _("Export"), wxBitmap( export_xpm ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_tool5 = myToolbar->AddTool( wxID_PRINT, _("Print"), wxBitmap( print_xpm ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_toolAbout = myToolbar->AddTool( wxID_ABOUT, _("AmiKo Desitin"), wxBitmap( AmiKo_xpm ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	myToolbar->Realize();

	m_menubar1 = new wxMenuBar( 0 );
	m_menuFile = new wxMenu();
	m_menu11 = new wxMenu();
	wxMenuItem* m_menu11Item = new wxMenuItem( m_menuFile, wxID_ANY, _("Prescription Info"), wxEmptyString, wxITEM_NORMAL, m_menu11 );
	wxMenuItem* m_menuItem1;
	m_menuItem1 = new wxMenuItem( m_menu11, wxID_FI_FIND_SHOW, wxString( _("Find...") ) + wxT('\t') + wxT("CTRL+F"), wxEmptyString, wxITEM_NORMAL );
	m_menu11->Append( m_menuItem1 );

	wxMenuItem* m_menuItem2;
	m_menuItem2 = new wxMenuItem( m_menu11, wxID_FI_FIND_NEXT, wxString( _("Find Next...") ) + wxT('\t') + wxT("CTRL+G"), wxEmptyString, wxITEM_NORMAL );
	m_menu11->Append( m_menuItem2 );

	wxMenuItem* m_menuItem3;
	m_menuItem3 = new wxMenuItem( m_menu11, wxID_FI_FIND_PREVIOUS, wxString( _("Find Previous...") ) + wxT('\t') + wxT("CTRL+SHIFT+G"), wxEmptyString, wxITEM_NORMAL );
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
	m_miUpdateDB = new wxMenuItem( m_menuFile, wxID_UPDATE_DB, wxString( _("Update via Internet") ) + wxT('\t') + wxT("CTRL+A"), wxEmptyString, wxITEM_NORMAL );
	m_menuFile->Append( m_miUpdateDB );

	wxMenuItem* m_miLoadDB;
	m_miLoadDB = new wxMenuItem( m_menuFile, wxID_LOAD_DB, wxString( _("Update from file") ) + wxT('\t') + wxT("CTRL+L"), wxEmptyString, wxITEM_NORMAL );
	m_menuFile->Append( m_miLoadDB );

	m_menuFile->AppendSeparator();

	wxMenuItem* m_menuItem9;
	m_menuItem9 = new wxMenuItem( m_menuFile, wxID_EXPORT_WORDLIST, wxString( _("Word Analysis") ) , wxEmptyString, wxITEM_NORMAL );
	m_menuFile->Append( m_menuItem9 );

	m_menubar1->Append( m_menuFile, _("File") );

	m_menuEdit = new wxMenu();
	m_menubar1->Append( m_menuEdit, _("Edit") );

	m_menuPrescr = new wxMenu();
	wxMenuItem* m_menuItem10;
	m_menuItem10 = new wxMenuItem( m_menuPrescr, wxID_LOAD_PRESCRIPTION, wxString( _("Load Prescription...") ) , wxEmptyString, wxITEM_NORMAL );
	m_menuPrescr->Append( m_menuItem10 );

	wxMenuItem* m_menuItem11;
	m_menuItem11 = new wxMenuItem( m_menuPrescr, wxID_SAVE_PRESCRIPTION, wxString( _("Save Prescription...") ) , wxEmptyString, wxITEM_NORMAL );
	m_menuPrescr->Append( m_menuItem11 );

	m_menubar1->Append( m_menuPrescr, _("Prescription") );

	m_menuContacts = new wxMenu();
	wxMenuItem* m_menuItem12;
	m_menuItem12 = new wxMenuItem( m_menuContacts, wxID_PATIENT_SEARCH, wxString( _("Patient management...") ) + wxT('\t') + wxT("ALT+CTRL+P"), wxEmptyString, wxITEM_NORMAL );
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

	// Connect Events
	this->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( MainWindowBase::OnUpdateUI ) );
	mySearchField->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( MainWindowBase::OnSearchNow ), NULL, this );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnButtonPressed ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnButtonPressed ), NULL, this );
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnButtonPressed ), NULL, this );
	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnButtonPressed ), NULL, this );
	m_button5->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnButtonPressed ), NULL, this );
	m_button6->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnButtonPressed ), NULL, this );
	myTabView->Connect( wxEVT_COMMAND_BOOKCTRL_PAGE_CHANGED, wxBookCtrlEventHandler( MainWindowBase::OnSimplebookPageChanged ), NULL, this );
	myTabView->Connect( wxEVT_COMMAND_BOOKCTRL_PAGE_CHANGING, wxBookCtrlEventHandler( MainWindowBase::OnSimplebookPageChanging ), NULL, this );
	fiSearchField->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( MainWindowBase::OnSearchFiNow ), NULL, this );
	m_button20->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnPerformFindAction ), NULL, this );
	m_button21->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnPerformFindAction ), NULL, this );
	m_button22->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnPerformFindAction ), NULL, this );
	m_button7->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnSearchPatient ), NULL, this );
	myPrescriptionsTableView->Connect( wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler( MainWindowBase::OnTreeBeginLabelEdit ), NULL, this );
	myPrescriptionsTableView->Connect( wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler( MainWindowBase::OnTreeEndLabelEdit ), NULL, this );
	myPrescriptionsTableView->Connect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( MainWindowBase::OnTreeItemMenu ), NULL, this );
	btnNewPrescription->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnNewPrescription ), NULL, this );
	btnCheckInter->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnCheckForInteractions ), NULL, this );
	saveButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnSavePrescription ), NULL, this );
	sendButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnSendPrescription ), NULL, this );
	btnDelAmk->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnDeletePrescription ), NULL, this );
	mySectionTitles->Connect( wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler( MainWindowBase::OnSelectionDidChange ), NULL, this );
	this->Connect( m_tool1->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainWindowBase::OnToolbarAction ) );
	this->Connect( m_tool2->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainWindowBase::OnToolbarAction ) );
	this->Connect( m_tool3->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainWindowBase::OnToolbarAction ) );
	this->Connect( m_tool4->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainWindowBase::OnToolbarAction ) );
	this->Connect( m_tool7->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainWindowBase::OnExportWordListSearchResults ) );
	this->Connect( m_tool5->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainWindowBase::OnPrintDocument ) );
	this->Connect( m_toolAbout->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainWindowBase::OnShowAboutPanel ) );
	m_menu11->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainWindowBase::OnPerformFindAction ), this, m_menuItem1->GetId());
	m_menu11->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainWindowBase::OnPerformFindAction ), this, m_menuItem2->GetId());
	m_menu11->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainWindowBase::OnPerformFindAction ), this, m_menuItem3->GetId());
	m_menuFile->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainWindowBase::OnUpdateAipsDatabase ), this, m_miUpdateDB->GetId());
	m_menuFile->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainWindowBase::OnLoadAipsDatabase ), this, m_miLoadDB->GetId());
	m_menuFile->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainWindowBase::OnExportWordListSearchResults ), this, m_menuItem9->GetId());
	m_menuPrescr->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainWindowBase::OnLoadPrescription ), this, m_menuItem10->GetId());
	m_menuPrescr->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainWindowBase::OnSavePrescription ), this, m_menuItem11->GetId());
	m_menuContacts->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainWindowBase::OnManagePatients ), this, m_menuItem12->GetId());
	m_menuContacts->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainWindowBase::OnSetOperatorIdentity ), this, m_menuItem13->GetId());
}

MainWindowBase::~MainWindowBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( MainWindowBase::OnUpdateUI ) );
	mySearchField->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( MainWindowBase::OnSearchNow ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnButtonPressed ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnButtonPressed ), NULL, this );
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnButtonPressed ), NULL, this );
	m_button4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnButtonPressed ), NULL, this );
	m_button5->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnButtonPressed ), NULL, this );
	m_button6->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnButtonPressed ), NULL, this );
	myTabView->Disconnect( wxEVT_COMMAND_BOOKCTRL_PAGE_CHANGED, wxBookCtrlEventHandler( MainWindowBase::OnSimplebookPageChanged ), NULL, this );
	myTabView->Disconnect( wxEVT_COMMAND_BOOKCTRL_PAGE_CHANGING, wxBookCtrlEventHandler( MainWindowBase::OnSimplebookPageChanging ), NULL, this );
	fiSearchField->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( MainWindowBase::OnSearchFiNow ), NULL, this );
	m_button20->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnPerformFindAction ), NULL, this );
	m_button21->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnPerformFindAction ), NULL, this );
	m_button22->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnPerformFindAction ), NULL, this );
	m_button7->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnSearchPatient ), NULL, this );
	myPrescriptionsTableView->Disconnect( wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler( MainWindowBase::OnTreeBeginLabelEdit ), NULL, this );
	myPrescriptionsTableView->Disconnect( wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler( MainWindowBase::OnTreeEndLabelEdit ), NULL, this );
	myPrescriptionsTableView->Disconnect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( MainWindowBase::OnTreeItemMenu ), NULL, this );
	btnNewPrescription->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnNewPrescription ), NULL, this );
	btnCheckInter->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnCheckForInteractions ), NULL, this );
	saveButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnSavePrescription ), NULL, this );
	sendButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnSendPrescription ), NULL, this );
	btnDelAmk->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainWindowBase::OnDeletePrescription ), NULL, this );
	mySectionTitles->Disconnect( wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler( MainWindowBase::OnSelectionDidChange ), NULL, this );
	this->Disconnect( m_tool1->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainWindowBase::OnToolbarAction ) );
	this->Disconnect( m_tool2->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainWindowBase::OnToolbarAction ) );
	this->Disconnect( m_tool3->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainWindowBase::OnToolbarAction ) );
	this->Disconnect( m_tool4->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainWindowBase::OnToolbarAction ) );
	this->Disconnect( m_tool7->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainWindowBase::OnExportWordListSearchResults ) );
	this->Disconnect( m_tool5->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainWindowBase::OnPrintDocument ) );
	this->Disconnect( m_toolAbout->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainWindowBase::OnShowAboutPanel ) );

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

	mFamilyName = new wxTextCtrl( m_panel6Left, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( mFamilyName, 0, wxALL|wxEXPAND, 5 );

	m_staticText5 = new wxStaticText( m_panel6Left, wxID_ANY, _("Name*"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer1->Add( m_staticText5, 0, wxALL, 5 );

	mGivenName = new wxTextCtrl( m_panel6Left, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( mGivenName, 0, wxALL|wxEXPAND, 5 );

	m_staticText6 = new wxStaticText( m_panel6Left, wxID_ANY, _("Street*"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer1->Add( m_staticText6, 0, wxALL, 5 );

	mPostalAddress = new wxTextCtrl( m_panel6Left, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( mPostalAddress, 0, wxALL|wxEXPAND, 5 );

	m_staticText9 = new wxStaticText( m_panel6Left, wxID_ANY, _("City*"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	fgSizer1->Add( m_staticText9, 0, wxALL, 5 );

	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxHORIZONTAL );

	mCity = new wxTextCtrl( m_panel6Left, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( mCity, 0, wxALL, 5 );

	m_staticText10 = new wxStaticText( m_panel6Left, wxID_ANY, _("ZIP*"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	bSizer16->Add( m_staticText10, 0, wxALL, 5 );

	mZipCode = new wxTextCtrl( m_panel6Left, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( mZipCode, 0, wxALL, 5 );


	fgSizer1->Add( bSizer16, 1, wxEXPAND, 5 );

	m_staticText11 = new wxStaticText( m_panel6Left, wxID_ANY, _("Country"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	fgSizer1->Add( m_staticText11, 0, wxALL, 5 );

	mCountry = new wxTextCtrl( m_panel6Left, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( mCountry, 0, wxALL, 5 );

	m_staticText12 = new wxStaticText( m_panel6Left, wxID_ANY, _("Birthday*"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	fgSizer1->Add( m_staticText12, 0, wxALL, 5 );

	mBirthDate = new wxTextCtrl( m_panel6Left, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( mBirthDate, 0, wxALL, 5 );

	m_staticText23 = new wxStaticText( m_panel6Left, wxID_ANY, _("Sex*"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	fgSizer1->Add( m_staticText23, 0, wxALL, 5 );

	wxString mSexChoices[] = { _("female"), _("male") };
	int mSexNChoices = sizeof( mSexChoices ) / sizeof( wxString );
	mSex = new wxRadioBox( m_panel6Left, wxID_RB_SEX, wxEmptyString, wxDefaultPosition, wxDefaultSize, mSexNChoices, mSexChoices, 1, wxRA_SPECIFY_ROWS );
	mSex->SetSelection( 0 );
	fgSizer1->Add( mSex, 0, wxALL, 5 );

	m_staticText24 = new wxStaticText( m_panel6Left, wxID_ANY, _("Weight (kg)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText24->Wrap( -1 );
	fgSizer1->Add( m_staticText24, 0, wxALL, 5 );

	mWeight_kg = new wxTextCtrl( m_panel6Left, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( mWeight_kg, 0, wxALL, 5 );

	m_staticText25 = new wxStaticText( m_panel6Left, wxID_ANY, _("Height (cm)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText25->Wrap( -1 );
	fgSizer1->Add( m_staticText25, 0, wxALL, 5 );

	mHeight_cm = new wxTextCtrl( m_panel6Left, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( mHeight_cm, 0, wxALL, 5 );

	m_staticText26 = new wxStaticText( m_panel6Left, wxID_ANY, _("Phone"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText26->Wrap( -1 );
	fgSizer1->Add( m_staticText26, 0, wxALL, 5 );

	mPhone = new wxTextCtrl( m_panel6Left, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( mPhone, 0, wxALL, 5 );

	m_staticText27 = new wxStaticText( m_panel6Left, wxID_ANY, _("Email"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText27->Wrap( -1 );
	fgSizer1->Add( m_staticText27, 0, wxALL, 5 );

	mEmail = new wxTextCtrl( m_panel6Left, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( mEmail, 0, wxALL|wxEXPAND, 5 );


	bSizer12->Add( fgSizer1, 1, wxEXPAND, 5 );

	mNotification = new wxStaticText( m_panel6Left, wxID_ANY, _("Save..."), wxDefaultPosition, wxDefaultSize, 0 );
	mNotification->Wrap( -1 );
	bSizer12->Add( mNotification, 0, wxALL, 5 );

	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxHORIZONTAL );

	m_button14 = new wxButton( m_panel6Left, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer18->Add( m_button14, 0, wxALL, 5 );


	bSizer18->Add( 0, 0, 1, wxEXPAND, 5 );

	m_button15 = new wxButton( m_panel6Left, wxID_SAVE_PATIENT, _("Save"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer18->Add( m_button15, 0, wxALL, 5 );


	bSizer12->Add( bSizer18, 1, wxEXPAND, 5 );


	m_panel6Left->SetSizer( bSizer12 );
	m_panel6Left->Layout();
	bSizer12->Fit( m_panel6Left );
	m_panel7Right = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );

	mSearchKey = new wxSearchCtrl( m_panel7Right, wxID_PAT_SEARCH_FIELD, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	#ifndef __WXMAC__
	mSearchKey->ShowSearchButton( true );
	#endif
	mSearchKey->ShowCancelButton( true );
	bSizer13->Add( mSearchKey, 0, wxALL|wxEXPAND, 5 );

	mTableView = new wxListCtrl( m_panel7Right, wxID_PATIENT_LIST, wxDefaultPosition, wxDefaultSize, wxLC_NO_HEADER|wxLC_REPORT|wxLC_SINGLE_SEL );
	bSizer13->Add( mTableView, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxHORIZONTAL );

	m_button22 = new wxButton( m_panel7Right, wxID_ADD_PATIENT, _("+"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	bSizer17->Add( m_button22, 0, wxALL, 5 );

	m_button23 = new wxButton( m_panel7Right, wxID_DELETE_PATIENT, _("-"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	bSizer17->Add( m_button23, 0, wxALL, 5 );

	mNumPatients = new wxStaticText( m_panel7Right, wxID_ANY, _("Label"), wxDefaultPosition, wxDefaultSize, 0 );
	mNumPatients->Wrap( -1 );
	bSizer17->Add( mNumPatients, 0, wxALL, 5 );


	bSizer17->Add( 0, 0, 1, wxEXPAND, 5 );

	m_button21 = new wxButton( m_panel7Right, wxID_ANY, _("👥"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_button21->SetToolTip( _("Toggle between patient DB and system address book") );

	bSizer17->Add( m_button21, 0, wxALL, 5 );


	bSizer13->Add( bSizer17, 0, wxEXPAND, 5 );


	m_panel7Right->SetSizer( bSizer13 );
	m_panel7Right->Layout();
	bSizer13->Fit( m_panel7Right );
	m_splitter2->SplitVertically( m_panel6Left, m_panel7Right, 0 );
	bSizer11->Add( m_splitter2, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer11 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	mSex->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( PatientSheetBase::OnSelectSex ), NULL, this );
	m_button14->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PatientSheetBase::OnCancel ), NULL, this );
	m_button15->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PatientSheetBase::OnSavePatient ), NULL, this );
	mSearchKey->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PatientSheetBase::OnSearchDatabase ), NULL, this );
	mTableView->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( PatientSheetBase::OnSelectPatient ), NULL, this );
	mTableView->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( PatientSheetBase::OnListItemSelected ), NULL, this );
	m_button22->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PatientSheetBase::OnNewPatient ), NULL, this );
	m_button23->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PatientSheetBase::OnDeletePatient ), NULL, this );
	m_button21->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PatientSheetBase::OnShowContacts ), NULL, this );
}

PatientSheetBase::~PatientSheetBase()
{
	// Disconnect Events
	mSex->Disconnect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( PatientSheetBase::OnSelectSex ), NULL, this );
	m_button14->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PatientSheetBase::OnCancel ), NULL, this );
	m_button15->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PatientSheetBase::OnSavePatient ), NULL, this );
	mSearchKey->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PatientSheetBase::OnSearchDatabase ), NULL, this );
	mTableView->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( PatientSheetBase::OnSelectPatient ), NULL, this );
	mTableView->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( PatientSheetBase::OnListItemSelected ), NULL, this );
	m_button22->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PatientSheetBase::OnNewPatient ), NULL, this );
	m_button23->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PatientSheetBase::OnDeletePatient ), NULL, this );
	m_button21->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PatientSheetBase::OnShowContacts ), NULL, this );

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

	mTitle = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( mTitle, 0, wxALL, 5 );

	m_staticText16 = new wxStaticText( this, wxID_ANY, _("Surname"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	fgSizer2->Add( m_staticText16, 0, wxALL, 5 );

	mFamilyName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( mFamilyName, 0, wxALL|wxEXPAND, 5 );

	m_staticText17 = new wxStaticText( this, wxID_ANY, _("Name"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	fgSizer2->Add( m_staticText17, 0, wxALL, 5 );

	mGivenName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( mGivenName, 0, wxALL|wxEXPAND, 5 );

	m_staticText22 = new wxStaticText( this, wxID_ANY, _("Street"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText22->Wrap( -1 );
	fgSizer2->Add( m_staticText22, 0, wxALL, 5 );

	mPostalAddress = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( mPostalAddress, 0, wxALL|wxEXPAND, 5 );

	m_staticText18 = new wxStaticText( this, wxID_ANY, _("City"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18->Wrap( -1 );
	fgSizer2->Add( m_staticText18, 0, wxALL, 5 );

	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxHORIZONTAL );

	mCity = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer21->Add( mCity, 0, wxALL, 5 );

	m_staticText27 = new wxStaticText( this, wxID_ANY, _("ZIP"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText27->Wrap( -1 );
	bSizer21->Add( m_staticText27, 0, wxALL, 5 );

	mZipCode = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer21->Add( mZipCode, 0, wxALL, 5 );


	fgSizer2->Add( bSizer21, 1, wxEXPAND, 5 );

	m_staticText19 = new wxStaticText( this, wxID_ANY, _("Phone"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText19->Wrap( -1 );
	fgSizer2->Add( m_staticText19, 0, wxALL, 5 );

	mPhoneNumber = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( mPhoneNumber, 0, wxALL|wxEXPAND, 5 );

	m_staticText20 = new wxStaticText( this, wxID_ANY, _("Email"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText20->Wrap( -1 );
	fgSizer2->Add( m_staticText20, 0, wxALL, 5 );

	mEmailAddress = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( mEmailAddress, 0, wxALL|wxEXPAND, 5 );


	bSizer19->Add( fgSizer2, 1, wxEXPAND, 5 );

	m_button20 = new wxButton( this, wxID_CLEAR_SIGNATURE, _("x"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	bSizer19->Add( m_button20, 0, wxALIGN_RIGHT|wxALL, 5 );

	mSignView = new SignatureView( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 383,134 ), 0 );
	mSignView->SetBackgroundColour( wxColour( 254, 254, 254 ) );

	bSizer19->Add( mSignView, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxHORIZONTAL );

	m_button16 = new wxButton( this, wxID_LOAD_SIGNATURE, _("Signature from file"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer20->Add( m_button16, 0, wxALL, 5 );


	bSizer20->Add( 0, 0, 1, wxEXPAND, 5 );

	m_button18 = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer20->Add( m_button18, 0, wxALL, 5 );

	m_button17 = new wxButton( this, wxID_SAVE_OPERATOR, _("Save"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer20->Add( m_button17, 0, wxALL, 5 );


	bSizer19->Add( bSizer20, 0, wxEXPAND, 5 );


	this->SetSizer( bSizer19 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_button20->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OperatorIDSheetBase::OnClearSignature ), NULL, this );
	m_button16->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OperatorIDSheetBase::OnLoadSignature ), NULL, this );
	m_button17->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OperatorIDSheetBase::OnSaveOperator ), NULL, this );
}

OperatorIDSheetBase::~OperatorIDSheetBase()
{
	// Disconnect Events
	m_button20->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OperatorIDSheetBase::OnClearSignature ), NULL, this );
	m_button16->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OperatorIDSheetBase::OnLoadSignature ), NULL, this );
	m_button17->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( OperatorIDSheetBase::OnSaveOperator ), NULL, this );

}
