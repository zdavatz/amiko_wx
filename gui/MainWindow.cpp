
#include <chrono>
#include <vector>
#include <iostream>
#include <fstream>
#include <thread>

#include <wx/wx.h>
#include <wx/stdpaths.h>
#include <wx/busyinfo.h>

#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/tglbtn.h>
#include <wx/xml/xml.h>
#include <wx/menu.h>
#include <wx/event.h>
#include "wx/html/htmprint.h"

#include <sqlite3.h>

#include <nlohmann/json.hpp>

#include "MainWindow.h"
#include "PatientSheet.h"

#include "FullTextDBAdapter.hpp"
#include "customURLConnection.hpp"
#include "DBAdapter.hpp"
#include "InteractionsAdapter.hpp"
#include "Utilities.hpp"
#include "TableViewDelegate.hpp"
#include "Medication.hpp"
#include "DataObject.hpp"
#include "PrescriptionsAdapter.hpp"
#include "PatientSheet.h"
#include "Patient.hpp"
#include "OperatorIDSheet.h"
#include "Operator.hpp"
#include "FullTextSearch.hpp"
#include "FullTextEntry.hpp"
#include "DataStore.hpp"
#include "InteractionsHtmlView.hpp"
#include "PrescriptionItem.hpp"
#include "ItemCellView.hpp"
#include "config.h"
#include "SignatureView.hpp"
#include "DefaultsController.hpp"

#include "HealthCard.hpp"
#include "PatientDBAdapter.hpp"

#include "../res/xpm/CoMed.xpm"
#include "../sync/GoogleSyncManager.hpp"

#define CSV_SEPARATOR       wxT(";")

// Alternatively implement its own tabview to show the results
#define CSV_EXPORT_RESTORES_PREVIOUS_STATE

/// 84
// Database types
enum {
    kdbt_Aips=0, kdbt_Hospital=1, kdbt_Favorites=2, kdbt_Interactions=4
};

// Search states
// [[deprecated]] use the button IDs directly
enum {
    kss_Title=wxID_BTN_PREPARATION,
    kss_Author=wxID_BTN_REGISTRATION_OWNER,
    kss_AtcCode=wxID_BTN_ACTIVE_SUBSTANCE,
    kss_RegNr=wxID_BTN_REGISTRATION_NUMBER,
    kss_Therapy=wxID_BTN_THERAPY,
//    kss_WebView=5,
    kss_FullText=wxID_BTN_FULL_TEXT,
};

// Webview
enum {
    kExpertInfoView=0, kFullTextSearchView=1, kInteractionsCartView=2
};


// 106
static int mCurrentSearchState = kss_Title;
static int mCurrentWebView = kExpertInfoView;
static wxString mCurrentSearchKey;

// Class variable
PrescriptionsCart MainWindow::mPrescriptionsCart[];

// Events not processed by MainWindow will, by default, be handled by MainWindowBase
BEGIN_EVENT_TABLE(MainWindow, MainWindowBase)
    EVT_LISTBOX(wxID_ANY, MainWindow::OnLboxSelect)
    EVT_LISTBOX_DCLICK(wxID_ANY, MainWindow::OnLboxDClick)

    EVT_HTML_LINK_CLICKED(wxID_ANY, MainWindow::OnHtmlLinkClicked)
    EVT_HTML_CELL_HOVER(wxID_ANY, MainWindow::OnHtmlCellHover)
    EVT_HTML_CELL_CLICKED(wxID_ANY, MainWindow::OnHtmlCellClicked)

    EVT_WEBVIEW_NAVIGATING(wxID_ANY, MainWindow::OnNavigationRequest)
    EVT_WEBVIEW_TITLE_CHANGED(wxID_ANY, MainWindow::OnTitleChanged)
    EVT_WEBVIEW_LOADED(wxID_ANY, MainWindow::OnDocumentLoaded)

    EVT_DROP_FILES(MainWindow::OnDropFiles)
    EVT_COMMAND(wxID_ANY, SYNC_MANAGER_UPDATED_AMK, MainWindow::OnAmkFilesUpdated)
END_EVENT_TABLE()

MainWindow::MainWindow( wxWindow* parent )
: MainWindowBase( parent )
, mUsedDatabase(kdbt_Aips)
, mSearchInteractions(false)
, mPrescriptionMode(false)
, mSearchInProgress(false)
, mMed(nullptr)
, mPrescriptionAdapter(nullptr)
, mPatientSheet(nullptr)
, mOperatorIDSheet(nullptr)
, m_alpha(0.0F)
, m_delta(0.01F)
, mFullTextSearch(nullptr)
, mFullTextEntry(nullptr)
, mDb(nullptr)
, mFullTextDb(nullptr)
, mInteractions(nullptr)
, m_findCount(wxNOT_FOUND)
, possibleToOverwrite(false)
, modifiedPrescription(false)
, csvMedication(nullptr)
, healthCard(nullptr)
{
#ifndef NDEBUG
    std::cerr << "PROJECT: " << PROJECT_NAME << "\nAPP: " << APP_NAME << std::endl;
    wxASSERT_MSG((wxID_BTN_PREPARATION+6-1) == wxID_BTN_FULL_TEXT,
                 wxT("defines must be consecutive"));
    std::cerr << "UTF8 locale: "<< wxLocaleIsUtf8 << std::endl;
#endif
    if (wxString(APP_NAME) == "CoMed") {
        m_toolAbout->SetLabel("CoMed Desitin");
        myToolbar->SetToolNormalBitmap(wxID_ABOUT, wxBitmap( CoMed_xpm ));
    }

    mySectionTitles->AppendTextColumn( "Sections" );

    myTextFinder->Hide();
    fiSearchFieldBGColor = myTextFinder->GetBackgroundColour();
    fiSizer->Layout();

    SetTitle(APP_NAME + wxString(" Desitin"));
    const wxEventTable *et = GetEventTable();

    //fadeInAndShow(); // Too early here because we are not doing the fade-in (yet)

    // 256
    // Register applications defaults
    // TODO: if necessary 'DBLastUpdate'
    DefaultsController* defaults = DefaultsController::Instance();
    std::clog << "Defaults local path: <" << defaults->GetLocalFileName(APP_NAME) << ">" << std::endl;

    // 267
    // TODO: Start timer to check if database needs to be updated (checks every hour)

    // 275
    // Open AIPS database (SQLite file)
    bool ok = openSQLiteDatabase();
#ifndef NDEBUG
    if (ok)
        std::cerr << "Number of records in AIPS database: "
        << mDb->getNumRecords() << std::endl;
#endif

    // Open full-text database (SQLite file)
    ok = openFullTextDatabase();
#ifndef NDEBUG
    if (ok)
        std::cerr << "Number of records in fulltext database: "
        << mFullTextDb->getNumRecords() << std::endl;
#endif

    // 286
    // Open drug interactions (CSV file)
    ok = openInteractionsCsvFile();
#ifndef NDEBUG
    if (ok)
        std::cerr << "Number of records in interaction file: "
        << mInteractions->getNumInteractions() << std::endl;
#endif

    // Issue #8
    // TBC: it's not in the call stack when running on Linux
    fadeInAndShow();

    // 292
    // Initialize interactions cart
    mInteractionsView = new InteractionsHtmlView;

    // 295
    // TODO: Create bridge between JScript and ObjC

    // 298
    // Initialize full text search
    mFullTextSearch = new FullTextSearch;

    // 301
    // Initialize all three prescription baskets
    for (int i=0; i<NUM_ACTIVE_PRESCRIPTIONS; ++i)
        mPrescriptionsCart[i].setInteractionsAdapter(mInteractions);

    // 306
    mPrescriptionAdapter = new PrescriptionsAdapter;

    // 308
    // Register drag and drop on prescription table view
#if 1
    // wxEVT_DROP_FILES
    // cannot use DragAcceptFiles() and SetDropTarget() together
    DragAcceptFiles(true);
#else
    mySectionTitles->SetDropTarget(wxDropTarget *dropTarget);
#endif
    
#ifdef TEST_MIME_TYPE
    m_mimeDatabase = new wxMimeTypesManager;
    wxString ext("amk");
    ftInfo = new wxFileTypeInfo("application/octet-stream",
                                wxEmptyString,
                                wxEmptyString,
                                "AmiKo Prescription File",
                                ext, wxNullPtr);
    m_mimeDatabase->Associate(*ftInfo);
    {
        std::clog << "MIME info about extension ." << ext << std::endl;

        wxFileType *filetype = m_mimeDatabase->GetFileTypeFromExtension(ext);

        wxString type;
        if (filetype->GetMimeType(&type))
            std::clog << "\tMIME type: " << type << std::endl;

        wxString desc;
        if (filetype->GetDescription(&desc))
            std::clog << "\tDescription: " << desc << std::endl;

        wxIconLocation loc;
        if (filetype->GetIcon(&loc) && loc.IsOk())
            std::clog << "\tIcon: " << loc.GetFileName() << std::endl;

        wxString filename = "filename";
        filename << "." << ext;
        wxFileType::MessageParameters params(filename, type);
        wxString open;
        if (filetype->GetOpenCommand(&open, params))
            std::clog << "\tCommand to open: " << open << std::endl;

        wxArrayString mimeTypes;
        if (filetype->GetMimeTypes(mimeTypes))
            for (auto m : mimeTypes)
                std::clog << "\t\tMIME types: " << m << std::endl;

        // it is your responsibility to delete the returned pointer when you're done with it!
        delete filetype;
    }
#endif

    // 315
    // Initialize webview
    myWebView->SetPage("<html><head></head><body></body></html>", wxString()); // loadHTMLString
    //myWebView->Fit();

    // 321
    // Initialize favorites (articles + full text entries)
    DataStore *favorites = new DataStore;
    loadFavorites(favorites);
    favoriteMedsSet = favorites->favMedsSet;
    favoriteFTEntrySet = favorites->favFTEntrySet;
#ifndef NDEBUG
    std::cerr << "favoriteKeyData size: " << favoriteKeyData.size() << std::endl;
    std::cerr << "favoriteMedsSet size: " << favoriteMedsSet.size() << std::endl;
    std::cerr << "favoriteFTEntrySet size: " << favoriteFTEntrySet.size() << std::endl;
#endif

    // 327
    // Set default database
    //mUsedDatabase = kdbt_Aips; // already done above
    myToolbar->ToggleTool(wxID_TB_COMPENDIUM, true); // setSelectedItemIdentifier("AIPS")

    // 331
    // Set search state
    setSearchState(kss_Title, wxID_BTN_PREPARATION);

    // 381
    healthCard = new HealthCard;
    m_cardTimer.Start(CARD_POLLING_PERIOD);

    wxTreeItemId root = myPrescriptionsTableView->AddRoot("Root"); // Hidden

    myPrescriptionsTableView->ExpandAll();
    myPrescriptionsTableView->SetIndent(5); // default 15

    GoogleSyncManager::Instance()->startBackgroundSync();
    GoogleSyncManager::Instance()->amkUpdatedHandler = this;

#ifndef __APPLE__
    // Issue #36
    m_menuFile->AppendSeparator();
    m_menuFile->Append(wxID_EXIT, wxT("&Quit"));
    Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainWindow::OnQuit));
#endif
}

MainWindow::~MainWindow()
{
    m_cardTimer.Stop();
#ifdef TEST_MIME_TYPE
#if 1 //ndef __APPLE__ // MIME
    wxFileType *filetype = m_mimeDatabase->GetFileTypeFromExtension("amk");
    m_mimeDatabase->Unassociate(filetype);
#endif
#endif
}

#ifndef __APPLE_
void MainWindow::OnQuit( wxCommandEvent& event)
{
    Close(true);
}
#endif

// Purpose: access the search state from TableViewDelegate
// It doesn't need to be a class member because 'mCurrentSearchState'
// is a global variable
bool searchStateFullText() {
    return (mCurrentSearchState == kss_FullText);
}

// 483
void MainWindow::hideTextFinder()
{
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;

#if 0 // TODO:
    // Inform NSTextFinder the text is going to change
    [myTextFinder noteClientStringWillChange];
    // Hide text finder
    [myTextFinder performAction:NSTextFinderActionHideFindInterface];
    // Discard previous data structures
    [myWebView invalidateTextRanges];
#endif
}

// 548
void MainWindow::fadeInAndShow()
{
#ifdef __linux__
    while (m_alpha < 1.0F) {  // Hopefully this delay helps with Issue #8 in Linux
        m_alpha += m_delta;
        // TODO: fade in window with m_alpha
        wxMilliSleep(10);
        wxTheApp->Yield();
    }
#endif

    resetDataInTableView();
}

// 591
bool MainWindow::openInteractionsCsvFile()
{
    if (!mInteractions)
        mInteractions = new InteractionsAdapter;

    return mInteractions->openInteractionsCsvFile( wxString::Format("drug_interactions_csv_%s", UTI::appLanguage()));
}

// 605
bool MainWindow::openSQLiteDatabase()
{
    if (!mDb)
        mDb = new DBAdapter();

    return mDb->openDatabase( wxString::Format("amiko_db_full_idx_%s", UTI::appLanguage()));
}

// 621
bool MainWindow::openFullTextDatabase()
{
    if (!mFullTextDb)
        mFullTextDb = new FullTextDBAdapter();

    return mFullTextDb->openDatabase( wxString::Format("amiko_frequency_%s", UTI::appLanguage()));
}

// 640
/*
 if not visible && exists in patient_db
    update patient text in main window
 else
    launch patient panel
 */
void MainWindow::newHealthCardData(PAT_DICT &dict) //(NSNotification *)notification
{
    // 650
    Patient *incompletePatient = new Patient;
    incompletePatient->importFromDict(dict);
#ifndef NDEBUG
    std::clog << "patient " << incompletePatient->description();
#endif

    PatientDBAdapter *patientDb = PatientDBAdapter::sharedInstance();
    Patient *existingPatient = patientDb->getPatientWithUniqueID(incompletePatient->uniqueId);

#ifndef NDEBUG
    if (existingPatient)
        std::clog << "Existing patient from DB " << existingPatient->description();
#endif
    
    if (!mPatientSheet)
        mPatientSheet = new PatientSheet(this);
    
    if (mPatientSheet->IsVisible())
        return; // handled by PatientSheet notification

    // Patient sheet not visible

    if (existingPatient) // show it in the prescription
    {
        mPatientSheet->setSelectedPatient(existingPatient);

        if (myTabView->GetSelection() != 2) {
            // 667
            // Switch tab view
            myTabView->ChangeSelection(2);
            myToolbar->ToggleTool(wxID_TB_PRESCRIPTION, true); // setSelectedItemIdentifier("Rezept")
        }

        myPatientAddressTextField->SetValue( mPatientSheet->retrievePatientAsString());
        
        // Update prescription history in right most pane
        mPrescriptionMode = true;
        updatePrescriptionHistory();
    }
    else // prepare to define a new patient
    {
        // 682
        wxCommandEvent event(wxEVT_BUTTON, wxID_ADD_PATIENT);
        mPatientSheet->newPatient(event);
        mPatientSheet->setSelectedPatient(incompletePatient);
        mPatientSheet->setAllFields(incompletePatient);
        mPatientSheet->ShowWindowModal();
    }
}

