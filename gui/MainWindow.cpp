
#include <vector>
#include <iostream>
#include <fstream>

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
#include "FullTextEntry.hpp"
#include "DataStore.hpp"
#include "InteractionsHtmlView.hpp"
#include "PrescriptionItem.hpp"

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
    EVT_LISTBOX(wxID_ANY, MainWindow::OnLboxSelect)
    EVT_LISTBOX_DCLICK(wxID_ANY, MainWindow::OnLboxDClick)
    EVT_HTML_LINK_CLICKED(wxID_ANY, MainWindow::OnHtmlLinkClicked)
    EVT_HTML_CELL_HOVER(wxID_ANY, MainWindow::OnHtmlCellHover)
    EVT_HTML_CELL_CLICKED(wxID_ANY, MainWindow::OnHtmlCellClicked)
    EVT_WEBVIEW_NAVIGATING(wxID_ANY, MainWindow::OnNavigationRequest)
    EVT_WEBVIEW_TITLE_CHANGED(wxID_ANY, MainWindow::OnTitleChanged)
    EVT_WEBVIEW_LOADED(wxID_ANY, MainWindow::OnDocumentLoaded)
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
    fiSizer->Layout();

    SetTitle(APP_NAME + wxString(" Desitin"));
    const wxEventTable *et = GetEventTable();

    //fadeInAndShow(); // Too early here because we are not doing the fade-in (yet)

    // TODO: Register applications defaults if necessary
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
    // TODO: Initialize all three prescription baskets

    // 306
    mPrescriptionAdapter = new PrescriptionsAdapter;

    // 308
    // TODO: Register drag and drop on prescription table view

    // 315
    // TODO: Initialize webview
    myWebView->SetPage("<html><head></head><body></body></html>", wxString()); // loadHTMLString
    //myWebView->Fit();

    // 321
    // TODO: Initialize favorites (articles + full text entries)
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

    // 331
    // Set search state
    setSearchState(kss_Title);

    // 381
    // TODO:
    //healthCard = new HealthCard;
}

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
    else if (mUsedDatabase == kdbt_Favorites){
        searchResults = // TODO:
        retrieveAllFavorites();
    }
}

