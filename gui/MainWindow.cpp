
#include <vector>

#include <wx/wx.h>
#include <wx/stdpaths.h>
#include <wx/busyinfo.h>

#include <wx/wfstream.h>
#include <wx/txtstrm.h>

#include <sqlite3.h>

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
#include "OperatorIDSheet.h"
#include "FullTextSearch.hpp"

#include "../res/xpm/CoMed.xpm"

// Database types
enum {
    kdbt_Aips=0, kdbt_Hospital=1, kdbt_Favorites=2, kdbt_Interactions=4
};

// Search states
enum {
    kss_Title=0, kss_Author=1, kss_AtcCode=2, kss_RegNr=3, kss_Therapy=4, kss_WebView=5, kss_FullText=6
};

// Webview
enum {
    kExpertInfoView=0, kFullTextSearchView=1, kInteractionsCartView=2
};


// 106
static int mCurrentSearchState = kss_Title;
static int mCurrentWebView = kExpertInfoView;
static wxString mCurrentSearchKey;

// Events not processed by MainWindow will, by default, be handled by MainWindowBase
BEGIN_EVENT_TABLE(MainWindow, MainWindowBase)
    EVT_HTML_LINK_CLICKED(wxID_ANY, MainWindow::OnHtmlLinkClicked)
    EVT_HTML_CELL_CLICKED(wxID_ANY, MainWindow::OnHtmlCellClicked)
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
, mDb(nullptr)
, mFullTextDb(nullptr)
, mInteractions(nullptr)
, m_findCount(wxNOT_FOUND)
{
	std::cerr << __PRETTY_FUNCTION__ << " APP_NAME " << APP_NAME << std::endl;
    if (wxString(APP_NAME) == "CoMed") {
        m_toolAbout->SetLabel("CoMed Desitin");
        m_tbMain->SetToolNormalBitmap(wxID_ABOUT, wxBitmap( CoMed_xpm ));
    }

    mySectionTitles->AppendTextColumn( "Sections" );

    myTextFinder->Hide();
    fiSearchFieldBGColor = myTextFinder->GetBackgroundColour();
    //fiSearchCount->SetWindowVariant(wxWINDOW_VARIANT_MINI);
    fiSizer->Layout();

    SetTitle(APP_NAME + wxString(" Desitin"));
    const wxEventTable *et = GetEventTable();

    //fadeInAndShow(); // Too early here because we are not doing the fade-in (yet)

    // TODO: Register applications defaults if necessary
    // TODO: Start timer to check if database needs to be updated (checks every hour)

    // 275
    // Open AIPS database
    bool ok = openSQLiteDatabase();
#ifndef NDEBUG
    if (ok)
        std::cerr << "Number of records in AIPS database: "
        << mDb->getNumRecords() << std::endl;
#endif

    // Open full-text database
    ok = openFullTextDatabase();
#ifndef NDEBUG
    if (ok)
        std::cerr << "Number of records in fulltext database: "
        << mFullTextDb->getNumRecords() << std::endl;
#endif

    // 286
    // Open drug interactions csv file
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
    // TODO: Initialize interactions cart

    // 295
    // TODO: Create bridge between JScript and ObjC

    // 298
    // TODO: Initialize full text search

    // 301
    // TODO: Initialize all three prescription baskets

    // 306
    mPrescriptionAdapter = new PrescriptionsAdapter;

    // 308
    // TODO: Register drag and drop on prescription table view

    // 315
    // TODO: Initialize webview
    myWebView->SetPage("<html><head></head><body></body></html>", wxString());
    //myWebView->Fit();

    // 321
    // TODO: Initialize favorites (articles + full text entries)

    // 327
    // Set default database
    //mUsedDatabase = kdbt_Aips; // already done above

    // 331
    // Set search state
    setSearchState(kss_Title);

    // 381
    // TODO:
    //healthCard = new HealthCard;
}