// 690
void MainWindow::prescriptionDoctorChanged() // (NSNotification *)notification
{
    setOperatorID();
}

// 696
void MainWindow::prescriptionPatientChanged() // (NSNotification *)notification
{
    //if ([[notification name] isEqualToString:@"MLPrescriptionPatientChanged"])
    {
        if (!mPatientSheet)
            mPatientSheet = new PatientSheet(this);

        myPatientAddressTextField->SetValue( mPatientSheet->retrievePatientAsString());
        
        // 703
        mPrescriptionsCart[0].makeNewUniqueHash();
        
        modifiedPrescription = true;
        //updateButtons(); __deprecated
    }

    mPrescriptionMode = true;

    // Update prescription history in right-most pane
    updatePrescriptionHistory();

    // Switch tab view
    myTabView->ChangeSelection(2); // selectTabViewItemAtIndex:2;
    // and corresponding toolbar button
    myToolbar->ToggleTool(wxID_TB_PRESCRIPTION, true); // setSelectedItemIdentifier("Rezept")
}

// 720
void MainWindow::prescriptionPatientDeleted() //NSNotification *)notification
{
    //if ([[notification name] isEqualToString:@"MLPrescriptionPatientDeleted"])
    {
        myPatientAddressTextField->SetValue(wxEmptyString);
        setOperatorID();
        mPrescriptionsCart[0].clearCart();
        myPrescriptionsTableView_reloadData(0);
        resetPrescriptionHistory();
    }
}

// 734
void MainWindow::resetPrescriptionHistory()
{
    // TODO: mListOfSectionTitles = [[NSArray alloc] init];
    // TODO: mListOfSectionIds = [[NSArray alloc] init];
    mySectionTitles_reloadData(); // reloadData
}

// 741
void MainWindow::updatePrescriptionHistory()
{
    if (!mPrescriptionMode) {
#ifndef NDEBUG
        std::clog << __FUNCTION__ << " not prescription mode" << std::endl;
#endif
        return;
    }

    // 750
    // Extract section IDs
    if (!mMed)
        return;
    
    // not in amiko-osk
    if (!mPatientSheet)
        mPatientSheet = new PatientSheet(this);

    if (mMed->sectionIds) {
        wxArrayString listOfPrescriptions = mPrescriptionAdapter->listOfPrescriptionURLsForPatient( mPatientSheet->retrievePatient());
        mListOfSectionIds = listOfPrescriptions;  // array of full paths
    }

    // 756
    // Extract section titles
    if (mMed->sectionTitles) {
        Patient *p = mPatientSheet->retrievePatient();
        wxArrayString listOfPrescriptions = mPrescriptionAdapter->listOfPrescriptionsForPatient(p);
        mListOfSectionTitles = listOfPrescriptions; // array of just the basenames
    }

     mySectionTitles_reloadData(); // reloadData
}

// 768
// Notification called when updates have been downloaded
void MainWindow::finishedDownloading()
{
    std::clog << __FUNCTION__ << std::endl;
    bool ok;

    {
        if (mDb)
            mDb->closeDatabase();

        // 775
        // Re-open database
        ok = openSQLiteDatabase();
    }

    {
        if ( mFullTextDb)
            mFullTextDb->closeDatabase();
        
        // 780
        // Re-open database
        ok = openFullTextDatabase();
    }
    
    {
        if (mInteractions)
            mInteractions->closeInteractionsCsvFile();
        
        // 784
        // Re-open interaction database
        ok = openInteractionsCsvFile();
    }
    
    if (!ok)
        std::cerr << __FUNCTION__ << " some problem opening DB files" << std::endl;
    
    // TODO: Reload table
    
    // 791
    // Reset data in table view and get number of rows in table (=searchResults)
    resetDataInTableView();
    
    // TODO: Update
    
    // TODO: Display friendly message

}

// 847
void MainWindow::updateSearchResults()
{
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;

    if (mUsedDatabase == kdbt_Aips) {
        //searchResults =
        searchAnyDatabasesWith(mCurrentSearchKey); // it updates searchResults and searchResultsFT
    }
    else if (mUsedDatabase == kdbt_Favorites) {
        searchResults = retrieveAllFavorites();
        searchResultsFT = retrieveAllFTFavorites();
    }
}

// 858
void MainWindow::resetDataInTableView()
{
	std::cerr << __PRETTY_FUNCTION__ << std::endl;

	// Reset search state
    setSearchState(kss_Title, wxID_BTN_PREPARATION);

    mCurrentSearchKey = wxEmptyString;
    //searchResults =
    searchAnyDatabasesWith(mCurrentSearchKey);  // it updates searchResults and searchResultsFT

    if (searchResults.size()>0) {
        updateTableView();

        myTableView->SetItemCount(searchResults.size()); // reloadData
        if (searchResults.size() > 0)
            myTableView->SetSelection(0); // scrollRectToVisible
        else
            myTableView->SetSelection(wxNOT_FOUND); // Initially no selection
        myTableView->Refresh();
    }
}

// 918
Medication * MainWindow::getShortMediWithId(long mid)
{
    if (mDb)
        return mDb->getShortMediWithId(mid);

    return nullptr;
}

// 925
void MainWindow::tappedOnStar(int row)
{
    FAVORITES_SET::iterator it;

    if (mCurrentSearchState != kss_FullText) {
        wxString medRegnrs = favoriteKeyData[row];

        it = favoriteMedsSet.find(medRegnrs);
        if (it != favoriteMedsSet.end()) {
            favoriteMedsSet.erase(it);
            std::clog << "Row " << row << ", remove medRegnrs from fav: <" << medRegnrs << ">\n";
        }
        else {
            favoriteMedsSet.insert(medRegnrs);
            std::clog << "Row " << row << ", add medRegnrs to fav: <" << medRegnrs << ">\n";
        }
    }
    else {
        wxString hashId = favoriteKeyData[row];

        it = favoriteFTEntrySet.find(hashId);
        if (it != favoriteFTEntrySet.end()) {
            favoriteFTEntrySet.erase(it);
            std::clog << "Row " << row << ", remove hashId from fav: <" << hashId << ">\n";
        }
        else {
            favoriteFTEntrySet.insert(hashId);
            std::clog << "Row " << row << ", add hashId to fav: <" << hashId << ">\n";
        }
    }
    
    saveFavorites();
}

// 1047
void MainWindow::printTechInfo()
{
    wxHtmlEasyPrinting *m_Prn = new wxHtmlEasyPrinting("Printing", this);
    wxString pageSource = myWebView->GetPageSource();
    //bool previewResult = m_Prn->PreviewText( pageSource); // it returns immediately
    m_Prn->PrintText( pageSource);
    wxDELETE(m_Prn);
}

// 1249
void MainWindow::setOperatorID()
{
    if (!mOperatorIDSheet)
        mOperatorIDSheet = new OperatorIDSheet(this);

    wxString operatorIDStr = mOperatorIDSheet->retrieveIDAsString();
    wxString operatorPlace = mOperatorIDSheet->retrieveCity();
    myOperatorIDTextField->SetValue( operatorIDStr);
    if (operatorPlace.IsEmpty())
        myPlaceDateField->SetLabel(UTI::prettyTime());
    else
        myPlaceDateField->SetLabel( wxString::Format("%s, %s", operatorPlace, UTI::prettyTime()));
    
    wxString documentsDirectory = UTI::documentsDirectory();
    wxString filePath = documentsDirectory + wxFILE_SEP_PATH + DOC_SIGNATURE_FILENAME;
    if (wxFileName::Exists(filePath)) {
        wxImage image;
        if (image.LoadFile(filePath, wxBITMAP_TYPE_PNG)) {
            mySignView->SetBitmap( wxBitmap(image));
            mySignView->SetScaleMode(wxStaticBitmap::Scale_AspectFit);
        }
        else
            std::cerr << __FUNCTION__ << " couldn't load " << filePath << std::endl;
    }
}

// in amiko-osx [self.myPrescriptionsTableView rowForView:sender];
int MainWindow::myPrescriptionsTableView_rowForView()
{
    int row = -1;

    wxTreeItemId rootItem = myPrescriptionsTableView->GetRootItem();
    if (!rootItem.IsOk())
        return row;

    wxTreeItemIdValue cookie;
    for (wxTreeItemId medItem = myPrescriptionsTableView->GetFirstChild(rootItem, cookie);
         medItem.IsOk();
         medItem = myPrescriptionsTableView->GetNextChild(rootItem, cookie))
    {
        row++;

        // Stop counting at the first selected item
        if (myPrescriptionsTableView->IsSelected(medItem))
            break;
    }

    return row;
}

// 1281
void MainWindow::removeItemFromPrescription()
{
    int row = myPrescriptionsTableView_rowForView();
    if (row == -1)
        return;
    
#ifndef NDEBUG
    std::cerr << __FUNCTION__ << " Removing item " << row << " from prescription" << std::endl;
#endif

    // Get item with index
    const int cartNo = 0;
    PrescriptionItem *item = mPrescriptionsCart[ cartNo].getItemAtIndex(row);
    if (item != nullptr) {
        mPrescriptionsCart[ cartNo].removeItemFromCart(item);

        // If prescription cart is not empty, generate new hash
        if (mPrescriptionsCart[ cartNo].cart.size() > 0)
            mPrescriptionsCart[ cartNo].makeNewUniqueHash();

        myPrescriptionsTableView_reloadData( cartNo);
        // updateButtons(); __deprecated
    }
}

// 1558
// Handler for EVT_WEBVIEW_NAVIGATING
// see amiko-osx clickedTableView
/** From wxWidgets sources webview.cpp:778
  * Callback invoked when there is a request to load a new page (for instance when the user clicks a link)
  */
void MainWindow::OnNavigationRequest(wxWebViewEvent& evt)
{
#if 0 //ndef NDEBUG
    std::clog << __PRETTY_FUNCTION__
    << " Navigation request to " << evt.GetURL() // https://__bridge_loaded__/
    << ", (target=" << evt.GetTarget() << ")"   // <!--frame1-->
    << std::endl;

    std::clog << "source: " << myWebView->GetPageSource() << std::endl;
    std::clog << "text: " << myWebView->GetPageText() << std::endl;

    if (evt.GetNavigationAction() == wxWEBVIEW_NAV_ACTION_USER)
        std::clog << " (user)" << std::endl;
#endif

#if 0
    // If we don't want to handle navigation then veto the event and navigation
    // will not take place.
    evt.Veto();
#endif
}

// 1573
// 'filename' contains the full path
void MainWindow::loadPrescription_andRefreshHistory(wxString filename, bool refresh)
{
    wxString hash = mPrescriptionAdapter->loadPrescriptionFromFile(filename);
    if (hash.IsEmpty())
        std::cerr << __FUNCTION__ << __LINE__ << " Empty prescription hash\n";

    mPrescriptionsCart[0].cart = mPrescriptionAdapter->cart;
    mPrescriptionsCart[0].uniqueHash = hash;

    // Set patient found in prescription
    Patient *p = mPrescriptionAdapter->patient;
    if (p) {
        wxString patientHash = p->uniqueId;

        if (!mPatientSheet)
            mPatientSheet = new PatientSheet(this);

        if (!mPatientSheet->patientExistsWithID(patientHash))
            mPatientSheet->addPatient(p);            // Import patient...

        myPatientAddressTextField->SetValue( p->asString());
        mPatientSheet->setSelectedPatient(p);
    }
    
    // 1601
    // Update views
    updatePrescriptionsView();
    if (refresh)
        updatePrescriptionHistory();
    
    // Set operator / doctor found in prescription
    /*
    Operator *o = mPrescriptionAdapter->doctor;
    if (o != nullptr) {
        myOperatorIDTextField.stringValue = o->retrieveOperatorAsString();
    }
    */

    possibleToOverwrite = true;
    modifiedPrescription = false;
    // updateButtons(); __deprecated
}

// 1627
/*
 Save button
 A) not possible to overwrite: no alert panel, just save
 
 B) possible to overwrite: 3 buttons [Cancel], [Save], [Overwrite]
    1) Overwrite —> new hash
    3) New file —> new hash
 */
void MainWindow::savePrescription()
{
    if (!mPatientSheet)
        mPatientSheet = new PatientSheet(this);
    
    mPrescriptionAdapter->cart = mPrescriptionsCart[0].cart;
    
    // 1634
    storeAllPrescriptionComments();
    Patient *patient = mPatientSheet->retrievePatient();
    
    if (mPrescriptionsCart[0].cart.size() < 1) {
        // TODO: maybe the save button should be disabled to prevent coming here
#ifndef NDEBUG
        std::cerr << __FUNCTION__ << " cart is empty" << std::endl;
#endif
        return;
    }

    // 1645
    if (!possibleToOverwrite) {
        mPrescriptionAdapter->savePrescriptionForPatient_withUniqueHash_andOverwrite( patient, mPrescriptionsCart[0].uniqueHash, false);
        possibleToOverwrite = true;
        modifiedPrescription = false;
        //updateButtons(); __deprecated
        updatePrescriptionHistory();

#if 1
        std::clog << __PRETTY_FUNCTION__ << " Line:" << __LINE__ << " TODO" << std::endl;
#else
        #if 1 //def DYNAMIC_AMK_SELECTION
        // 1655
        // Select the topmost entry
        mySectionTitles->SelectRow(0);
        #endif
#endif
        return;
    }

    // 1633
    wxMessageDialog alert(this,
                          _("Do you really want to overwrite the existing prescription ? (NO to generate a new one)"),
                          _("Overwrite prescription?"),
                          wxNO_DEFAULT | wxYES_NO | wxCANCEL | wxICON_WARNING);
    // TODO: [alert addButtonWithTitle:NSLocalizedString(@"New prescription", nil)]; // NO button

    int returnCode = alert.ShowModal();
    
    // 1684
    // Create a new hash for both "overwrite" and "new file"
    if (returnCode != wxID_CANCEL)
        mPrescriptionsCart[0].makeNewUniqueHash();  // Issue #9

    wxURL url;

    switch ( returnCode )
    {
        case wxID_YES: // Overwrite
            url = mPrescriptionAdapter->savePrescriptionForPatient_withUniqueHash_andOverwrite( patient, mPrescriptionsCart[0].uniqueHash, true);
            modifiedPrescription = false;
            //updateButtons(); __deprecated
            break;

        case wxID_NO: // NewFile
            url = mPrescriptionAdapter->savePrescriptionForPatient_withUniqueHash_andOverwrite( patient, mPrescriptionsCart[0].uniqueHash, false);
            possibleToOverwrite = true;
            modifiedPrescription = false;
            //updateButtons(); __deprecated
            break;

        default:
            break;
    }

    updatePrescriptionHistory();

#if 1
    std::clog << __PRETTY_FUNCTION__ << " Line:" << __LINE__ << " TODO" << std::endl;
#else
        #ifdef DYNAMIC_AMK_SELECTION
        // Select the topmost entry
        NSIndexSet *indexSet = [NSIndexSet indexSetWithIndex:0];
        [mySectionTitles selectRowIndexes:indexSet byExtendingSelection:NO];
        #endif
    }];