// 858
void MainWindow::resetDataInTableView()
{
	std::cerr << __PRETTY_FUNCTION__ << std::endl;

	// Reset search state
    setSearchState(kss_Title);

    mCurrentSearchKey = wxEmptyString;
    //searchResults =
    searchAnyDatabasesWith(mCurrentSearchKey);  // it updates searchResults and searchResultsFT

    if (searchResults.size()>0) {
        updateTableView();

        myTableView->SetItemCount(searchResults.size()); // reloadData
        myTableView->SetSelection(0); // scrollRectToVisible
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
            myTableView->Refresh();

            // Switch tab view
            updateExpertInfoView(wxEmptyString);
            myTabView->ChangeSelection(0); // 1800
            break;

        case wxID_TB_FAVORITES:
            std::cerr << __FUNCTION__ << " line " << __LINE__ << " Favorites"<< std::endl;
            mUsedDatabase = kdbt_Favorites;
            mSearchInteractions = false;
            mPrescriptionMode = false;
            
            // 1826
            searchResults = retrieveAllFavorites();
            
            // /1828
            // Update tableview
            updateTableView();

            // 1831
            myTableView->SetItemCount(searchResults.size()); // reloadData
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
            myTabView->ChangeSelection(0); // 1840
            break;

        case wxID_TB_INTERACTIONS:
            //std::clog << "Interactions" << std::endl;
            // 1847
            mUsedDatabase = kdbt_Aips;
            mSearchInteractions = true;
            mPrescriptionMode = false;
            
            // 1850
            stopProgressIndicator();
            setSearchState(kss_Title);
            pushToMedBasket(mMed);
            updateInteractionsView();
            
            // 1854
            // Switch tab view
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
MEDICATION_RESULTS MainWindow::retrieveAllFavorites()
{
    MEDICATION_RESULTS medList;

    // 1905
    if (mCurrentSearchState != kss_FullText) {
        if (mDb) {
            for (auto regnrs : favoriteMedsSet) {
                auto med = mDb->searchRegNr(regnrs);
                if (/*med!=nullptr &&*/ med.size() > 0)
                    medList.push_back(med[0]);
            }
        }
    }
    else {
        if (mFullTextDb) {
#if 1 // TODO:
            std::clog << __PRETTY_FUNCTION__ << " TODO: " << std::endl;
#else
            for (auto hashId : favoriteFTEntrySet) {
                auto entry = mFullTextDb->searchHash(hashId);
                if (entry != nullptr)
                    medList.push_back(entry);
            }
#endif
        }
    }

    return medList;
}

// In Amiko-osx it's "~/Library/Preferences/"
#define FAVORITES_DIR       GetUserDataDir

// In Amiko-osx it's "data" for both
#define FAV_MED_FILE        "FavMed.txt"
#define FAV_FT_ENTRY_FILE   "FavFTEntry.txt"

// 1933
void MainWindow::saveFavorites()
{
    wxString path = wxStandardPaths::Get().FAVORITES_DIR();
    std::clog << __FUNCTION__ << " to dir " << path << std::endl;
    FAVORITES_SET::iterator it;

    {
        wxString path1 = path + wxFILE_SEP_PATH + wxString(FAV_MED_FILE);
        std::ofstream myfile(path1);
        for (it = favoriteMedsSet.begin(); it != favoriteMedsSet.end(); ++it)
            myfile << *it << "\n";

        myfile.close();
    }

    {
        wxString path2 = path + wxFILE_SEP_PATH + wxString(FAV_FT_ENTRY_FILE);
        std::ofstream myfile(path2);
        for (it = favoriteFTEntrySet.begin(); it != favoriteFTEntrySet.end(); ++it)
            myfile << *it << "\n";

        myfile.close();
    }
}

// 1950
void MainWindow::loadFavorites(DataStore *favorites)
{
    wxString path = wxStandardPaths::Get().FAVORITES_DIR();
    std::clog << __FUNCTION__ << " from dir " << path << std::endl;
    std::string line;

    {
        wxString path1 = path + wxFILE_SEP_PATH + wxString(FAV_MED_FILE);
        std::ifstream myfile(path1);
        while ( getline (myfile, line) )
            if (line.length() > 0)
                favorites->favMedsSet.insert(line);

        myfile.close();
    }

    {
        wxString path2 = path + wxFILE_SEP_PATH + wxString(FAV_FT_ENTRY_FILE);
        std::ifstream myfile(path2);
        while ( getline (myfile, line) )
            if (line.length() > 0)
                favorites->favFTEntrySet.insert(line);

        myfile.close();
    }
}

// 1967
void MainWindow::setSearchState(int searchState)
{
	std::cerr << __PRETTY_FUNCTION__ << " " << searchState << std::endl;
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

        case kss_WebView:
            hideTextFinder();

            // NOTE: Commented out because we're using SHCWebView now (02.03.2015)
            /*
            mySearchField->SetValue(wxEmptyString);
            mCurrentSearchState = kWebView;
            mySearchField->SetDescriptiveText(wxString::Format("%s %s", _("Search"), _("in Fachinformation"))); // fr: @"Notice Infopro"
            */
            break;

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
        m->title = L"Not specified"; // "k.A."
    
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
                if (m->regnrs) {
                    favoriteKeyData.Add(m->regnrs);
                    addTitle_andAuthor_andMedId(wxString::FromUTF8(m->title), m->auth, m->medId);
                }
            }
            // 2328
            else if (mUsedDatabase == kdbt_Favorites) {
                if (m->regnrs) {
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
                if (m->regnrs) {
                    favoriteKeyData.Add(m->regnrs);
                    addTitle_andAtcCode_andAtcClass_andMedId(wxString::FromUTF8(m->title), m->atccode, m->atcClass, m->medId);
                }
            }
            // 2345
            else if (mUsedDatabase == kdbt_Favorites) {
                std::clog << __PRETTY_FUNCTION__ << " TODO AtcCode Favorites" << std::endl;
            }
        }
    }
    // 2355
    else if (mCurrentSearchState == kss_RegNr) {
        for (auto m : searchResults) {
            // 2357
            if (mUsedDatabase == kdbt_Aips) {
                if (m->regnrs) {
                    favoriteKeyData.Add(m->regnrs);
                    addTitle_andRegnrs_andAuthor_andMedId(
                            wxString::FromUTF8(m->title),
                            m->regnrs,
                            m->auth, m->medId);
                }
            }
            // 2363
            else if (mUsedDatabase == kdbt_Favorites) {
                std::clog << __PRETTY_FUNCTION__ << " TODO RegNr Favorites" << std::endl;
            }
        }
    }
    // 2373
    else if (mCurrentSearchState == kss_Therapy) {
        for (auto m : searchResults) {
            // 2375
            if (mUsedDatabase == kdbt_Aips) {
                if (m->regnrs) {
                    favoriteKeyData.Add(m->regnrs);
                    addTitle_andApplications_andMedId(m->title, m->application, m->medId);
                }
            }
            // 2381
            else if (mUsedDatabase == kdbt_Favorites) {
                std::clog << __PRETTY_FUNCTION__ << " TODO Therapy Favorites" << std::endl;
            }
        }
    }
    // 2391
    else if (mCurrentSearchState == kss_FullText) {
        std::clog << __PRETTY_FUNCTION__ << " FullText" << std::endl;
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
    wxString str = evt.GetString();
    // str now contains the JSON string set in the JavaScript

#ifndef NDEBUG
    std::clog << __FUNCTION__
    << " Title changed: <" << str << ">\n"
    << "myWebView title: <" << myWebView->GetCurrentTitle() << ">\n";
#endif
    
    wxArrayString msg = wxSplit(str, ',');
    if (msg.size() == 3) {
        // interactions
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
    else if (msg.size() == 4) {
        // Full text search
        // 2447
        wxString ean = msg[2];
        wxString anchor = msg[3];
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
    
    if (!mMed)
        std::clog << __PRETTY_FUNCTION__ << " FIXME: mMed is NULL" << std::endl;

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
        wxString keyword = mFullTextEntry->keyword;
        if (!keyword.IsEmpty()) {
            // Instead of appending like in the Windows version,
            // insert before "</body>"
            wxString jsCode = wxString::Format("highlightText(document.body,'%s')", keyword);
            wxString extraHtmlCode = wxString::Format("<script>%s</script>\n </body>", jsCode);
            htmlStr.Replace("</body>", extraHtmlCode);
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
    
    // With the following implementation, the images are not loaded
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
    //std::clog << __FUNCTION__ << " contentStr: <" << contentStr << ">" << std::endl;

    wxString colorCss = UTI::getColorCss();

    // Load style sheet from file
#ifdef __linux__
    wxFileName f(wxStandardPaths::Get().GetExecutablePath());
    wxString fullTextCssPath(f.GetPath());
#else
    // TODO: use GetResourcesDir()
    wxString fullTextCssPath = wxStandardPaths::Get().GetUserDataDir();
#endif
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

// 3047
void MainWindow::updateButtons()
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

    if (searchText.length() > 0) { // TODO: > 2 ?
        //searchResults =
        searchAnyDatabasesWith(searchText);// it updates searchResults and searchResultsFT
    }
    else {
         if (mUsedDatabase == kdbt_Favorites)
             searchResults = // TODO:
             retrieveAllFavorites();
    }

    // 977
    // Update tableview
    updateTableView();
    
    if (mCurrentSearchState == kss_FullText) {
        myTableView->SetItemCount(searchResultsFT.size()); // reloadData
        if (searchResultsFT.size()>0)
            myTableView->SetSelection(0); // scrollRectToVisible
    }
    else {
        myTableView->SetItemCount(searchResults.size()); // reloadData
        if (searchResults.size()>0)
            myTableView->SetSelection(0); // scrollRectToVisible
    }

    myTableView->Refresh();
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

    updateTableView();

    if (mCurrentSearchState == kss_FullText) {
        myTableView->SetItemCount(searchResultsFT.size()); // reloadData
        if (searchResultsFT.size() > 0)
            myTableView->SetSelection(0); // scrollRectToVisible
    }
    else {
        myTableView->SetItemCount(searchResults.size()); // reloadData
        if (searchResults.size() > 0)
            myTableView->SetSelection(0); // scrollRectToVisible
    }

    myTableView->Refresh();
}

// There is no corresponding code in amiko-osx, because there it's implemented
// by injecting JavaScript code into the webview
// Called when the text changes in the search control
void MainWindow::OnSearchFiNow( wxCommandEvent& event )
{
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

// 2917
// tableViewSelectionDidChange
// In amiko-osx this is a delegate function for 3 table views:
//      myTableView, mySectionTitles, myPrescriptionsTableView
// Here in amiko-wx
//      'myTableView' is handled elsewhere, see OnHtmlCellClicked()
//      'mySectionTitles' handled here
//      'myPrescriptionsTableView' yet to be implemented
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
        std::clog << __PRETTY_FUNCTION__ << " Line " << __LINE__ << " TODO loadPrescription()\n";
        // TODO: loadPrescription_andRefreshHistory(mListOfSectionIds[row], false);
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
    updateButtons();
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
             wxGetOsDescription(), wxVERSION_STRING, SQLITE_VERSION),
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

// Handler for EVT_LISTBOX
void MainWindow::OnLboxSelect(wxCommandEvent& event)
{
#ifndef NDEBUG
    std::cerr << "Listbox selection is now " << event.GetInt() << std::endl;
    //event.Skip();
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

    wxPoint absPosCell = event.GetCell()->GetAbsPos();
    wxPoint eventPoint = event.GetPoint();
    wxPoint calculatedPos = absPosCell + eventPoint;

#if 0 //ndef NDEBUG
    std::clog
        << "Click over cell " << event.GetCell()
        << ", cell ID " << event.GetCell()->GetId()
        << ", cell pos " << absPosCell.x << ";" << absPosCell.y
        << ", at " << event.GetPoint().x << ";" << event.GetPoint().y
        << ", calc pos " << calculatedPos.x << ";" << calculatedPos.y
        << ", sel " << row
        << std::endl;
#endif
    
    if (calculatedPos.x < 20 &&
        calculatedPos.y < 20)
    {
        tappedOnStar(row);
        myTableView->RefreshRow(row);
        return;
    }

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
    
    // 3003
    updateButtons();

    // if we don't skip the event, OnHtmlLinkClicked won't be called!
    event.Skip();
}

// MLItemCellView.m:148 tableViewSelectionDidChange
void MainWindow::OnHtmlLinkClicked(wxHtmlLinkEvent& event)
{
    int packageIndex = wxAtoi(event.GetLinkInfo().GetHref());

#ifndef NDEBUG
    std::clog << __FUNCTION__
    << ", event Id: " << event.GetId()
    << ", HTML cell " << event.GetLinkInfo().GetHtmlCell()
    << ", HTML cell ID " << event.GetLinkInfo().GetHtmlCell()->GetId()
    << ", package at index: <" << packageIndex << ">"
    << std::endl;
#endif

    // TODO: find the row number
    int row = myTableView->GetSelection(); // TODO: this is not reliable

    // Popup menu to add medicine to any of 3 prescription carts

    // 155
    wxMenu menu;
    menu.SetTitle(_("Contextual Menu")); // it doesn't appear
    
    // 156
    DataObject *dobj = myTableView->searchRes[row];
    wxArrayString listOfPackages = wxSplit(wxString(dobj->subTitle), '\n');

    // Validate index
    if (packageIndex >= listOfPackages.size()) {
        std::clog << __FUNCTION__ << " Select cell first" << std::endl;
        return;
    }

    wxString selectedPackage = listOfPackages[packageIndex];
    
    menu.Append(wxID_HIGHEST+0, wxString::Format("%s", selectedPackage));
    menu.Append(wxID_HIGHEST+1, _("Prescription"));
    // TODO: maybe add 2 more prescription carts

    const int rc = GetPopupMenuSelectionFromUser(menu, wxDefaultPosition);
    if ( rc != wxID_NONE )
    {
        std::clog << "You have selected \"%d\"" << rc - wxID_HIGHEST << std::endl;
        selectBasket(1);
    }

    event.Skip();
}

// MLItemCellView.m:179
void MainWindow::selectBasket(int cartNumber)
{
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
    
#if 0
    // MLItemCellView.h:32
    Medication *selectedMedi = new Medication;
    
    // 192
    PrescriptionItem *item = new PrescriptionItem;
    
    // 196
    // Extract EAN/GTIN
    Medication *m = getShortMediWithId(selectedMedi->medId);
    
    // TODO
    
    // TODO: (not in amiko-osx) deallocate objects
    delete item;
#endif
}

void MainWindow::mySectionTitles_reloadData()
{
    std::clog << __PRETTY_FUNCTION__ << std::endl;

    mySectionTitles->DeleteAllItems(); // OnSelectionDidChange() will probably be called
    int n = mListOfSectionTitles.size();
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