// 483
void MainWindow::hideTextFinder()
{
    std::clog << __PRETTY_FUNCTION__ << "TODO" << std::endl;

#if 0
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

// 741
void MainWindow::updatePrescriptionHistory()
{
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;

    if (!mPrescriptionMode) {
#ifndef NDEBUG
        std::clog << __FUNCTION__ << " not prescription mode" << std::endl;
#endif
        return;
    }

    // Extract section IDs
    if (!mMed)
        return;

    if (mMed->sectionIds) {
        wxArrayString listOfPrescriptions = mPrescriptionAdapter->listOfPrescriptionURLsForPatient( mPatientSheet->retrievePatient());
        mListOfSectionIds = listOfPrescriptions;  // array of full paths
    }

    // Extract section titles
    if (mMed->sectionTitles) {
        Patient *p = mPatientSheet->retrievePatient();
        wxArrayString listOfPrescriptions = mPrescriptionAdapter->listOfPrescriptionsForPatient(p);
        mListOfSectionTitles = listOfPrescriptions; // array of just the basenames
    }

    // TODO:
    //mySectionTitles->reloadData();
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

    if (mUsedDatabase == kdbt_Aips)
        searchResults = searchAnyDatabasesWith(mCurrentSearchKey);
    else if (mUsedDatabase == kdbt_Favorites)
        searchResults = retrieveAllFavorites();
}

// 858
void MainWindow::resetDataInTableView()
{
	std::cerr << __PRETTY_FUNCTION__ << std::endl;

	// Reset search state
    setSearchState(kss_Title);

    mCurrentSearchKey = "";
    searchResults = searchAnyDatabasesWith(mCurrentSearchKey);  // FIXME:

    if (searchResults.size()>0) {
        updateTableView();

        myTableView->SetItemCount(searchResults.size()); // reloadData
        myTableView->SetSelection(0); // scrollRectToVisible
    }
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
            // TODO:
            myTabView->ChangeSelection(0); // 1800
            break;

        case wxID_TB_FAVORITES:
            //std::clog << "Favorites" << std::endl;
            mUsedDatabase = kdbt_Favorites;
            mSearchInteractions = false;
            mPrescriptionMode = false;
            // TODO:
            myTabView->ChangeSelection(0); // 1840
            break;

        case wxID_TB_INTERACTIONS:
            //std::clog << "Interactions" << std::endl;
            mUsedDatabase = kdbt_Aips;
            mSearchInteractions = true;
            mPrescriptionMode = false;
            // TODO:
            myTabView->ChangeSelection(0); // 1855
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

            myTabView->ChangeSelection(2); // 1868
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
}

// 1897
std::vector<Medication *> MainWindow::retrieveAllFavorites()
{
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
    std::vector<Medication *> temp;
    return temp;
}

// 1967
void MainWindow::setSearchState(int searchState)
{
	std::cerr << __PRETTY_FUNCTION__ << " " << searchState << std::endl;
    switch (searchState)
    {
        case kss_Title:
            mySearchField->SetValue("");
            mCurrentSearchState = kss_Title;
            mySearchField->SetDescriptiveText(wxString::Format("%s %s", _("Search"), _("Preparation")));
            break;

        case kss_Author:
            mySearchField->SetValue("");
            mCurrentSearchState = kss_Author;
            mySearchField->SetDescriptiveText(wxString::Format("%s %s", _("Search"), _("Owner")));
            break;

        case kss_AtcCode:
            mySearchField->SetValue("");
            mCurrentSearchState = kss_AtcCode;
            mySearchField->SetDescriptiveText(wxString::Format("%s %s", _("Search"), _("ATC Code")));
            break;

        case kss_RegNr:
            mySearchField->SetValue("");
            mCurrentSearchState = kss_RegNr;
            mySearchField->SetDescriptiveText(wxString::Format("%s %s", _("Search"), _("Reg. No")));
            break;

        case kss_Therapy:
            mySearchField->SetValue("");
            mCurrentSearchState = kss_Therapy;
            mySearchField->SetDescriptiveText(wxString::Format("%s %s", _("Search"), _("Therapy")));
            break;

        case kss_WebView:
            // Hide textfinder
            hideTextFinder();
            // NOTE: Commented out because we're using SHCWebView now (02.03.2015)
            /*
            mySearchField->SetValue("");
            mCurrentSearchState = kWebView;
            mySearchField->SetDescriptiveText(wxString::Format("%s %s", _("Search"), _("in Fachinformation"))); // fr: @"Notice Infopro"
            */
            break;

        case kss_FullText:
            mySearchField->SetValue("");
            mCurrentSearchState = kss_FullText;
            mySearchField->SetDescriptiveText(wxString::Format("%s %s", _("Search"), _("Full Text")));
            break;
    }

    mCurrentSearchKey = "";
    mCurrentSearchState = searchState;
    std::cerr << __PRETTY_FUNCTION__ << " Ended" << std::endl;
}

// 2029
std::vector<Medication *> MainWindow::searchAnyDatabasesWith(wxString searchQuery)
{
#ifndef NDEBUG
    std::clog << __FUNCTION__ << ", searchQuery <" << searchQuery.ToStdString() << ">"  << std::endl;
#endif

    ALL_RESULTS searchResObsolete;
    std::vector<Medication *> searchRes;

    if (mCurrentSearchState == kss_Title)
        searchRes = mDb->searchTitle(searchQuery);  // array of Medication
    else if (mCurrentSearchState == kss_Author)
        searchResObsolete = mDb->searchAuthor(searchQuery);
    else if (mCurrentSearchState == kss_AtcCode)
        searchResObsolete = mDb->searchATCCode(searchQuery);
    else if (mCurrentSearchState == kss_RegNr)
        searchResObsolete = mDb->searchRegNr(searchQuery);
    else if (mCurrentSearchState == kss_Therapy)
        searchResObsolete = mDb->searchApplication(searchQuery);
    else if (mCurrentSearchState == kss_FullText)
    {
        if (searchQuery.length() > 2)
            searchResObsolete = mFullTextDb->searchKeyword(searchQuery); // array of FullTextEntry
    }

    mCurrentSearchKey = searchQuery;

    return searchRes;
}

// 2064
void MainWindow::addTitle_andPackInfo_andMedId(wxString title, wxString packinfo, long medId)
{
	//std::cerr << __PRETTY_FUNCTION__ << std::endl;

	DataObject *m = new DataObject;
    if (title.size() > 0)
        m->title = title;
    else
        m->title = L"Not specified"; // TODO: localize

    if (packinfo.size() > 0) {
        if (!mSearchInteractions)
            m->subTitle = packinfo;
        else {
            // We pass 'atccode' instead, which needs to be unpacked
            // TODO:
        }
    }
    else
        m->subTitle = (char *)"Not specified"; // TODO: localize

    m->medId = medId;
    doArray.push_back(m); // to be obsolete
    myTableView->searchRes.push_back(m);
}

// 2286
void MainWindow::updateTableView()
{
    //std::cerr << __PRETTY_FUNCTION__  << std::endl;
 
    if (searchResults.size() == 0) {
        stopProgressIndicator();
        std::cerr << __FUNCTION__ << " 0 results" << std::endl;
        return;
    }
    
#ifndef NDEBUG
    std::cerr << __FUNCTION__ << " Line " << __LINE__
            << " searchResults.size " << searchResults.size()
            << std::endl;
#endif

    if (doArray.size() > 0)
        doArray.clear(); // to be obsolete

    if (myTableView->searchRes.size() > 0)
        myTableView->searchRes.clear();

#if 0 // TODO:
    if (favoriteKeyData != nil)
        [favoriteKeyData removeAllObjects];
#endif

    // 2298
    if (mCurrentSearchState == kss_Title) {
        if (mUsedDatabase == kdbt_Aips) {

            // 2300
            for (auto m : searchResults) {
                // TODO: [favoriteKeyData addObject:m.regnrs];
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
    }
    // 2321
    else if (mCurrentSearchState == kss_Author) {
        std::clog << __PRETTY_FUNCTION__ << " TODO Author" << std::endl;
    }
    // 2338
    else if (mCurrentSearchState == kss_AtcCode) {
        std::clog << __PRETTY_FUNCTION__ << " TODO AtcCode" << std::endl;
    }
    // 2355
    else if (mCurrentSearchState == kss_RegNr) {
        std::clog << __PRETTY_FUNCTION__ << " TODO RegNr" << std::endl;
    }
    // 2373
    else if (mCurrentSearchState == kss_Therapy) {
        std::clog << __PRETTY_FUNCTION__ << " TODO Therapy" << std::endl;
    }
    // 2391
    else if (mCurrentSearchState == kss_FullText) {
        std::clog << __PRETTY_FUNCTION__ << " TODO FullText" << std::endl;
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
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
}

// 2473
void MainWindow::updateExpertInfoView(wxString anchor)
{
	std::cerr << __PRETTY_FUNCTION__ << std::endl;

	// 2476
    wxString color_Style = wxString::Format("<style type=\"text/css\">%s</style>", UTI::getColorCss());
    
    // 2479
    wxString amiko_Style; // TODO: read it once only and store it, instead of doing this for every fachinfo
    {
        // Load style sheet from file
#ifdef __linux__
        wxFileName f(wxStandardPaths::Get().GetExecutablePath());
        wxString amikoCssPath(f.GetPath());
#else
        // TODO: use GetResourcesDir()
        wxString amikoCssPath = wxStandardPaths::Get().GetUserDataDir();
#endif
        amikoCssPath += wxFILE_SEP_PATH + wxString("amiko_stylesheet.css");

        wxString amikoCss;
        if (wxFileName::Exists(amikoCssPath)) {
            wxFileInputStream input( amikoCssPath );
            wxTextInputStream text(input, wxT("\x09"), wxConvUTF8 );
            while (input.IsOk() && !input.Eof() )
                amikoCss += text.ReadLine();
        }
        else
            amikoCss = mMed->styleStr; // TODO: Unused ?
        
        amiko_Style = wxString::Format("<style type=\"text/css\">%s</style>", amikoCss);
    }
    
    //std::cerr << "amiko_Style: " << amiko_Style << std::endl;
    
    // 2492
    wxString js_Script;
    {
        // TODO: Load JavaScript from file
#ifdef __linux__
        wxFileName f(wxStandardPaths::Get().GetExecutablePath());
        wxString jscriptPath(f.GetPath());
#else
        // TODO: use GetResourcesDir()
        wxString jscriptPath = wxStandardPaths::Get().GetUserDataDir();
#endif
        
        jscriptPath += wxFILE_SEP_PATH + wxString("main_callbacks.js");
        wxString jscriptStr;
        //= [NSString stringWithContentsOfFile:jscriptPath encoding:NSUTF8StringEncoding error:nil];
        if (wxFileName::Exists(jscriptPath)) {
            wxFileInputStream input( jscriptPath );
            wxTextInputStream text(input, wxT("\x09"), wxConvUTF8 );
            while (input.IsOk() && !input.Eof() )
                jscriptStr += text.ReadLine();
        }

        //std::cerr << "jscriptStr: " << jscriptStr << std::endl;

        js_Script = wxString::Format("<script type=\"text/javascript\">%s</script>", jscriptStr);
    }

    // 2502
    // Generate html string
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

    if (mCurrentSearchState == kss_FullText)
    {
        // 2534
        // TODO: wxString keyword = mFullTextEntry->keyword;

        // TODO:
    }

    // 2547
    myWebView->SetPage(htmlStr, wxString());
    //myWebView->Fit();

    // 2553
    if (!mPrescriptionMode) {
        // Extract section ids
        if (mMed->sectionIds)
            mListOfSectionIds = mMed->listOfSectionIds();

        // Extract section titles
        if (mMed->sectionTitles)
            mListOfSectionTitles = mMed->listOfSectionTitles();

        //std::clog << "Line " << __LINE__  << " size " << mListOfSectionTitles.size() << std::endl; // 20

#if 0
        mySectionTitles->reloadData();
#else
        // 2827
        mySectionTitles->DeleteAllItems();
        int n = mListOfSectionTitles.size();
        wxVector<wxVariant> values;
        for (int i=0; i<n; i++) {
            //std::clog << "Line " << i  << ", <" << mListOfSectionTitles[i] << ">" << std::endl;

            values.clear();
            values.push_back(wxVariant(mListOfSectionTitles[i]));
            mySectionTitles->AppendItem(values);
        }
        //mySectionTitles->Fit(); // ng
#endif
    }
}

// 2565
void MainWindow::updateInteractionsView()
{
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
}

// 2589
void MainWindow::updatePrescriptionsView()
{
    myTabView->ChangeSelection(2); // 2592

    // Update date
    wxString placeDate = mPrescriptionAdapter->placeDate;
    if (!placeDate.IsEmpty())
        myPlaceDateField->SetLabel(placeDate);

#if 0    // TODO:
    myPrescriptionsTableView->reloadData();
#endif

    mPrescriptionMode = true;

    // TODO:
    //myToolbar->setSelectedItemIdentifier("Rezept");
}

// 2603
void MainWindow::updateFullTextSearchView(wxString contentStr)
{
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
}

// /////////////////////////////////////////////////////////////////////////////
// 949
void MainWindow::OnSearchNow( wxCommandEvent& event )
{
#ifndef NDEBUG
    std::cerr << "=== " << __FUNCTION__
    << " from ID: " << event.GetId()  // wxID_MY_SEARCH_FIELD
    << " <" << mySearchField->GetValue() << ">"
    << std::endl;
#endif

    wxString searchText = mySearchField->GetValue();
    if (mCurrentSearchState == kss_WebView)
        return;

    while (mSearchInProgress) {
        //[NSThread sleepForTimeInterval:0.005];  // Wait for 5ms
        wxMilliSleep(10);
        wxTheApp->Yield();
    }

    if (!mSearchInProgress) {
        mSearchInProgress = true;
    }

    if (searchText.length() > 0) // TODO: > 2 ?
         searchResults = searchAnyDatabasesWith(searchText);
    else {
         if (mUsedDatabase == kdbt_Favorites)
             searchResults = retrieveAllFavorites();
    }

    // 977
    // Update tableview
    updateTableView();
    
    myTableView->SetItemCount(searchResults.size()); // reloadData
    if (searchResults.size()>0)
        myTableView->SetSelection(0); // scrollRectToVisible

    mSearchInProgress = false;
}

// 997
void MainWindow::OnButtonPressed( wxCommandEvent& event )
{
	std::cerr << __PRETTY_FUNCTION__ << std::endl;
    int prevState = mCurrentSearchState;

    switch (event.GetId()) {
        case wxID_BTN_PREPARATION:
            setSearchState(kss_Title);
            break;

        case wxID_BTN_REGISTRATION_OWNER:
            setSearchState(kss_Author);
            break;

        case wxID_BTN_ACTIVE_SUBSTANCE:
            setSearchState(kss_AtcCode);
            break;

        case wxID_BTN_REGISTATION_NUMBER:
            setSearchState(kss_RegNr);
            break;

        case wxID_BTN_THERAPY:
            setSearchState(kss_Therapy);
            break;

        case wxID_BTN_FULL_TEXT:
            setSearchState(kss_FullText);
            mCurrentWebView = kFullTextSearchView;
            break;
    }

    if (prevState == kss_FullText || mCurrentSearchState == kss_FullText)
        updateSearchResults();

    if (searchResults.size() > 0) {
        updateTableView();

        myTableView->SetItemCount(searchResults.size()); // reloadData
        myTableView->SetSelection(0); // scrollRectToVisible
    }
}

// There is no corresponding code in amiko-osx, because there it's implemented
// by injecting JavaScript code into the webview
// Called when the text changes in the search control
void MainWindow::OnSearchFiNow( wxCommandEvent& event )
{
    wxString find_text = fiSearchField->GetValue();
    
    if (find_text.IsEmpty())
        m_findCount = myWebView->Find(wxEmptyString); // reset the search
    else
        m_findCount = myWebView->Find(find_text, wxWEBVIEW_FIND_HIGHLIGHT_RESULT);

#ifndef NDEBUG
    std::cerr << "=== " << __FUNCTION__
    << " from ID: " << event.GetId() // wxID_FI_SEARCH_FIELD
    << " <" << find_text << ">"
    << " m_findCount " << m_findCount
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

// 2917
// tableViewSelectionDidChange
void MainWindow::OnSelectionDidChange( wxDataViewEvent& event )
{
//    std::clog << __PRETTY_FUNCTION__ << " " << event.GetId() << std::endl;
//    std::clog << "event " << event.GetEventObject() << std::endl;
//    std::clog << "event Id " << event.GetId() << std::endl;
//    std::clog << "Id "<< mySectionTitles->GetId() << std::endl;

    if (event.GetId() != mySectionTitles->GetId()) { // wxID_SECTION_TITLES
        std::clog << "Skip event Id " << event.GetId() << std::endl;
        event.Skip();
        return;
    }

    int row = mySectionTitles->GetSelectedRow(); // 0 based

#if 1 // TODO: tidy up this debug code
    if (row > mListOfSectionIds.size()) {
        std::cerr << __FUNCTION__ << " WARNING: "
                << " row: " << row
                << " > mListOfSectionIds.size(): " << mListOfSectionIds.size()
                << std::endl;
        return;
    }
#endif

    // 2973 wxID_SECTION_TITLES

    // 2981
    if (mPrescriptionMode) {
        //NSLog(@"%s row:%ld, %@", __FUNCTION__, row, mListOfSectionIds[row]);
        // TODO: loadPrescription_andRefreshHistory(mListOfSectionIds[row], false);
    }
    else if (mCurrentSearchState != kss_FullText ||
             mCurrentWebView != kFullTextSearchView)
    {
        // NSString *javaScript = [NSString stringWithFormat:@"window.location.hash='#%@'", mListOfSectionIds[row]];

        // TODO: debug that mListOfSectionIds has valid data

        wxString javaScript = wxString::Format("var hashElement=document.getElementById('%s');if(hashElement) {hashElement.scrollIntoView();}", mListOfSectionIds[row]);

        //std::clog << __FUNCTION__ << " javaScript: " << javaScript.ToStdString() << std::endl;
        myWebView->RunScript(javaScript); // stringByEvaluatingJavaScriptFromString
    }
    else {
        // Update webviewer's content without changing anything else
        wxString contentStr = mFullTextSearch->tableWithArticles_andRegChaptersDict_andFilter( nullptr, nullptr, mListOfSectionIds[row]);
        updateFullTextSearchView(contentStr);
    }
}

void MainWindow::OnToolbarAction( wxCommandEvent& event )
{
    // TODO: launchProgressIndicator();
    switchTabs(event.GetId());
}

// 1148
void MainWindow::OnPrintDocument( wxCommandEvent& event )
{
    std::clog << __PRETTY_FUNCTION__ << " TODO" << event.GetId() << std::endl;
}

// 1537
void MainWindow::OnShowAboutPanel( wxCommandEvent& event )
{
    wxMessageBox(wxString::Format("%s\n%s\nSQLite %s",
             wxGetOsDescription().ToStdString(), wxVERSION_STRING, SQLITE_VERSION),
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
    wxBusyInfo info("Downloading latest DB files, please wait...", this);

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
    std::clog << __PRETTY_FUNCTION__ << std::endl;
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

// 2917
// See tableViewSelectionDidChange
// FIXME: not very reliable, sometimes we have to click more than once for the event to be detected
void MainWindow::OnHtmlCellClicked(wxHtmlCellEvent &event)
{
    if (event.GetId() != myTableView->GetId()) { // wxID_MY_TV
        std::clog << "Skip event Id " << event.GetId() << std::endl;
        event.Skip();
        return;
    }

    int row = myTableView->GetSelection();

    std::clog
        << "Click over cell " << event.GetCell()
        << ", ID " << event.GetCell()->GetId().ToStdString()
        << ", at " << event.GetPoint().x << ";" << event.GetPoint().y
        << ", sel " << row
        << std::endl;

    // 2936
    if ( mCurrentSearchState != kss_FullText) {
        // Search in AIPS DB or Interactions DB
        long mId = doArray[row]->medId;
        // Get medi
        mMed = mDb->getMediWithId(mId);
        // TODO: Hide textfinder

        // 2946
        if (mSearchInteractions==false) {
            updateExpertInfoView(wxEmptyString);
        }
        else {
            pushToMedBasket(mMed);
            updateInteractionsView();
        }
    }

    // if we don't skip the event, OnHtmlLinkClicked won't be called!
    event.Skip();
}

void MainWindow::OnHtmlLinkClicked(wxHtmlLinkEvent& event)
{
    std::clog << __FUNCTION__
    << ", event Id: " << event.GetId()
    << ", HTML cell " << event.GetLinkInfo().GetHtmlCell()
    << ", HTML cell ID " << event.GetLinkInfo().GetHtmlCell()->GetId().ToStdString()
    << ", package at index " << event.GetLinkInfo().GetHref()
    << std::endl;

    //myTableView->RefreshRow(1);
    event.Skip();
}