#endif
        
}

// 1749
void MainWindow::stopProgressIndicator()
{
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
}

// 1755
// Switch app state
void MainWindow::switchTabs(int item)
{
    switch (item) {
        case wxID_TB_COMPENDIUM:
            //std::clog << "AIPS Database" << std::endl;
            mUsedDatabase = kdbt_Aips;
            mSearchInteractions = false;
            mPrescriptionMode = false;
            
            // 1783
            if (mCurrentSearchState != kss_FullText)
            {
                mCurrentSearchState = kss_Title;
                mCurrentSearchKey = wxEmptyString;
            }
            
            // 1787
            searchAnyDatabasesWith(mCurrentSearchKey); // it updates searchResults and searchResultsFT
            
            // /1790
            // Update tableview
            updateTableView();
            
            // 1791
            myTableView->SetItemCount(searchResults.size()); // reloadData
            myTableView->SetSelection(wxNOT_FOUND); // Initially no selection
            myTableView->Refresh();

            // Switch tab view
            updateExpertInfoView(wxEmptyString);
            // 1800
            myTabView->ChangeSelection(0);
            btnDelAmk->Hide();
            break;

        case wxID_TB_FAVORITES:
            std::cerr << __FUNCTION__ << " line " << __LINE__ << " Favorites" << std::endl;
            mUsedDatabase = kdbt_Favorites;
            mSearchInteractions = false;
            mPrescriptionMode = false;
            
            // 1826
            searchResults = retrieveAllFavorites();
            searchResultsFT = retrieveAllFTFavorites();
            
            // 1828
            // Update tableview
            updateTableView();

            // 1831
            myTableView->SetItemCount(searchResults.size()); // reloadData
            myTableView->SetSelection(wxNOT_FOUND); // Initially no selection
            myTableView->Refresh();
            
#ifndef WITH_JS_BRIDGE
            {
            // Workaround new to Amiko-wx (TBC)
            // Purpose: define mMed and prevent crash in updateExpertInfoView()
            // Code taken from createJSBridge()
            wxString ean = wxEmptyString; // TODO: msg[2];
            mMed = mDb->getMediWithRegnr(ean);
            }
#endif

            // Switch tab view
            updateExpertInfoView(wxEmptyString);
            // 1840
            myTabView->ChangeSelection(0);
            btnDelAmk->Hide();
            break;

        case wxID_TB_INTERACTIONS:
            //std::clog << "Interactions" << std::endl;
            // 1847
            mUsedDatabase = kdbt_Aips;
            mSearchInteractions = true;
            mPrescriptionMode = false;
            
            // 1850
            stopProgressIndicator();
            setSearchState(kss_Title, wxID_BTN_PREPARATION);
            pushToMedBasket(mMed);  // TODO : check amiko-osx issue #85
            updateInteractionsView();
            
            // 1854
            // Switch tab view
            myTabView->ChangeSelection(0);
            btnDelAmk->Hide();
            break;

        case wxID_TB_PRESCRIPTION:
            //std::clog << "Rezept" << std::endl;
            mUsedDatabase = kdbt_Aips;
            mSearchInteractions = false;
            mPrescriptionMode = true;
            // TODO:
            stopProgressIndicator();
            updatePrescriptionsView();
            updatePrescriptionHistory();

            // 1868
            // FIXME: calling SetSelection() instead of ChangeSelection()
            // is supposed to generate page changing events
#if 1
            myTabView->ChangeSelection(2);
            setOperatorID();
            myPrescriptionsTableView_reloadData(0);
            // TODO: myPrescriptionsPrintTV->reloadData();
            //updateButtons(); __deprecated
#else
            myTabView->SetSelection(2);
#endif
            btnDelAmk->Show();
            break;

#if 0 // TODO
        case wxID_EXPORT:
            break;

        case wxID_TB_AMIKO:
            break;
#endif
        default:
            break;
    }

    Fit();
}

// 1897
MEDICATION_RESULTS MainWindow::retrieveAllFavorites()
{
    MEDICATION_RESULTS medList;

    // 1905
    if (mCurrentSearchState != kss_FullText) {
        if (mDb) {
            for (auto regnrs : favoriteMedsSet) {
                auto med = mDb->searchRegNr(regnrs);
                if (med.size() > 0)
                    medList.push_back(med[0]); // Use only the first one
            }
        }
    }

    return medList;
}

FULLTEXT_RESULTS MainWindow::retrieveAllFTFavorites()
{
    FULLTEXT_RESULTS ftList;

    // 1915
    if (mCurrentSearchState == kss_FullText) {
        if (mFullTextDb) {
            for (auto hashId : favoriteFTEntrySet) {
                auto entry = mFullTextDb->searchHash(hashId);
                if (entry)
                    ftList.push_back(entry);
            }
        }
    }

    return ftList;
}

// In Amiko-osx it's "~/Library/Preferences/"
#define FAVORITES_DIR       UTI::documentsDirectory()

// In Amiko-osx it's "data" for both
#define FAV_MED_FILE        "favorites.json"
#define FAV_FT_ENTRY_FILE   "favorites-full-text.json"

// 1933
void MainWindow::saveFavorites()
{
    wxString path = FAVORITES_DIR;
    
#ifndef NDEBUG
    std::clog << __FUNCTION__ << " to dir " << path
    << "\n\t favorites: " << favoriteMedsSet.size()
    << "\n\t favoritesFT: " << favoriteFTEntrySet.size()
    << std::endl;
#endif

    {
        std::vector<std::string> favorites;
        wxString filePath = path + wxFILE_SEP_PATH + wxString(FAV_MED_FILE);
        for (const auto fav : favoriteMedsSet) {
            favorites.push_back(fav.ToStdString());
        }

        std::ofstream file(filePath);
        nlohmann::json json = favorites;
        file << json;
    }

    {
        std::vector<std::string> favorites;
        wxString filePath = path + wxFILE_SEP_PATH + wxString(FAV_FT_ENTRY_FILE);
        for (const auto fav : favoriteFTEntrySet) {
            favorites.push_back(fav.ToStdString());
        }

        std::ofstream file(filePath);
        nlohmann::json json = favorites;
        file << json;
    }

    GoogleSyncManager::Instance()->requestSync();
}

// 1950
void MainWindow::loadFavorites(DataStore *favorites)
{
    wxString path = FAVORITES_DIR;
    std::clog << __FUNCTION__ << " from dir " << path << std::endl;

    {
        wxString filePath = path + wxFILE_SEP_PATH + wxString(FAV_MED_FILE);
        wxFileName filename = wxFileName(filePath);
        if (filename.FileExists()) {
            std::ifstream file(filePath);
            nlohmann::json json;
            file >> json;

            for (nlohmann::json::iterator it = json.begin(); it != json.end(); ++it) {
                std::string str = *it;
                favorites->favMedsSet.insert(wxString(str));
            }
        }
    }

    {
        wxString filePath = path + wxFILE_SEP_PATH + wxString(FAV_FT_ENTRY_FILE);
        wxFileName filename = wxFileName(filePath);
        if (filename.FileExists()) {
            std::ifstream file(filePath);
            nlohmann::json json;
            file >> json;

            for (nlohmann::json::iterator it = json.begin(); it != json.end(); ++it) {
                std::string str = *it;
                favorites->favFTEntrySet.insert(wxString(str));
            }
        }
    }
}

// 1967
// TODO: deprecate first parameter
void MainWindow::setSearchState(int searchState, int btnId)
{
    // Manage the wxToggleButton manually (radio behaviour)
    for (long i=wxID_BTN_PREPARATION; i<=wxID_BTN_FULL_TEXT; i++) {
        auto tb = static_cast<wxToggleButton *>(FindWindowById(i));
        tb->SetValue(i == btnId);
    }

    switch (searchState)
    {
        case kss_Title:
            mySearchField->SetValue(wxEmptyString);
            mCurrentSearchState = kss_Title;
            mySearchField->SetDescriptiveText(wxString::Format("%s %s", _("Search"), _("Preparation")));
            break;

        case kss_Author:
            mySearchField->SetValue(wxEmptyString);
            mCurrentSearchState = kss_Author;
            mySearchField->SetDescriptiveText(wxString::Format("%s %s", _("Search"), _("Owner")));
            break;

        case kss_AtcCode:
            mySearchField->SetValue(wxEmptyString);
            mCurrentSearchState = kss_AtcCode;
            mySearchField->SetDescriptiveText(wxString::Format("%s %s", _("Search"), _("ATC Code")));
            break;

        case kss_RegNr:
            mySearchField->SetValue(wxEmptyString);
            mCurrentSearchState = kss_RegNr;
            mySearchField->SetDescriptiveText(wxString::Format("%s %s", _("Search"), _("Reg. No")));
            break;

        case kss_Therapy:
            mySearchField->SetValue(wxEmptyString);
            mCurrentSearchState = kss_Therapy;
            mySearchField->SetDescriptiveText(wxString::Format("%s %s", _("Search"), _("Therapy")));
            break;

#if 0
        case kss_WebView:
            hideTextFinder();

            // NOTE: Commented out because we're using SHCWebView now (02.03.2015)
            /*
            mySearchField->SetValue(wxEmptyString);
            mCurrentSearchState = kWebView;
            mySearchField->SetDescriptiveText(wxString::Format("%s %s", _("Search"), _("in Fachinformation"))); // fr: @"Notice Infopro"
            */
            break;
#endif

        case kss_FullText:
            mySearchField->SetValue(wxEmptyString);
            mCurrentSearchState = kss_FullText;
            mySearchField->SetDescriptiveText(wxString::Format("%s %s", _("Search"), _("Full Text")));
            break;
    }

    mCurrentSearchKey = wxEmptyString;
    mCurrentSearchState = searchState;
    std::cerr << __PRETTY_FUNCTION__ << " Ended" << std::endl;
}

// 2029
void MainWindow::searchAnyDatabasesWith(wxString searchQuery)
{
#ifndef NDEBUG
    std::clog << __FUNCTION__ << ", searchQuery <" << searchQuery << ">"  << std::endl;
#endif

    if (mCurrentSearchState == kss_Title)
        searchResults = mDb->searchTitle(searchQuery);  // array of Medication

    else if (mCurrentSearchState == kss_Author)
        searchResults = mDb->searchAuthor(searchQuery);

    else if (mCurrentSearchState == kss_AtcCode)
        searchResults = mDb->searchATCCode(searchQuery);

    else if (mCurrentSearchState == kss_RegNr)
        searchResults = mDb->searchRegNr(searchQuery);

    else if (mCurrentSearchState == kss_Therapy)
        searchResults = mDb->searchApplication(searchQuery);

    else if (mCurrentSearchState == kss_FullText) {
        // 2048
        if (searchQuery.length() > 2)
            searchResultsFT = mFullTextDb->searchKeyword(searchQuery); // array of FullTextEntry
    }

    mCurrentSearchKey = searchQuery;
    
#ifndef NDEBUG
    int resultCount;
    if (mCurrentSearchState == kss_FullText)
        resultCount = searchResultsFT.size();
    else
        resultCount = searchResults.size();

    std::clog << resultCount << " results" << std::endl;
#endif

    return; // searchRes;
}

// 2064
void MainWindow::addTitle_andPackInfo_andMedId(wxString title, wxString packinfo, long medId)
{
	DataObject *m = new DataObject;
    if (title.size() > 0)
        m->title = title;
    else
        m->title = _("Not specified");

    if (packinfo.size() > 0) {
        if (!mSearchInteractions)
            m->subTitle = packinfo;
        else {
            // 2079
            // We pass 'atccode' instead, which needs to be unpacked
            wxArrayString m_atc = wxSplit(packinfo, ';');
            wxString m_atccode_str;
            wxString m_atcclass_str;
            if (m_atc.size() > 1) {
                if (m_atc[0].length() > 0)
                    m_atccode_str = m_atc[0];

                if (m_atc[1].length() > 0)
                    m_atcclass_str = m_atc[1];
            }

            if (m_atccode_str.length() == 0)
                m_atccode_str = _("Not specified");

            if (m_atcclass_str.length() == 0)
                m_atcclass_str = _("Not specified");

            m->subTitle = wxString::Format("%s - %s", m_atccode_str, m_atcclass_str);
        }
    }
    else
        m->subTitle = _("Not specified");

    m->medId = medId;

    doArray.push_back(m); // __deprecated
    myTableView->searchRes.push_back(m);
}

// 2105
void MainWindow::addTitle_andAuthor_andMedId(wxString title, wxString author, long medId)
{
    DataObject *m = new DataObject;

    if (title.size() > 0)
        m->title = title;
    else
        m->title =  _("Not specified");
    
    if (author.size() > 0) {
        m->subTitle = author;
    }
    else
        m->title = L"Not specified";
    
    m->medId = medId;

    doArray.push_back(m); // __deprecated
    myTableView->searchRes.push_back(m);
}

// 2126
void MainWindow::addTitle_andAtcCode_andAtcClass_andMedId(wxString title, wxString atccode, wxString atcclass, long medId)
{
    DataObject *m = new DataObject;

    if (title.size() > 0)
        m->title = title;
    else
        m->title = _("Not specified");
    
    // 2135
    if (atccode.IsEmpty())
        atccode = _("Not specified");

    if (atcclass.IsEmpty())
        atcclass = _("Not specified");
    
    // 2141
    wxArrayString m_atc = wxSplit(wxString::FromUTF8(atccode), ';');
    wxArrayString m_class = wxSplit(wxString::FromUTF8(atcclass), ';');
    wxString m_atccode_str;
    wxString m_atcclass_str;
    if (m_atc.size() > 1) {
        if (!m_atc[0].IsEmpty())
            m_atccode_str = m_atc[0];

        if (!m_atc[1].IsEmpty())
            m_atcclass_str = m_atc[1];
    }
    else {
        m_atccode_str = _("Not specified");
    }
    
    // 2154
    if (m_atccode_str.IsEmpty())
        m_atccode_str = _("Not specified");

    if (m_atcclass_str.IsEmpty())
        m_atcclass_str = _("Not specified");
    
    wxString m_atcclass;
    if (m_class.size() == 2) {  // *** Ver.<1.2
        m_atcclass = m_class[1];
        if (m_atcclass.IsEmpty())
            m_atcclass = _("Not specified");

        m->subTitle = wxString::Format("%s - %s\n%s", m_atccode_str, m_atcclass_str, m_atcclass); // TBC \n or <br> ?
    }
    else if (m_class.size() == 3) {  // *** Ver.>=1.2
        wxArrayString m_atc_class_l4_and_l5 = wxSplit(wxString::FromUTF8(m_class[2]), '#');
        int n = (int)m_atc_class_l4_and_l5.size();
        if (n>1)
            m_atcclass = m_atc_class_l4_and_l5[n-2];

        if (m_atcclass.IsEmpty())
            m_atcclass = _("Not specified");

        m->subTitle = wxString::Format("%s - %s\n%s\n%s", m_atccode_str, m_atcclass_str, m_atcclass, m_class[1]);
    }
    else {
        m_atcclass = _("Not specified");
        m->subTitle = _("Not specified");
    }

    m->medId = medId;
    
    doArray.push_back(m); // __deprecated
    myTableView->searchRes.push_back(m);
}

// 2188
void MainWindow::addTitle_andRegnrs_andAuthor_andMedId(wxString title, wxString regnrs, wxString author, long medId)
{
    DataObject *m = new DataObject;
    if (title.size() > 0)
        m->title = title;
    else
        m->title = _("Not specified");

    wxString m_regnrs = regnrs;
    wxString m_auth = author;
    if (m_regnrs.IsEmpty())
        m_regnrs = _("Not specified");

    if (m_auth.IsEmpty())
        m_auth = _("Not specified");

    m->subTitle = wxString::Format("%s - %s", m_regnrs, m_auth);
    m->medId = medId;

    doArray.push_back(m); // __deprecated
    myTableView->searchRes.push_back(m);
}

// 2237
void MainWindow::addTitle_andApplications_andMedId(wxString title, wxString applications, long medId)
{
    DataObject *m = new DataObject;
    if (title.size() > 0)
        m->title = title;
    else
        m->title = _("Not specified");

    //
    wxArrayString m_applications = wxSplit(wxString::FromUTF8(applications), ';');
    wxString m_swissmedic;
    wxString m_bag;
    if (m_applications.size() > 0) {
        if (!m_applications[0].IsEmpty())
            m_swissmedic = m_applications[0];

        if (m_applications.size() > 1) {
            if (!m_applications[1].IsEmpty())
                m_bag = m_applications[1];
        }
    }
    if (m_swissmedic.IsEmpty())
        m_swissmedic = _("Not specified");

    if (m_bag.IsEmpty())
        m_bag = _("Not specified"); // @"k.A.";

    m->subTitle = wxString::Format("%s\n%s", m_swissmedic, m_bag);
    m->medId = medId;

    doArray.push_back(m); // __deprecated
    myTableView->searchRes.push_back(m);
}

// 2271
void MainWindow::addKeyword_andNumHits_andHash(wxString keyword, unsigned long numHits, wxString hash)
{
    DataObject *m = new DataObject;

    if (keyword.size() > 0)
        m->title = keyword;
    else
        m->title = _("Not specified");

    m->subTitle = wxString::Format("%ld %s", numHits, _("Results"));  // Treffer
    m->hashId = hash;

    doArray.push_back(m); // __deprecated
    myTableView->searchRes.push_back(m);
}

// 2288
void MainWindow::updateTableView()
{
    //std::cerr << __PRETTY_FUNCTION__  << std::endl;
 
#if 0
    if (mCurrentSearchState == kss_FullText) {
        if (searchResultsFT.size() == 0) {
            stopProgressIndicator();
            std::cerr << __FUNCTION__ << " 0 results" << std::endl;
            return;
        }
    }
    else {
        if (searchResults.size() == 0) {
            stopProgressIndicator();
            std::cerr << __FUNCTION__ << " 0 results" << std::endl;
            return;
        }
    }
#endif
    
#ifndef NDEBUG
    std::cerr << __FUNCTION__ << " Line " << __LINE__
            << " searchResults.size " << searchResults.size()
            << std::endl;
#endif

    if (doArray.size() > 0)
        doArray.clear(); // __deprecated

    if (myTableView->searchRes.size() > 0)
        myTableView->searchRes.clear();

    // 2295
    if (favoriteKeyData.size() > 0)
        favoriteKeyData.Clear(); // removeAllObjects

    // 2298
    if (mCurrentSearchState == kss_Title) {
        if (mUsedDatabase == kdbt_Aips) {
            // 2300
            for (auto m : searchResults) {
                favoriteKeyData.Add(m->regnrs);
                if (mSearchInteractions == false)
                    addTitle_andPackInfo_andMedId(
                    		wxString::FromUTF8(m->title),
                    		wxString::FromUTF8(m->packInfo),
							m->medId);
                else
                    addTitle_andPackInfo_andMedId(
                    		wxString::FromUTF8(m->title),
							m->atccode,
							m->medId);
            }
        }
        // 2310
        else if (mUsedDatabase == kdbt_Favorites) {
            for (auto m : searchResults) {
                // 2313
                FAVORITES_SET::iterator it = favoriteMedsSet.find(m->regnrs);
                if (it != favoriteMedsSet.end()) {
                    favoriteKeyData.Add(m->regnrs);
                    addTitle_andPackInfo_andMedId(wxString::FromUTF8(m->title), m->packInfo, m->medId);
                }
            }
        }
    }
    // 2321
    else if (mCurrentSearchState == kss_Author) {
        for (auto m : searchResults) {
            // 2323
            if (mUsedDatabase == kdbt_Aips) {
                if (m->regnrs.length() > 0) {
                    favoriteKeyData.Add(m->regnrs);
                    addTitle_andAuthor_andMedId(wxString::FromUTF8(m->title), m->auth, m->medId);
                }
            }
            // 2328
            else if (mUsedDatabase == kdbt_Favorites) {
                if (m->regnrs.length() > 0) {
                    FAVORITES_SET::iterator it = favoriteMedsSet.find(m->regnrs);
                    if (it != favoriteMedsSet.end()) {
                        favoriteKeyData.Add(m->regnrs);
                        addTitle_andAuthor_andMedId(wxString::FromUTF8(m->title), m->auth, m->medId);
                    }
                }
            }
        }
    }
    // 2338
    else if (mCurrentSearchState == kss_AtcCode) {
        for (auto m : searchResults) {
            // 2340
            if (mUsedDatabase == kdbt_Aips) {
                if (m->regnrs.length() > 0) {
                    favoriteKeyData.Add(m->regnrs);
                    addTitle_andAtcCode_andAtcClass_andMedId(wxString::FromUTF8(m->title), m->atccode, m->atcClass, m->medId);
                }
            }
            // 2345
            else if (mUsedDatabase == kdbt_Favorites) {
                if (m->regnrs.length() > 0) {
                    FAVORITES_SET::iterator it = favoriteMedsSet.find(m->regnrs);
                    if (it != favoriteMedsSet.end()) {
                        favoriteKeyData.Add(m->regnrs);
                        addTitle_andAtcCode_andAtcClass_andMedId(wxString::FromUTF8(m->title), m->atccode, m->atcClass, m->medId);
                    }
                }
            }
        }
    }
    // 2355
    else if (mCurrentSearchState == kss_RegNr) {
        for (auto m : searchResults) {
            // 2357
            if (mUsedDatabase == kdbt_Aips) {
                if (m->regnrs.length() > 0) {
                    favoriteKeyData.Add(m->regnrs);
                    addTitle_andRegnrs_andAuthor_andMedId(
                            wxString::FromUTF8(m->title),
                            m->regnrs,
                            m->auth, m->medId);
                }
            }
            // 2363
            else if (mUsedDatabase == kdbt_Favorites) {
                if (m->regnrs.length() > 0) {
                    FAVORITES_SET::iterator it = favoriteMedsSet.find(m->regnrs);
                    if (it != favoriteMedsSet.end()) {
                        favoriteKeyData.Add(m->regnrs);
                        addTitle_andRegnrs_andAuthor_andMedId(wxString::FromUTF8(m->title), m->regnrs, m->auth, m->medId);
                    }
                }
            }
        }
    }
    // 2373
    else if (mCurrentSearchState == kss_Therapy) {
        for (auto m : searchResults) {
            // 2375
            if (mUsedDatabase == kdbt_Aips) {
                if (m->regnrs.length() > 0) {
                    favoriteKeyData.Add(m->regnrs);
                    addTitle_andApplications_andMedId(wxString::FromUTF8(m->title), m->application, m->medId);
                }
            }
            // 2381
            else if (mUsedDatabase == kdbt_Favorites) {
                if (m->regnrs.length() > 0) {
                    FAVORITES_SET::iterator it = favoriteMedsSet.find(m->regnrs);
                    if (it != favoriteMedsSet.end()) {
                        favoriteKeyData.Add(m->regnrs);
                        addTitle_andApplications_andMedId(wxString::FromUTF8(m->title), m->application, m->medId);
                    }
                }
            }
        }
    }
    // 2391
    else if (mCurrentSearchState == kss_FullText) {
        for (auto e : searchResultsFT) {
            if (mUsedDatabase == kdbt_Aips ||
                mUsedDatabase == kdbt_Favorites)
            {
                if (!e->hash.IsEmpty()) {
                    favoriteKeyData.Add(e->hash);
                    addKeyword_andNumHits_andHash(e->keyword, e->getNumHits(), e->hash);
                }
            }
        }
    }
    
    // 2402
    // Sort alphabetically
    if (mUsedDatabase == kdbt_Favorites) {
        std::clog << __PRETTY_FUNCTION__ << " TODO: Sort alphabetically" << std::endl;
        //NSSortDescriptor *titleSort = [NSSortDescriptor sortDescriptorWithKey:@"title" ascending:YES];
        //[doArray sortUsingDescriptors:[NSArray arrayWithObject:titleSort]];
    }

    stopProgressIndicator();
    std::cerr << __PRETTY_FUNCTION__ << " Ended" << std::endl;
}

// 2412
// Add med in the buffer to the interaction basket
void MainWindow::pushToMedBasket(Medication *med)
{
    mInteractionsView->pushToMedBasket(med);
}

// 2424
// Handler for EVT_WEBVIEW_TITLE_CHANGED
// In amiko-osx see createJSBridge
void MainWindow::OnTitleChanged(wxWebViewEvent& evt)
{
    wxString str = wxString::FromUTF8(evt.GetString());
    if (str.IsEmpty())
        return;

    // str now contains the JSON string set in the JavaScript

#ifndef NDEBUG
    std::cerr << __PRETTY_FUNCTION__ << " line: " << __LINE__ << " JSON: <" << str << ">" << std::endl;
#endif

    nlohmann::json msg;
    try {
        msg = nlohmann::json::parse((const char *)str.c_str());
    }
    catch (const std::exception&e) {
        std::cerr << __PRETTY_FUNCTION__ << " line: " << __LINE__ 
                << " Exception: " << e.what() 
                << std::endl;
        return;
    }
    
    if (msg.size() == 3) {              // Interactions
        // 2432
        if (msg[0] == "interactions_cb") {
            if (msg[1] == "notify_interaction")
                mInteractionsView->sendInteractionNotice();
            else if (msg[1] == "delete_all")
                mInteractionsView->clearMedBasket();
            else if (msg[1] == "delete_row")
                mInteractionsView->removeFromMedBasketForKey(msg[2]);
            
            // Update med basket
            mCurrentWebView = kInteractionsCartView;
            updateInteractionsView();
        }
    }
    else if (msg.size() == 4) {         // Full text search
        // 2447
        // msg[0] is "main_cb"
        // msg[1] is "display_fachinfo"
        std::string ean = msg[2];
        std::string anchor = msg[3];
        if (ean.length() > 0) {
            // 2452
            mCurrentWebView = kExpertInfoView;
            mMed = mDb->getMediWithRegnr(ean);
            updateExpertInfoView(anchor);
            
            // TODO: moveToHighlight ?
        }
    }
}

// 2464
// Handler for EVT_WEBVIEW_LOADED
// Callback invoked when a page is finished loading
// In amiko-osx see WebFrameLoadDelegate - webView:didFinishLoadForFrame:
void MainWindow::OnDocumentLoaded(wxWebViewEvent& evt)
{
#if 0
    std::clog << __PRETTY_FUNCTION__
    << " Document loaded, evt.GetURL(): <" << evt.GetURL()
    << ">, myWebView URL: <" << myWebView->GetCurrentURL() << ">"
    << ">, mAnchor: <" << mAnchor << ">"
    << std::endl;
#endif

#if 0
    // Only notify if the document is the main frame, not a subframe
    if (evt.GetURL() == m_browser->GetCurrentURL())
#endif
    {
        // Inject JS into webview
        if (mAnchor.length() > 0) {
            wxString jsCallback = wxString::Format("moveToHighlight('%s')", mAnchor);
            myWebView->RunScript(jsCallback); // stringByEvaluatingJavaScriptFromString
        }
    }
}

// 2473
void MainWindow::updateExpertInfoView(wxString anchor)
{
	//std::cerr << __PRETTY_FUNCTION__ << std::endl;
    
    if (!mMed) {
        std::clog << __PRETTY_FUNCTION__ << " FIXME: mMed is NULL" << std::endl;
        return;
    }

	// 2476
    wxString color_Style = wxString::Format("<style type=\"text/css\">%s</style>", UTI::getColorCss());
    
    // 2479
    wxString amiko_Style; // TODO: read it once only and store it, instead of doing this for every fachinfo
    {
        // Load style sheet from file
        wxString amikoCssPath = wxStandardPaths::Get().GetResourcesDir();
        amikoCssPath += wxFILE_SEP_PATH + wxString("amiko_stylesheet.css");

        wxString amikoCss;
        if (wxFileName::Exists(amikoCssPath)) {
            wxFileInputStream input( amikoCssPath );
            wxTextInputStream text(input, wxT("\x09"), wxConvUTF8 );
            while (input.IsOk() && !input.Eof() )
                amikoCss += text.ReadLine() + wxT("\n");
        }
        else
            amikoCss = mMed->styleStr; // TODO: Unused ?
        
        amiko_Style = wxString::Format("<style type=\"text/css\">%s</style>", amikoCss);
    }
    
    //std::cerr << "amiko_Style: " << amiko_Style << std::endl;
    
    // 2492
    wxString js_Script;
    {
        // Load JavaScript from file
        wxString jscriptPath = wxStandardPaths::Get().GetResourcesDir();        
        jscriptPath += wxFILE_SEP_PATH + wxString("main_callbacks.js");

        wxString jscriptStr;
        //= [NSString stringWithContentsOfFile:jscriptPath encoding:NSUTF8StringEncoding error:nil];
        if (wxFileName::Exists(jscriptPath)) {
            wxFileInputStream input( jscriptPath );
            wxTextInputStream text(input, wxT("\x09"), wxConvUTF8 );
            while (input.IsOk() && !input.Eof() )
                jscriptStr += text.ReadLine() + wxT("\n");
        }

        //std::cerr << "jscriptStr: " << jscriptStr << std::endl;

        js_Script = wxString::Format("<script type=\"text/javascript\">%s</script>", jscriptStr);
    }

    // 2502
    // Generate HTML string
    wxString htmlStr = wxString::FromUTF8(mMed->contentStr);
    //std::cerr << "Line " << __LINE__  << " <" << htmlStr << ">" << std::endl;

    // 2508
    const char *charset_Meta = "<meta charset=\"utf-8\" />";
    const char *colorScheme_Meta = "<meta name=\"supported-color-schemes\" content=\"light dark\" />";
    wxString headHtml = wxString::Format("<head>%s\n%s\n%s\n%s\n%s\n</head>",
                                         charset_Meta,
                                         colorScheme_Meta,
                                         js_Script.c_str(),
                                         color_Style.c_str(),
                                         amiko_Style.c_str());

    // 2517
   htmlStr.Replace("<head></head>", headHtml, false);

    // 2529
    // TODO: Some tables have the color set in the HTML string (not set with CSS)

    if (mCurrentSearchState == kss_FullText) {
        // 2534
        if (mFullTextEntry) {
            wxString keyword = mFullTextEntry->keyword;
            if (!keyword.IsEmpty()) {
                // Instead of appending like in the Windows version,
                // insert before "</body>"
                wxString jsCode = wxString::Format("highlightText(document.body,'%s')", keyword);
                wxString extraHtmlCode = wxString::Format("<script>%s</script>\n </body>", jsCode);
                htmlStr.Replace("</body>", extraHtmlCode);
            }
        }

        mAnchor = anchor;
    }

    // 2547
    myWebView->SetPage(htmlStr, wxString()); // loadHTMLString

    // 2553
    if (!mPrescriptionMode) {
        // Extract section ids
        if (mMed->sectionIds)
            mListOfSectionIds = mMed->listOfSectionIds();

        // Extract section titles
        if (mMed->sectionTitles)
            mListOfSectionTitles = mMed->listOfSectionTitles();

        //std::clog << "Line " << __LINE__  << " size " << mListOfSectionTitles.size() << std::endl; // 20

        // 2561
        mySectionTitles_reloadData(); // reloadData
    }
}

// 2565
void MainWindow::updateInteractionsView()
{
    // Generate main interaction table
    wxString htmlStr = mInteractionsView->fullInteractionsHtml(mInteractions);
    
    // NSURL *mainBundleURL = [NSURL fileURLWithPath:[[NSBundle mainBundle] bundlePath]];
    // [[myWebView mainFrame] loadHTMLString:htmlStr baseURL:mainBundleURL];
 
    //std::clog << __FUNCTION__ << " htmlStr:\n" << htmlStr << std::endl;

    myWebView->SetPage(htmlStr, wxString()); // loadHTMLString

    // 2577
    if (!mPrescriptionMode) {
        // Update section title anchors
        if (mInteractionsView->listofSectionIds.size() > 0)
            mListOfSectionIds = mInteractionsView->listofSectionIds;

        // Update section titles (here: identical to anchors)
        if (mInteractionsView->listofSectionTitles.size() > 0)
            mListOfSectionTitles = mInteractionsView->listofSectionTitles;
        
        mySectionTitles_reloadData(); // reloadData
    }
}

// 2589
void MainWindow::updatePrescriptionsView()
{
    // 2591
    // Switch tab view
    myTabView->ChangeSelection(2);
    // and corresponding toolbar button
    myToolbar->ToggleTool(wxID_TB_PRESCRIPTION, true); // setSelectedItemIdentifier("Rezept")

    // Update date
    wxString placeDate = mPrescriptionAdapter->placeDate;
    if (!placeDate.IsEmpty())
        myPlaceDateField->SetLabel(placeDate);

    myPrescriptionsTableView_reloadData(0);
    mPrescriptionMode = true;
}

// 2603
void MainWindow::updateFullTextSearchView(wxString contentStr)
{
    //std::clog << __FUNCTION__ << " contentStr: <" << contentStr << ">" << std::endl;

    wxString colorCss = UTI::getColorCss();

    // Load style sheet from file
    wxString fullTextCssPath = wxStandardPaths::Get().GetResourcesDir();
    fullTextCssPath += wxFILE_SEP_PATH + wxString("fulltext_style.css");

    wxString fullTextCss;
    if (wxFileName::Exists(fullTextCssPath)) {
        wxFileInputStream input( fullTextCssPath );
        wxTextInputStream text(input, wxT("\x09"), wxConvUTF8 );
        while (input.IsOk() && !input.Eof() )
            fullTextCss += text.ReadLine() + wxT("\n");
    }

    wxString js_Script;
    {
		// Load javascript from file
        wxString jscriptPath = wxStandardPaths::Get().GetResourcesDir();
        jscriptPath += wxFILE_SEP_PATH + wxString("main_callbacks.js");
        
        wxString jscriptStr;
        //= [NSString stringWithContentsOfFile:jscriptPath encoding:NSUTF8StringEncoding error:nil];
        if (wxFileName::Exists(jscriptPath)) {
            wxFileInputStream input( jscriptPath );
            wxTextInputStream text(input, wxT("\x09"), wxConvUTF8 );
            while (input.IsOk() && !input.Eof() )
                jscriptStr += text.ReadLine() + wxT("\n");
        }
        
        js_Script = wxString::Format("<script type=\"text/javascript\">%s</script>", jscriptStr);
    }

    wxString htmlStr = wxString::Format("<html><head><meta charset=\"utf-8\" /><meta name=\"supported-color-schemes\" content=\"light dark\" />");
    htmlStr += wxString::Format("%s\n<style type=\"text/css\">%s</style><style type=\"text/css\">%s</style></head><body><div id=\"fulltext\">%s</div></body></html>",
             js_Script,
             colorCss,
             fullTextCss,
             contentStr);

    //std::clog << __FUNCTION__ << " line " << __LINE__ << " htmlStr: <" << htmlStr << ">" << std::endl;

    // 2622
    myWebView->SetPage(htmlStr, wxString()); // loadHTMLString

    // 2626
    // Update right pane (section titles)
    if (mFullTextSearch->listOfSectionIds.size() > 0)
        mListOfSectionIds = mFullTextSearch->listOfSectionIds;

    if (mFullTextSearch->listOfSectionTitles.size() > 0)
        mListOfSectionTitles = mFullTextSearch->listOfSectionTitles;

    // 2631
    mySectionTitles_reloadData(); // reloadData
}

// 2635
// Also, possibly set the flag for a modified prescription
void MainWindow::storeAllPrescriptionComments()
{
    // Get all comments
    wxArrayString comments;
    
    wxTreeItemId rootItem = myPrescriptionsTableView->GetRootItem();
    if (!rootItem.IsOk())
        return;

    int numRows = myPrescriptionsTableView->GetChildrenCount(rootItem, false);

    // 2640
    wxTreeItemIdValue cookie;
    for (wxTreeItemId medItem = myPrescriptionsTableView->GetFirstChild(rootItem, cookie);
         medItem.IsOk();
         medItem = myPrescriptionsTableView->GetNextChild(rootItem, cookie))
    {
        int childrenCount = myPrescriptionsTableView->GetChildrenCount( medItem, false);
        if (childrenCount == 2) {
            wxTreeItemIdValue subCookie;
            wxTreeItemId gtinItem = myPrescriptionsTableView->GetFirstChild(medItem, subCookie);
            wxTreeItemId commentItem = myPrescriptionsTableView->GetNextChild(medItem, subCookie);
            
            // 2643
            comments.Add(myPrescriptionsTableView->GetItemText(commentItem));
        }
        else
            comments.Add(wxEmptyString);
    }

    // 2646
    int row = 0;
    for (PrescriptionItem *item : mPrescriptionsCart[0].cart) {
        if (row < numRows) {
#ifndef NDEBUG
            std::cerr << __FUNCTION__ << " line: " << __LINE__
            << ", comment <" << item->comment
            << "> <" << comments[row] << ">"
            << std::endl;
#endif

            if (item->comment != comments[row]) {
                item->comment = comments[row];
                modifiedPrescription = true;
            }
        }

        row++;
    }
}

// 2660
void MainWindow::addItem_toPrescriptionCartWithId(PrescriptionItem *item, int n)
{
    if (n < NUM_ACTIVE_PRESCRIPTIONS)
    {
        //if (!mPrescriptionsCart[n].cart)
        {
            //mPrescriptionsCart[n].cart = [[NSMutableArray alloc] init];
            mPrescriptionsCart[n].cartId = n;
        }

        // Get all prescription comments from table
        storeAllPrescriptionComments();

        // 2672
        // Get medi
        item->med = mDb->getShortMediWithId(item->mid);
        mPrescriptionsCart[n].addItemToCart(item);
        myPrescriptionsTableView_reloadData(n);

        modifiedPrescription = true;
        //updateButtons(); // __deprecated
    }
}

// 3047
// __deprecated The framework will call OnUpdateUI() instead
//void MainWindow::updateButtons()
//{
//    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
//}

// 3101
// Read the file containing the list of keywords
wxArrayString MainWindow::csvGetInputListFromFile()
{
    std::clog << __FUNCTION__ << std::endl;
    
    // Create a file open dialog class
    wxString wildCard = wxString::Format("CSV %s (*.csv;*.CSV)|*.csv;*.CSV|TXT %s (*.txt)|*.txt",
                                         _("files"), _("files"));
    wxFileDialog openDlgPanel(this,
                              _("Please select text file with one word or two words per line. The file can be created into a text editor. Encoding is UTF-8"),
                              wxEmptyString,
                              wxEmptyString,
                              wildCard,
                              wxFD_OPEN | wxFD_FILE_MUST_EXIST, // no wxFD_MULTIPLE
                              wxDefaultPosition);

    wxArrayString keywordList;

    if (openDlgPanel.ShowModal() != wxID_OK)
    {
        //std::clog << __FUNCTION__ << __LINE__ << " Canceled" << std::endl;
        return keywordList;
    }

    wxString filePath = openDlgPanel.GetPath();
    //std::clog << "Path " << filePath << std::endl;
    
    wxFileInputStream input(filePath);
    wxTextInputStream text(input, wxT("\x09"), wxConvUTF8 );
    while(input.IsOk() && !input.Eof() )
    {
        wxString line = text.ReadLine();// + wxT("\n");
        if (line.length() == 0)
            continue;

        //std::clog << "Line " << line << std::endl;
        keywordList.Add(line);
    }

    return keywordList;
}

// 3124
void MainWindow::searchKeyword_inMedication_chapters_regnr(wxString aKeyword,
                                                           Medication *med,
                                                           std::set<wxString> chSet,
                                                           wxString rn)
{
    wxString html = med->contentStr;
    wxString atc = med->atccode;

#ifndef NDEBUG
    //NSLog(@"%s %d, html %p lenght:%lu", __FUNCTION__, __LINE__, html, (unsigned long)[html length]);
    //NSLog(@"%s %d, html %@", __FUNCTION__, __LINE__, html);
    std::clog << __FUNCTION__ << __LINE__ << ", chapters size: " << chSet.size() << std::endl;
#endif

    // 3138
    if (chSet.size() == 0) {
        std::clog << "WARNING: Keyword <" << aKeyword << "> has no chapters";
    }
    else {
        for (auto el : chSet) {
            if (el.length() == 0) {
                std::clog << "WARNING: keyword <" << aKeyword << "> has empty chapter set";
            }
        }
    }
    
    // 3150
    wxStringInputStream in(html);
    wxXmlDocument xmlDoc(in);
    std::clog << "xmlDoc ver: " << xmlDoc.GetVersion()
    << ", ok: " << xmlDoc.IsOk()
    << ", root: " << xmlDoc.GetRoot()
    << ", docNode " << xmlDoc.GetDocumentNode()
    << std::endl;
    
    if (!xmlDoc.IsOk())
        return;

    // 3170
    wxXmlNode *rootElement = xmlDoc.GetRoot(); // rootElement
    std::clog << "rootElement content:<" << rootElement->GetContent() << ">"
    << ", node content:<" << rootElement->GetNodeContent() << ">"
    << ", name:<" << rootElement->GetName() << ">" // ok "html"
    << ", line No: " << rootElement->GetLineNumber()
    << std::endl;
    //NSLog(@"Line %d HTML root children count %lu", __LINE__, (unsigned long)[rootElement childCount]);
    wxASSERT(rootElement);

#if 0
    wxXmlNode *child = rootElement->GetChildren();
    while (child) {
        std::clog << "child type: " << child->GetType()
        << ", name:<" << child->GetName() << ">" // "head" and "body" (body truncated in the printout)
        << ", node content:<" << child->GetNodeContent() << ">"
        << ", line No: " << child->GetLineNumber()
        << ", type: " << child->GetType() // wxXML_ELEMENT_NODE
        << std::endl; // NSXMLFidelityElement "head" and "body"
        
        wxXmlAttribute *attr = child->GetAttributes();
        while (attr) {
            std::clog << "attr  name: " << attr->GetName() << std::endl;
            attr = attr->GetNext();
        }

        child = child->GetNext();
    }
#endif
    
#if 0
    wxXmlNode *nodeBody = rootElement->GetChildren(); //children[1];
    nodeBody = nodeBody->GetNext();
//    std::clog << "Line " << __LINE__ << ", class:%@, name:%@, %lu children", ,
//          [nodeBody class],
//          [nodeBody name],
//          (unsigned long)[nodeBody childCount]);

    wxXmlNode *nodeBodyDiv = nodeBody->GetChildren(); //[nodeBody childAtIndex:0];
    while (nodeBodyDiv) {
        std::clog << "Line " << __LINE__
        << ", name:<" << nodeBodyDiv->GetName() << ">"  // div
        << ", node content:<" << nodeBodyDiv->GetNodeContent() << ">"
        << ", line No: " << nodeBodyDiv->GetLineNumber()
        << ", type: " << nodeBodyDiv->GetType() // wxXML_ELEMENT_NODE
        << std::endl;
        
        wxXmlAttribute *attr = nodeBodyDiv->GetAttributes();
        while (attr) {
            std::clog
            << "attr name:<" << attr->GetName() << ">"
            << ", value:<" << attr->GetValue() << ">"
            << std::endl;
            attr = attr->GetNext();
        }

        nodeBodyDiv = nodeBodyDiv->GetNext();
    }
#endif

//    NSArray *shortTitles = [csvMedication listOfSectionTitles];  // they are hardcoded into the app
    
    wxArrayString atcArray = wxSplit(atc, ';');
    wxString activeSubstance;
    wxString atcCode;
    if (atcArray.size() > 0) atcCode = atcArray[0];
    if (atcArray.size() > 1) activeSubstance = atcArray[1];

    // 3203
    wxString brandName;
    // nodesForXPath:@"/html/body/div/div" error:nil];
    wxXmlNode *pBodyElem = rootElement->GetChildren();  // html/head
    pBodyElem = pBodyElem->GetNext();                   // html/body
    pBodyElem = pBodyElem->GetChildren();               // html/body/div
    pBodyElem = pBodyElem->GetChildren();               // html/body/div/div
    while (pBodyElem) {

        // 3207
        wxString divClass = pBodyElem->GetAttribute("class");
        wxString divId = pBodyElem->GetAttribute("id");

#ifndef NDEBUG
        std::clog << "Line " << __LINE__
        << ", name:<" << pBodyElem->GetName() << ">"  // div
        << ", node content:<" << pBodyElem->GetNodeContent() << ">" // Ponstan
        << ", XML line No: " << pBodyElem->GetLineNumber()
        << ", type: " << pBodyElem->GetType() // wxXML_ELEMENT_NODE: 1
        << ", attr class:<" << divClass << ">"
        << ", attr id:<" << divId << ">"
        << std::endl;
#endif
        
        // 3209
        if (divClass == "MonTitle")
        {
            brandName = pBodyElem->GetNodeContent();
        }
        else if (divClass == "paragraph")
        {
            // 3223
            // Extract section number from id and skip if not in NSSet
            
            // 3228
            // Throw away characters before the first number.
            
            // 3252
            wxXmlNode *paragraphs = pBodyElem->GetChildren();
        }
#ifndef NDEBUG
        else {
            // 3218
            // <div class="ownerCompany">
            // <p class="footer">
            std::clog << "Line " << __LINE__
            << " skip name:<" << pBodyElem->GetName() << ">" // p or div
            << ", class:<" << divClass  << ">"              // ownerCompany or footer
            << ", id:<" << divId  << ">"                    // missing
            << std::endl;
        }
#endif

#if 0 //ndef NDEBUG
        wxXmlAttribute *attr = pBodyElem->GetAttributes();
        while (attr) {
            std::clog << "Line " << __LINE__
            << "attr name:<" << attr->GetName() << ">" // class, id
            << ", value:<" << attr->GetValue() << ">"  // MonTitle, section1
            << std::endl;

            attr = attr->GetNext();
        }
#endif

        pBodyElem = pBodyElem->GetNext();
    }
    
#if 1
    std::clog << __PRETTY_FUNCTION__ << " Line " << __LINE__ << " TODO" << std::endl;
#else
    for (auto el : pBodyElem) {
        wxString divClass = el.GetAttribute("class");
        wxString divId = el.GetAttribute("id");
        // 3209
        if (divClass == "MonTitle") {
            brandName = el.GetNodeContent();// stringValue];

            // Sanitize:
            brandName = [brandName stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
            continue;
        }

        if (divClass == "paragraph") {
#ifndef NDEBUG
            std::clog << "Line " << __LINE__
            << " skip class:" << divClass << " id:" << divId << std::endl;  // [el name] is "div2
#endif
            continue;
        }

        // Extract section number and skip if not in NSSet
        
        NSScanner *scanner = [NSScanner scannerWithString:divId];
        NSCharacterSet *numbers = [NSCharacterSet characterSetWithCharactersInString:@"0123456789"];
        
        // Throw away characters before the first number.
        [scanner scanUpToCharactersFromSet:numbers intoString:nullptr];
        
        // Collect numbers.
        NSString* numberString;
        [scanner scanCharactersFromSet:numbers intoString:&numberString];
        if (![chSet member:numberString])
            continue;   // skip this section

        wxString chapterName;
#if 0
        // It's not as simple as this:
        NSInteger chNumber = numberString.integerValue;
        chapterName = shortTitles[chNumber - 1];  // subtract 1 because chpater 1 has index 0
#else
        // See how it's done in file 'MLFullTextSeaerch.m' function 'tableWithArticles'

        //NSLog(@"Line %d, kw:%@ index:%ld of %lu", __LINE__, aKeyword, (long)chNumber, (unsigned long)[shortTitles count]);
        NSDictionary *indexToTitlesDict = [csvMedication indexToTitlesDict];
        //NSLog(@"Line %d, indexToTitlesDict:%@", __LINE__, indexToTitlesDict);
        chapterName = indexToTitlesDict[numberString];
        //NSLog(@"Line %d, cStr:%@", __LINE__, chapterName);
#endif

        NSArray *paragraphs = [el children];
#ifndef NDEBUG
        NSLog(@"Line %d, use section (%d) with %lu paragraphs", __LINE__,
              numberString.intValue,
              (unsigned long)[paragraphs count]);
#endif
        for (NSXMLElement *p in paragraphs) {
            // [p name]          is "div"
            // [p stringValue]   content of tag
            //NSLog(@"Line %d, %lu children", __LINE__, (unsigned long)[p childCount]);
            
            if ([[p stringValue] containsString:aKeyword]) {
                //NSLog(@"TODO: for %@ output this:\n\n%@\n", aKeyword, [p stringValue]);
                NSString *link = [NSString stringWithFormat:@"https://amiko.oddb.org/de/fi?gtin=%@&highlight=%@&anchor=%@", rn, aKeyword, divId];
                [csv appendFormat:@"\n%@%@%@%@\"%@\"%@\"%@\"%@%@%@\"%@\"%@%@",
                 aKeyword, CSV_SEPARATOR,
                 activeSubstance, CSV_SEPARATOR,
                 brandName, CSV_SEPARATOR,
                 atcCode, CSV_SEPARATOR,
                 chapterName, CSV_SEPARATOR,
                 [p stringValue], CSV_SEPARATOR,
                 link];

                //NSLog(@"Line %d, csv:%@", __LINE__, csv);
            }
        }
    }
    
#if 0
    NSXMLNode *pNode = [rootElement attributeForName:@"body:div"];
    NSLog(@"pNode %@", [pNode name]);
#endif
#endif
}

// 3287
// TODO: Run in a separate thread
void MainWindow::csvOutputResult()
{
#if 1
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
#else
    NSDateFormatter *dateFormatter=[[NSDateFormatter alloc] init];
    [dateFormatter setDateFormat:@"yyyy.MM.dd_HHmm"];
    NSString * dateSuffix = [dateFormatter stringFromDate:[NSDate date]];
    
    NSString *fileName = [NSString stringWithFormat:@"%@_%@.csv", NSLocalizedString(@"word_analysis", "CSV filename prefix"), dateSuffix];

    dispatch_async(dispatch_get_main_queue(), ^{
    
        // Select the directory
        NSOpenPanel* oPanel = [NSOpenPanel openPanel];
        [oPanel setCanChooseFiles:NO];
        [oPanel setCanChooseDirectories:YES];
        [oPanel setMessage:NSLocalizedString(@"Please select a directory where to save the file", nil)];
        NSURL *desktopURL = [[[NSFileManager defaultManager] URLsForDirectory:NSDesktopDirectory
                                                                      inDomains:NSUserDomainMask] lastObject];
        [oPanel setDirectoryURL:desktopURL];
        [oPanel setPrompt:NSLocalizedString(@"Choose directory and save file", nil)];
        [oPanel setCanCreateDirectories:YES];

        [oPanel beginWithCompletionHandler:^(NSInteger result) {
            if (result != NSModalResponseOK) {
                //NSLog(@"%s %d", __FUNCTION__, __LINE__);
                return;
            }

            NSURL *dirUrl = [oPanel URL];
            NSString *fullPathCSV = [[dirUrl path] stringByAppendingPathComponent:fileName];
            //NSLog(@"%s %d fullPathCSV:<%@>", __FUNCTION__, __LINE__, fullPathCSV);

            NSError *error;
            BOOL res = [csv writeToFile:fullPathCSV atomically:YES encoding:NSUTF8StringEncoding error:&error];
            
            if (!res) {
                NSLog(@"Error %@ while writing to file %@", [error localizedDescription], fileName );
                return;
            }

            [[NSWorkspace sharedWorkspace] openFile:fullPathCSV];
        }];
    });
#endif
}

// 3332
// TODO: Run in a separate thread
void MainWindow::csvProcessKeywords(wxArrayString keywords)
{
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
    wxBusyInfo info(_("Looking up keywords. Please wait..."), this);
    // TODO: progress bar

#if 1
//    __block Wait *wait;
//
//    dispatch_async(dispatch_get_main_queue(), ^{
//        wait = [[Wait alloc] initWithString:NSLocalizedString(@"Looking up keywords. Please wait...", nil)];
//        [wait setCancel:YES];
//        [[wait progress] setMaxValue:[keywords count]];
//        [wait setSponsorTitle:NSLocalizedString(@"This feature is provided by", nil)];
//        [wait showWindow:self];
//    });
    
    for (wxString kw : keywords) {
        if (kw.length() < 3) // this also takes care of the empty line at the end of file
            continue;
        
#if 0
        // amiko_frequency_de.db
        searchAnyDatabasesWith(kw);  // it updates searchResults and searchResultsFT
#else
        FULLTEXT_RESULTS resultDb1 = mFullTextDb->searchKeyword(kw);
#endif
        
#ifndef NDEBUG
        std::clog << "Line " << __LINE__
        << " ========= search keyword: <" << kw
        << "> in DB1 frequency table, " << resultDb1.size() << " hit(s)\n";
        //NSLog(@"Line %d, resultDb1:%@", __LINE__, resultDb1);
#endif
        for (auto entry : resultDb1) {
            if (entry->keyword != kw) {
                // Make the search case sensitive. Easier to do it this way than through SQL
#ifndef NDEBUG
                std::clog << "Line " << __LINE__
                << " --------- skip: " << entry->keyword << std::endl;
#endif
                continue;
            }
            
#ifndef NDEBUG
            std::clog << "Line " << __LINE__
            << " --------- use: " << entry->keyword
            << ", regnrs: " << entry->regnrs
            << ", hash: " << entry->hash
            << std::endl;
#endif
            //NSLog(@"%d getRegChaptersDict: %@", __LINE__, [entry getRegChaptersDict]);
            // 3371
            std::clog << "getRegnrs: " << entry->getRegnrs();  // as string
            wxArrayString rnArray = entry->getRegnrsAsArray();
            //NSLog(@"Line %d, getRegnrsAsArray: %@", __LINE__, rnArray);
            for (wxString rn : rnArray) {
                
                //NSDictionary *dic2 = [entry getRegChaptersDict];    // One or more lines like:
                //NSLog(@"Line %d, chapter dic: %@", __LINE__, dic2); // 65161 = "{(\n    14\n)}"
                
                // 3379
                std::set<wxString> chapterSet = entry->getChaptersForKey(rn);
#ifndef NDEBUG
                std::clog << "Line " << __LINE__
                << ", rn: " << rn
                << " has chapter set: " << chapterSet.size() << std::endl;
#endif
                
                csvMedication = mDb->getMediWithRegnr(rn);
                searchKeyword_inMedication_chapters_regnr(kw ,csvMedication, chapterSet, rn);
            }
        }  // for

//        dispatch_async(dispatch_get_main_queue(), ^{
//            [wait incrementBy: 1];
//            [wait setSubtitle:kw];
//        });
    } // for keywords
    
//    dispatch_async(dispatch_get_main_queue(), ^{
//        [wait close];
//    });
#endif
}

// /////////////////////////////////////////////////////////////////////////////

// 3047
// amiko-osx updateButtons
void MainWindow::OnUpdateUI( wxUpdateUIEvent& event )
{
    // 3050
    bool doctorDefined =
    myOperatorIDTextField->GetValue().length() > 0 &&
    myOperatorIDTextField->GetValue() != _("Enter the doctor's address");
    bool patientDefined = myPatientAddressTextField->GetValue().length() > 0;
    
    // 3058
    if (doctorDefined &&
        patientDefined &&
        mPrescriptionsCart[0].cart.size() > 0)
    {
        if (modifiedPrescription)
            saveButton->Enable(true);
        
        sendButton->Enable(true);
    }
    else
    {
        saveButton->Enable(false);
        sendButton->Enable(false);
    }
    
    // See also validateMenuItem()
    
    // "Delete amk" button
    if (mPrescriptionMode)
    {
        btnDelAmk->Show();
        if (mySectionTitles->GetItemCount() > 0)
            btnDelAmk->Enable(true);
        else
            btnDelAmk->Enable(false);
    }
    else
    {
        btnDelAmk->Hide();
        btnDelAmk->Enable(false);
    }
}

// 949
void MainWindow::OnSearchNow( wxCommandEvent& event )
{
    wxString searchText = mySearchField->GetValue();

#ifndef NDEBUG
    std::cerr << "=== " << __FUNCTION__
    << " from ID: " << event.GetId()  // wxID_MY_SEARCH_FIELD
    << " <" << searchText << ">"
    << std::endl;
#endif

#if 0
    if (mCurrentSearchState == kss_WebView)
        return;
#endif

    while (mSearchInProgress) {
        //[NSThread sleepForTimeInterval:0.005];  // Wait for 5ms
        wxMilliSleep(10);
        wxTheApp->Yield();
    }

    if (!mSearchInProgress) {
        mSearchInProgress = true;
    }

    // TODO: amiko-osx issue #131
    if (searchText.length() > 0) { // TODO: > 2 ?
        //searchResults =
        searchAnyDatabasesWith(searchText); // it updates searchResults and searchResultsFT
    }
    else {
        if (mUsedDatabase == kdbt_Favorites) {
            searchResults = retrieveAllFavorites();
            searchResultsFT = retrieveAllFTFavorites();
        }        
    }

    // 977
    // Update tableview
    updateTableView();
    
    if (mCurrentSearchState == kss_FullText) {
        myTableView->SetItemCount(searchResultsFT.size()); // reloadData
        myTableView->SetSelection(wxNOT_FOUND); // Initially no selection
    }
    else {
        myTableView->SetItemCount(searchResults.size()); // reloadData
        myTableView->SetSelection(wxNOT_FOUND); // Initially no selection
    }

    myTableView->Refresh();
    mSearchInProgress = false;
}

// 997
void MainWindow::OnButtonPressed( wxCommandEvent& event )
{
    //std::cerr << __FUNCTION__  << " button: " << event.GetId() << std::endl;

    int prevState = mCurrentSearchState;

    switch (event.GetId()) {
        case wxID_BTN_PREPARATION:
            setSearchState(kss_Title, wxID_BTN_PREPARATION);
            break;

        case wxID_BTN_REGISTRATION_OWNER:
            setSearchState(kss_Author, wxID_BTN_REGISTRATION_OWNER);
            break;

        case wxID_BTN_ACTIVE_SUBSTANCE:
            setSearchState(kss_AtcCode, wxID_BTN_ACTIVE_SUBSTANCE);
            break;

        case wxID_BTN_REGISTRATION_NUMBER:
            setSearchState(kss_RegNr, wxID_BTN_REGISTRATION_NUMBER);
            break;

        case wxID_BTN_THERAPY:
            setSearchState(kss_Therapy, wxID_BTN_THERAPY);
            break;

        case wxID_BTN_FULL_TEXT:
            setSearchState(kss_FullText, wxID_BTN_FULL_TEXT);
            mCurrentWebView = kFullTextSearchView;
            break;
    }

    if (prevState == kss_FullText || mCurrentSearchState == kss_FullText)
        updateSearchResults();

    updateTableView();

    if (mCurrentSearchState == kss_FullText) {
        myTableView->SetItemCount(searchResultsFT.size()); // reloadData
        myTableView->SetSelection(wxNOT_FOUND); // Initially no selection
    }
    else {
        myTableView->SetItemCount(searchResults.size()); // reloadData
        myTableView->SetSelection(wxNOT_FOUND); // Initially no selection
    }

    myTableView->Refresh();
}

// 2702
// TODO: tabView:didSelectTabViewItem:
void MainWindow::OnSimplebookPageChanged( wxBookCtrlEvent& event )
{
    std::clog << __PRETTY_FUNCTION__ << " TODO " << event.GetSelection() << std::endl;
    
    // It doesn't seem to be called, but see switchTabs()
    // and myTabView->SetSelection(2)
}

void MainWindow::OnSimplebookPageChanging( wxBookCtrlEvent& event )
{
    std::clog << __PRETTY_FUNCTION__ << " TODO " << event.GetSelection() << std::endl;
}

// There is no corresponding code in amiko-osx, because there it's implemented
// by injecting JavaScript code into the webview
// Called when the text changes in the search control
void MainWindow::OnSearchFiNow( wxCommandEvent& event )
{
    // TODO: make sure the Prescription view is not the current one

    wxString find_text = fiSearchField->GetValue();
    
    if (find_text.IsEmpty())
        m_findCount = myWebView->Find(wxEmptyString); // reset the search and highlights
    else
        m_findCount = myWebView->Find(find_text, wxWEBVIEW_FIND_HIGHLIGHT_RESULT);

#ifndef NDEBUG
    std::cerr << "=== " << __FUNCTION__
    << " from ID: " << event.GetId() // wxID_FI_SEARCH_FIELD
    << " <" << find_text << ">"
    << " m_findCount " << m_findCount
    #if __APPLE__
    << " (not supported yet on macos)"
    #endif
    << std::endl;
#endif

    if (!fiSearchCount->IsShown())
        fiSearchCount->Show();

    if (m_findCount > 0) {
        fiSearchCount->SetLabel(wxString::Format("%i", m_findCount)); 
        if (!fiSearchCount->IsShown())
            fiSearchCount->Show();
    }
    else {
        if (fiSearchCount->IsShown())
            fiSearchCount->Hide();        
    }
    fiSizer->Layout();
    
    if (m_findCount > 0 || find_text.IsEmpty())
        fiSearchField->SetBackgroundColour(fiSearchFieldBGColor);
    else
        fiSearchField->SetBackgroundColour(wxColour(255, 101, 101));
}

// 1390
// TODO: consolidate with OnManagePatients()
void MainWindow::OnSearchPatient( wxCommandEvent& event )
{
    OnManagePatients(event);
}

void MainWindow::OnTreeBeginLabelEdit( wxTreeEvent& event )
{
    if (event.GetId() != myPrescriptionsTableView->GetId()) { // wxID_MEDICINE_LIST
        // Wrong tree
        return;
    }
    
    wxTreeItemId item = event.GetItem();
    wxTreeItemId parent = myPrescriptionsTableView->GetItemParent(item);
    if (parent == myPrescriptionsTableView->GetRootItem()) {
        // Don't allow editing medicine name
        event.Veto();
        return;
    }

    wxTreeItemIdValue cookie; // For GetNextChild()
    wxTreeItemId firstChild = myPrescriptionsTableView->GetFirstChild(parent, cookie);
    if (item == firstChild) {
        // Don't allow editing GTIN (first child)
        event.Veto();
        return;
    }
}

void MainWindow::OnTreeEndLabelEdit( wxTreeEvent& event )
{
    if (event.GetId() != myPrescriptionsTableView->GetId()) { // wxID_MEDICINE_LIST
        // Wrong table
        return;
    }

    std::cerr << __FUNCTION__
    << " TODO: save new comment <" << event.GetLabel() << ">"
    << std::endl;

#if 0 // FIXME: unless we have a breakpoint in the debugger it crashes
    // If new text is empty remove comment
    if (event.GetLabel().IsEmpty()) {
        myPrescriptionsTableView->Delete(event.GetItem());
        myPrescriptionsTableView_reloadData(0);
        event.Skip();
        return;
    }
#endif

}

// Context menu for the selected item
void MainWindow::OnTreeItemMenu( wxTreeEvent& event )
{
    if (event.GetId() != myPrescriptionsTableView->GetId()) { // wxID_MEDICINE_LIST
        return;
    }

    std::cerr << __FUNCTION__
    << " item " << myPrescriptionsTableView->GetSelection().GetID()
    << std::endl;
    
    wxTreeItemId item = event.GetItem();
    wxTreeItemId medItem;

    if (myPrescriptionsTableView->GetItemParent(item) ==
        myPrescriptionsTableView->GetRootItem())
    {
        medItem = item;
    }
    else
    {
        medItem = myPrescriptionsTableView->GetItemParent(item);
    }

    // Context menu
    wxMenu menu;
    menu.SetTitle(_("Contextual Menu")); // it doesn't appear
    menu.Append(wxID_HIGHEST+0, _("Print Label"));
    menu.Append(wxID_HIGHEST+1, _("Edit Comment"));
    menu.Append(wxID_HIGHEST+2, _("Delete Package and Comment"));
//    menu.AppendSeparator();
//    menu.Append(wxID_CANCEL, _("Cancel"));
    const int rc = GetPopupMenuSelectionFromUser(menu, wxDefaultPosition);
    if (rc == wxID_NONE ||
        rc == wxID_CANCEL)
    {
        return;
    }

    //std::clog << "You have selected " << rc - wxID_HIGHEST << std::endl;

    switch (rc - wxID_HIGHEST) {
        case 0:
            printMedicineLabel();
            break;
            
        case 1: // Edit comment
        {
            // If the comment is not there, add it
            int childrenCount = myPrescriptionsTableView->GetChildrenCount(medItem);
#ifndef NDEBUG
            std::clog << "med has " << childrenCount << " children" << std::endl;
#endif
            if (childrenCount == 1) // only GTIN so far
                myPrescriptionsTableView->AppendItem(medItem, wxEmptyString);

            wxTreeItemIdValue cookie; // For GetNextChild()
            wxTreeItemId gtinItem = myPrescriptionsTableView->GetFirstChild(medItem, cookie);
            wxTreeItemId commentItem = myPrescriptionsTableView->GetNextChild(medItem, cookie);
            (void)myPrescriptionsTableView->EditLabel(commentItem);
        }
            break;
            
        case 2: // Delete
            removeItemFromPrescription();
            break;
            
        default:
            break;
    }
}

// 1380
void MainWindow::OnNewPrescription( wxCommandEvent& event )
{
    setOperatorID();
    mPrescriptionsCart[0].clearCart();
    myPrescriptionsTableView_reloadData(0);
    possibleToOverwrite = false;
    modifiedPrescription = false;
    //updateButtons(); // __deprecated
}

// 1396
void MainWindow::OnCheckForInteractions( wxCommandEvent& event )
{
    mInteractionsView->clearMedBasket();
    // Array of PrescriptionItems
    std::vector<PrescriptionItem *> prescriptionMeds = mPrescriptionsCart[0].cart;
    for (auto item : prescriptionMeds) {
        mInteractionsView->pushToMedBasket(item->med);
    }

    updateInteractionsView();

    // 1407
    // Switch tab view
    mUsedDatabase = kdbt_Aips;
    mSearchInteractions = true;
    setSearchState(kss_Title, wxID_BTN_PREPARATION);

    myTabView->ChangeSelection(0); // selectTabViewItemAtIndex
    myToolbar->ToggleTool(wxID_TB_INTERACTIONS, true); // setSelectedItemIdentifier("Interaktionen")
}

// 1439
void MainWindow::OnSavePrescription( wxCommandEvent& event )
{
    savePrescription();
}

// 1444
void MainWindow::OnSendPrescription( wxCommandEvent& event )
{
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
}

// 2917
// tableViewSelectionDidChange
// In amiko-osx this is a delegate function for 3 table views:
//      myTableView, mySectionTitles, myPrescriptionsTableView
// Here in amiko-wx
//      'myTableView' is handled elsewhere, see OnHtmlCellClicked()
//      'mySectionTitles' handled here
//      'myPrescriptionsTableView' handled in OnTreeItemMenu()
void MainWindow::OnSelectionDidChange( wxDataViewEvent& event )
{
    if (event.GetId() != mySectionTitles->GetId()) { // wxID_SECTION_TITLES
        std::cerr << __FUNCTION__ << " line " << __LINE__ << " early return" << std::endl;
        std::clog << "Skip event Id: " << event.GetId() << std::endl;
        std::clog << "wxID_SECTION_TITLES: " << wxID_SECTION_TITLES << std::endl;
        event.Skip();
        return;
    }

    int row = mySectionTitles->GetSelectedRow(); // 0 based

#if 1 //ndef NDEBUG
    // Why do we get row -1 when selecting a result in myTableView in full-text mode ?
    // If we just return here it seems to do the right thing
    if (row == wxNOT_FOUND) {
        std::cerr << __FUNCTION__ << " line " << __LINE__
                << " WARNING: no selection" << std::endl;
        return;
    }
#endif

#if 1 // TODO: tidy up this debug code
    if (row > mListOfSectionIds.size()) {
        std::cerr << __FUNCTION__ << " line " << __LINE__
                << "WARNING: row " << row
                << " > mListOfSectionIds.size() " << mListOfSectionIds.size()
                << std::endl;
        return;
    }
#endif

    // 2973 wxID_SECTION_TITLES

    // 2981
    if (mPrescriptionMode) {
        //NSLog(@"%s row:%ld, %s", __FUNCTION__, row, mListOfSectionIds[row]);
        loadPrescription_andRefreshHistory(mListOfSectionIds[row], false);
    }
    // 2985
    else if (mCurrentSearchState != kss_FullText ||
             mCurrentWebView != kFullTextSearchView)
    {
        // NSString *javaScript = [NSString stringWithFormat:@"window.location.hash='#%s'", mListOfSectionIds[row]];

        // TODO: debug that mListOfSectionIds has valid data

        wxString javaScript = wxString::Format("var hashElement=document.getElementById('%s');if(hashElement) {hashElement.scrollIntoView();}", mListOfSectionIds[row]);

        //std::clog << __FUNCTION__ << " javaScript: " << javaScript << std::endl;
        myWebView->RunScript(javaScript); // stringByEvaluatingJavaScriptFromString
    }
    // 2990
    else {
        // We get here when we select an entry
        // of the list on the right (mySectionTitle) in full-text mode.
        // Update webviewer's content without changing anything else
        wxString contentStr;
        std::vector<Medication *> nullListOfArticles;
        std::map<wxString, std::set<wxString>> nullDict;
        contentStr = mFullTextSearch->tableWithArticles_andRegChaptersDict_andFilter( nullListOfArticles, nullDict, mListOfSectionIds[row]);

        updateFullTextSearchView(contentStr);
    }
    
    // 3003
    //updateButtons(); // __deprecated
}

// 1500
void MainWindow::OnDeletePrescription( wxCommandEvent& event )
{
    if (! mPrescriptionMode)
        return;

    wxMessageDialog *alert = new wxMessageDialog(this,
                          _("Do you really want to delete this prescription?"),
                          _("Delete prescription?"),
                          wxOK | wxCANCEL | wxICON_WARNING);

    alert->Bind(wxEVT_WINDOW_MODAL_DIALOG_CLOSED,
                &MainWindow::deletePrescription_returnCode_contextInfo,
                this);

    alert->ShowWindowModal();
}

// 1519
// Handler for window-modal message box
void MainWindow::deletePrescription_returnCode_contextInfo(wxWindowModalDialogEvent& event)
{
    wxDialog* dialog = event.GetDialog();

    // 1521
    if (dialog->GetReturnCode() == wxID_OK)
    {
        if (mPrescriptionMode) {
            int row = mySectionTitles->GetSelectedRow();
            mPrescriptionAdapter->deletePrescriptionWithName_forPatient( mListOfSectionTitles[row], mPatientSheet->retrievePatient());
            updatePrescriptionHistory();
            mPrescriptionsCart[0].clearCart();
            myPrescriptionsTableView_reloadData(0);
        }
    }

    delete dialog;
}

void MainWindow::OnToolbarAction( wxCommandEvent& event )
{
    // TODO: launchProgressIndicator();
    switchTabs(event.GetId());
}

// 1148
void MainWindow::OnPrintDocument( wxCommandEvent& event )
{
    if (myTabView->GetSelection() == 2)
        printPrescription();
    else
        printTechInfo();
}

// 1537
void MainWindow::OnShowAboutPanel( wxCommandEvent& event )
{
    wxMessageBox(wxString::Format("%s\n%s\nSQLite %s",
                              wxGetOsDescription(),
                              wxVERSION_STRING,
                              SQLITE_VERSION),
    wxString(APP_NAME), wxOK | wxICON_INFORMATION);
}

// 495
void MainWindow::OnPerformFindAction( wxCommandEvent& event )
{
#ifndef NDEBUG
    std::clog << __FUNCTION__ << " event ID: " << event.GetId() << std::endl;
#endif

    switch (event.GetId()) {
        case wxID_FI_FIND_SHOW: // tag 1 = NSTextFinderActionShowFindInterface
            myTextFinder->Show();
            fiSearchField->SetValue(wxEmptyString);
            fiSearchField->SetBackgroundColour(fiSearchFieldBGColor);
            fiSearchField->SetFocus();
            fiSearchCount->Hide(); // No need to show the count when the string is empty
            fiSizer->Layout();
            break;
            
        case wxID_FI_FIND_NEXT:     // tag 2 = NSTextFinderActionNextMatch
        case wxID_FI_FIND_PREVIOUS: // tag 3 = NSTextFinderActionPreviousMatch
            if (myTextFinder->IsShown() && m_findCount > 0) {
                int flags = wxWEBVIEW_FIND_HIGHLIGHT_RESULT;
                if (event.GetId() == wxID_FI_FIND_PREVIOUS)
                    flags |= wxWEBVIEW_FIND_BACKWARDS;

                long currentMatch = myWebView->Find(fiSearchField->GetValue(), flags);
                fiSearchCount->SetLabel(wxString::Format(wxT("%li/%i"), currentMatch+1, m_findCount));
                fiSizer->Layout();
#ifndef NDEBUG
                std::clog << __FUNCTION__ << " current match: " << currentMatch << std::endl;
#endif
            }
            break;

            
        case wxID_FI_FIND_DONE: // tag 11 = NSTextFinderActionHideFindInterface
            if (myTextFinder->IsShown()) {
                myTextFinder->Hide();
                fiSizer->Layout();
            }
            break;
                
        default:
            // tag 7 = NSTextFinderActionSetSearchString
            break;
    }
}

// 1168
void MainWindow::OnUpdateAipsDatabase( wxCommandEvent& event )
{
    // TODO: check if there is an active Internet connection
    //std::clog << wxDialupManager::IsOnline() << std::endl;

    //wxBusyCursor wait;
    wxBusyInfo info(_("Downloading latest DB files, please wait..."), this);

    const char * languageCode = UTI::appLanguage();

    downloadTextFileWithName(wxString::Format("amiko_report_%s.html", languageCode));

    downloadFileWithName(wxString::Format("drug_interactions_csv_%s.zip", languageCode));
    downloadFileWithName(wxString::Format("amiko_frequency_%s.db.zip", languageCode));
    downloadFileWithName(wxString::Format("amiko_db_full_idx_%s.zip", languageCode));
    
    finishedDownloading();
}

// 1192
void MainWindow::OnLoadAipsDatabase( wxCommandEvent& event )
{
    wxString wildCard = wxString::Format("DB %s (*.db;*.DB)|*.db;*.DB|"
                                         "HTML %s (*.html)|*.html|"
                                         "CSV %s (*.csv)|*.csv",
                                         _("files"), _("files"), _("files"));
    wxFileDialog openDlgPanel(this,
                              _("Please select DB file"),
                              wxEmptyString,
                              wxEmptyString,
                              wildCard,
                              wxFD_OPEN | wxFD_FILE_MUST_EXIST, // no wxFD_MULTIPLE
                              wxDefaultPosition);
    
    if (openDlgPanel.ShowModal() != wxID_OK)
        return;

    // Grab reference to what has been selected
    wxFileName fileURL = openDlgPanel.GetPath();
    wxString fileName = fileURL.GetFullName(); // lastPathComponent

    // Check if file is in the list of allowed files
    if (!UTI::checkFileIsAllowed(fileName))
        return;

    // Get documents directory
    wxFileName documentsDir( UTI::documentsDirectory(), wxEmptyString); // important for directory names: specify empty filename
    documentsDir.SetFullName(fileName);
    wxString dstPath = documentsDir.GetFullPath();

    // Extract source file path
    wxString srcPath = fileURL.GetFullPath();

    if (!srcPath.IsEmpty() &&
        !dstPath.IsEmpty())
    {
        // If it exists, remove old file
        if (wxFileName::Exists(dstPath))
            wxRemoveFile(dstPath);

        // Copy new file and notify (call the notification handler directly)
        if (wxCopyFile(srcPath, dstPath))
            finishedDownloading(); // postNotificationName:@"MLDidFinishLoading"
    }
}

// 1871 and 3400
void MainWindow::OnExportWordListSearchResults( wxCommandEvent& event )
{
    //std::clog << __PRETTY_FUNCTION__ << std::endl;
    
    mUsedDatabase = kdbt_Aips;
    stopProgressIndicator();

#ifdef CSV_EXPORT_RESTORES_PREVIOUS_STATE
    int savedState = mCurrentSearchState;
    int savedTabIndex = myTabView->GetSelection(); // indexOfTabViewItem:[ myTabView selectedTabViewItem]];
#endif

    setSearchState(kss_FullText, wxID_BTN_FULL_TEXT);

    // 3400
    //exportWordListSearchResults(item);
    wxArrayString keywords = csvGetInputListFromFile();
    if (keywords.size() == 0)
        return;  // canceled
    
    // 3410
    // CSV header
    wxArrayString csvHeader;
    csvHeader.Add( _("Search Term from Uploaded file"));
    csvHeader.Add( _("Active Substance")),
    csvHeader.Add( _("Brand-Name of the drug")),
    csvHeader.Add( _("ATC-Code")),
    csvHeader.Add( _("Chapter name")),
    csvHeader.Add( _("Sentence that contains the word")),
    csvHeader.Add( _("Link to the online reference"));
    //csv = [[csvHeader componentsJoinedByString:CSV_SEPARATOR] mutableCopy];
    csv.Empty();
    for (auto s : csvHeader) {
        csv.Append(s).Append(CSV_SEPARATOR);
    }
    
    // TODO: strip off last CSV_SEPARATOR
    
    std::clog << "csv: " << csv << std::endl;

    // TODO: Run in a separate thread
    csvProcessKeywords(keywords);
    csvOutputResult();

    // 1884
#ifdef CSV_EXPORT_RESTORES_PREVIOUS_STATE
    setSearchState(savedState, savedState);
    myTabView->ChangeSelection( savedTabIndex);
#endif
}

// 1415
// amiko-osx: OnLoadPrescription -> loadPrescriptionFromFile
// amiko-ios: -> importFromURL
// generika: -> importReceiptFromURL
void MainWindow::OnLoadPrescription( wxCommandEvent& event )
{
    //std::clog << __PRETTY_FUNCTION__ << std::endl;
    
    // Create a file open dialog class
    wxFileDialog openDlgPanel(this,
                              _("Select AMK file"),
                              wxEmptyString,
                              wxEmptyString,
                              _("AMK files (*.amk)|*.amk"),
                              wxFD_OPEN | wxFD_FILE_MUST_EXIST, // no wxFD_MULTIPLE
                              wxDefaultPosition);
    
    if (openDlgPanel.ShowModal() != wxID_OK)
    {
        //std::clog << __FUNCTION__ << __LINE__ << " Canceled" << std::endl;
        return;
    }

#if 0
    //openDlgPanel.GetFilenames(fileTypesArray);
    wxArrayString fileTypesArray;
    openDlgPanel.GetPaths(fileTypesArray); // full path, not just the filename
#endif

    // 1431
    wxString fileURL = openDlgPanel.GetPath();
    mPrescriptionAdapter->loadPrescriptionFromFile(fileURL);
    mPrescriptionsCart[0].cart = mPrescriptionAdapter->cart;
    updatePrescriptionsView();
}

// 1229
// TODO: consolidate with OnSearchPatient()
void MainWindow::OnManagePatients( wxCommandEvent& event )
{
    if (!mPatientSheet)
        mPatientSheet = new PatientSheet(this);

    mPatientSheet->ShowWindowModal();
}

// 1237
void MainWindow::OnSetOperatorIdentity( wxCommandEvent& event )
{
    if (!mOperatorIDSheet)
        mOperatorIDSheet = new OperatorIDSheet(this);

    mOperatorIDSheet->ShowWindowModal();
}

void MainWindow::OnSmartCardTick( wxTimerEvent& event )
{
    if (!healthCard)
        return;

    bool scChanged = healthCard->detectChanges();
#ifndef NDEBUG
    if (scChanged) {
        std::clog << __FUNCTION__ << " Inserted card: "
        << healthCard->familyName << " "
        << healthCard->givenName << std::endl;
    }
#endif
}

void MainWindow::cellProcessing(int row)
{
#ifndef NDEBUG
    std::clog << __FUNCTION__ << ", row: " << row << std::endl;
#endif
    {
        // 2789
        ItemCellView* cellView = ItemCellView::Instance();
        
        // 2792
        cellView->selectedMedi = doArray[row];
        cellView->packagesStr = doArray[row]->subTitle;
        if (mCurrentSearchState == kss_Title)
            cellView->showContextualMenu = true;
        else
            cellView->showContextualMenu = false;
        
        // TODO: cellView->packagesView.Refresh()
        
        // 2801
        // See TableViewDelegate::OnGetItem()
    }

    // We didn't click on the favourites star

    // 2936
    if ( mCurrentSearchState != kss_FullText) {
        // Search in AIPS DB or Interactions DB
        long mId = doArray[row]->medId;
        // Get medi
        mMed = mDb->getMediWithId(mId);

        hideTextFinder();

        // 2946
        if (!mSearchInteractions) {
            updateExpertInfoView(wxEmptyString); // Fachinfo HTML
        }
        else {
            pushToMedBasket(mMed);
            updateInteractionsView(); // Interactions HTML
        }
    }
    // 2953
    else {
        // Search in full text search DB
        wxString hashId = doArray[row]->hashId;
        // Get entry
        mFullTextEntry = mFullTextDb->searchHash(hashId);

        hideTextFinder();
        
        wxArrayString listOfRegnrs = mFullTextEntry->getRegnrsAsArray();
        MEDICATION_RESULTS listOfArticles = mDb->searchRegnrsFromList(listOfRegnrs);

        std::map<wxString, std::set<wxString>> dict = mFullTextEntry->regChaptersDict;
        
        mFullTextContentStr = mFullTextSearch->tableWithArticles_andRegChaptersDict_andFilter( listOfArticles, dict, wxEmptyString);

        mCurrentWebView = kFullTextSearchView;
        updateFullTextSearchView(mFullTextContentStr);
    }
}

// Handler for EVT_LISTBOX
// TODO: maybe now it can be removed
void MainWindow::OnLboxSelect(wxCommandEvent& event)
{
#ifndef NDEBUG
    std::clog << __FUNCTION__ << " row " << event.GetInt() << std::endl;
#endif
}

// Handler for EVT_LISTBOX_DCLICK
void MainWindow::OnLboxDClick(wxCommandEvent& event)
{
#ifndef NDEBUG
    std::cerr << "Listbox item " << event.GetInt() << " double clicked." << std::endl;
#endif
}

// Handler for EVT_HTML_CELL_HOVER
void MainWindow::OnHtmlCellHover(wxHtmlCellEvent &event)
{
#if 0 //ndef NDEBUG
    std::cerr << "Mouse over cell " << event.GetCell()
    << ", cell ID " << event.GetCell()->GetId()
    << ", cell link " << event.GetCell()->GetLink()
    << ", at " << event.GetPoint().x << ";" << event.GetPoint().y
    << std::endl;
#endif
}

// 2917
// Handler for EVT_HTML_CELL_CLICKED
// See MLMainWindowController.m:2783 tableView:viewForTableColumn:row:
// See MLMainWindowController.m:2917 tableViewSelectionDidChange
void MainWindow::OnHtmlCellClicked(wxHtmlCellEvent &event)
{
    if (event.GetId() != myTableView->GetId()) { // wxID_MY_TV
        std::clog << "Skip event Id " << event.GetId() << std::endl;
        event.Skip();
        return;
    }

    wxPoint absPosCell = event.GetCell()->GetAbsPos();
    wxPoint eventPoint = event.GetPoint();
    wxPoint calculatedPos = absPosCell + eventPoint;

    bool clickedOnStar = (calculatedPos.x < 20 &&
                          calculatedPos.y < 20);

    // Finally worked out how to detect the row number:
    unsigned long clickedRow;
    event.GetCell()->GetRootCell()->GetId().ToULong(&clickedRow);
 
#ifndef NDEBUG
    std::clog << std::endl << __FUNCTION__
    << ", (old) row:" << myTableView->GetSelection()
    << ", OnStar:" << clickedOnStar
    //<< ", event int:" << event.GetInt() // always 0
    //<< ", cell ID:<" << cell->GetId() << ">" // always empty
    << ", (new) row:<" << clickedRow << ">"
    << std::endl;
#endif

    // 3003
    //updateButtons(); // __deprecated
    
    if (clickedOnStar)
    {
        tappedOnStar(clickedRow);
        myTableView->RefreshRow(clickedRow);
        // Don't skip the event, so that OnHtmlLinkClicked() won't be called.
        return;
    }

    cellProcessing(clickedRow);

    // Skip the event, so that OnHtmlLinkClicked() will be called.
    event.Skip();
}

// amiko-osx MLItemCellView.m:148 tableViewSelectionDidChange
// amiko-ios MLViewController.m:3146 myLongPressMethod
void MainWindow::OnHtmlLinkClicked(wxHtmlLinkEvent& event)
{
    wxArrayString idx = wxSplit(event.GetLinkInfo().GetHref(), '_');
    int rowIndex = wxAtoi(idx[0]);
    int packageIndex = wxAtoi(idx[1]);
    
    // Force the update of cell selection
    myTableView->SetSelection(rowIndex);

#ifndef NDEBUG
    wxHtmlContainerCell* cellParent = event.GetLinkInfo().GetHtmlCell()->GetParent();

    std::clog << __FUNCTION__
    << ", event Id: " << event.GetId()
    //<< ", cell parent Id: " << cellParent->GetId() // crash Lubuntu running from terminal (ok from NetBeans IDE)
    << ", int: " << event.GetInt()
    << ", HTML cell " << event.GetLinkInfo().GetHtmlCell()
    << ", HTML cell ID " << event.GetLinkInfo().GetHtmlCell()->GetId()
    << ", package at index: <" << packageIndex << ">"
    << ", selected row: " << rowIndex
    << std::endl;
#endif

    DataObject *dobj = myTableView->searchRes[rowIndex];
    ItemCellView::Instance()->tableViewSelectionDidChange(rowIndex, packageIndex, dobj);

    event.Skip();
}

// 2783
// amiko-osx tableView:viewForTableColumn:row:
void MainWindow::mySectionTitles_reloadData()
{
    mySectionTitles->DeleteAllItems(); // OnSelectionDidChange() will probably be called
    int n = mListOfSectionTitles.size();
#ifndef NDEBUG
    std::clog << __FUNCTION__ << " line " << __LINE__
    << " tab: " << myTabView->GetSelection()
    << " n: " << n
    << std::endl;
#endif
    wxVector<wxVariant> values;
    for (int i=0; i<n; i++) {
        values.clear();
        values.push_back(wxVariant(mListOfSectionTitles[i]));
        mySectionTitles->AppendItem(values);
    }

    mySectionTitles->Refresh();
    //mySectionTitles->Fit();   // ng
    //GetSizer()->Layout();     // ng
}

// This code is not in amiko-osx
// Here we are not using any "model" for myPrescriptionsTableView
void MainWindow::myPrescriptionsTableView_reloadData(int cartNo)
{
    // Issue #55 Don't do this. Maybe we just deleted the last item
    // an we want to refresh the empty view
//    if (mPrescriptionsCart[cartNo].cart.size() < 1)
//        return;

    wxTreeItemId root = myPrescriptionsTableView->GetRootItem();
    myPrescriptionsTableView->DeleteChildren(root);

    for (PrescriptionItem *item : mPrescriptionsCart[cartNo].cart) {
        wxTreeItemId med =
        myPrescriptionsTableView->AppendItem(root, item->fullPackageInfo);
        myPrescriptionsTableView->SetItemTextColour(med, *wxBLUE);

        myPrescriptionsTableView->AppendItem(med, item->med->atccode);
        
        // If there is a comment, add it
        if (item->comment.length() > 0)
            myPrescriptionsTableView->AppendItem(med, item->comment);
    }

    myPrescriptionsTableView->ExpandAll();
    myPrescriptionsTableView->Refresh();
}

// 2889
// tableView:acceptDrop:row:dropOperation:
void MainWindow::OnDropFiles(wxDropFilesEvent& event)
{
    if (event.GetNumberOfFiles() < 1)
        return;

    // TODO: check file extension == amk
    
    // 2900
    loadPrescription_andRefreshHistory(event.GetFiles()[0], true);
}

void MainWindow::OnAmkFilesUpdated(wxCommandEvent& event) {
    updatePrescriptionHistory();
}
