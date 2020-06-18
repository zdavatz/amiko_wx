
#include <vector>

#include <wx/wx.h>
#include <wx/stdpaths.h>
//#include <wx/dialup.h>

#include <sqlite3.h>

#include "MainWindow.h"

#include "FullTextDBAdapter.hpp"
#include "customURLConnection.hpp"
#include "DBAdapter.hpp"
#include "InteractionsAdapter.hpp"
#include "Utilities.hpp"

#include "../res/xpm/CoMed.xpm"

// Database types
enum {
    kdbt_Aips=0, kdbt_Hospital=1, kdbt_Favorites=2, kdbt_Interactions=4
};

// Search states
enum {
    kss_Title=0, kss_Author=1, kss_AtcCode=2, kss_RegNr=3, kss_Therapy=4, kss_WebView=5, kss_FullText=6
};

// 106
static int mCurrentSearchState = kss_Title;
static wxString mCurrentSearchKey;

MainWindow::MainWindow( wxWindow* parent )
: MainWindowBase( parent )
, mUsedDatabase(kdbt_Aips)
, mSearchInteractions(false)
, mPrescriptionMode(false)
{
    if (APP_NAME == "CoMed") {
        m_toolAbout->SetLabel("CoMed Desitin");
        m_tbMain->SetToolNormalBitmap(wxID_ABOUT, wxBitmap( CoMed_xpm ));
    }
    
    SetTitle(APP_NAME);
    
    //fadeInAndShow(); // Too early here because we are not doing the fade-in (yet)

    // TODO: Register applications defaults if necessary
    // TODO: Start timer to check if database needs to be updatd (checks every hour)

    // Open sqlite database
    openSQLiteDatabase();
#ifndef NDEBUG
    std::clog << "Number of records in AIPS database: " << mDb->getNumRecords() << std::endl;
#endif
    
    // Open fulltext database
    openFullTextDatabase();
#ifndef NDEBUG
    std::clog << "Number of records in fulltext database: " << mFullTextDb->getNumRecords() << std::endl;
#endif
        
    // Open drug interactions csv file
    openInteractionsCsvFile();
#ifndef NDEBUG
    std::clog << "Number of records in interaction file: " << mInteractions->getNumInteractions() << std::endl;
#endif
    
    fadeInAndShow();


    fiPanel->SetPage("<html><body>Fachinfo</body></html>");
    fiPanel->Fit();
}

// 483
void MainWindow::hideTextFinder()
{
    std::clog << __PRETTY_FUNCTION__  << "TODO" << std::endl;

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
    resetDataInTableView();
}

// 591
void MainWindow::openInteractionsCsvFile()
{
    mInteractions = new InteractionsAdapter;
    bool ok = mInteractions->openInteractionsCsvFile( wxString::Format("drug_interactions_csv_%s", UTI::appLanguage()));
}

// 605
void MainWindow::openSQLiteDatabase()
{
    mDb = new DBAdapter;
    bool ok = mDb->openDatabase( wxString::Format("amiko_db_full_idx_%s", UTI::appLanguage()));
}

// 621
void MainWindow::openFullTextDatabase()
{
    mFullTextDb = new FullTextDBAdapter;
    bool ok = mFullTextDb->openDatabase( wxString::Format("amiko_frequency_%s", UTI::appLanguage()));
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
    // Reset search state
    setSearchState(kss_Title);
    
    mCurrentSearchKey = "";
    searchResults = searchAnyDatabasesWith(mCurrentSearchKey);

    //myTableView->reloadData();
}

// 1755
// Switch app state
void MainWindow::switchTabs(int item)
{
    switch (item) {
        case wxID_TB_COMPENDIUM:
            std::clog << "AIPS Database" << std::endl;
            mUsedDatabase = kdbt_Aips;
            mSearchInteractions = false;
            mPrescriptionMode = false;
            // TODO
            break;
            
        case wxID_TB_FAVORITES:
            std::clog << "Favorites" << std::endl;
            mUsedDatabase = kdbt_Favorites;
            mSearchInteractions = false;
            mPrescriptionMode = false;
            // TODO:
            break;
            
        case wxID_TB_INTERACTIONS:
            std::clog << "Interactions" << std::endl;
            mUsedDatabase = kdbt_Aips;
            mSearchInteractions = true;
            mPrescriptionMode = false;
            // TODO:
            break;
            
        case wxID_TB_PRESCRIPTION:
            std::clog << "Rezept" << std::endl;
            mUsedDatabase = kdbt_Aips;
            mSearchInteractions = false;
            mPrescriptionMode = true;
            // TODO:
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
MYRESULTS MainWindow::retrieveAllFavorites()
{
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
}

// 1967
void MainWindow::setSearchState(int searchState)
{
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
}

// 2029
MYRESULTS MainWindow::searchAnyDatabasesWith(wxString searchQuery)
{
    std::clog << __FUNCTION__ << ", searchQuery: " << searchQuery.ToStdString() << std::endl;

    MYRESULTS searchRes;

    if (mCurrentSearchState == kss_Title)
        searchRes = mDb->searchTitle(searchQuery);  // array of MLMedication
    else if (mCurrentSearchState == kss_Author)
        searchRes = mDb->searchAuthor(searchQuery);
    else if (mCurrentSearchState == kss_AtcCode)
        searchRes = mDb->searchATCCode(searchQuery);
    else if (mCurrentSearchState == kss_RegNr)
        searchRes = mDb->searchRegNr(searchQuery);
    else if (mCurrentSearchState == kss_Therapy)
        searchRes = mDb->searchApplication(searchQuery);
    else if (mCurrentSearchState == kss_FullText)
    {
        if (searchQuery.length() > 2)
            searchRes = mFullTextDb->searchKeyword(searchQuery); // array of FullTextEntry
    }

    mCurrentSearchKey = searchQuery;
    
    return searchRes;
}

// 2286
void MainWindow::updateTableView()
{
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
}

void MainWindow::OnButtonPressed( wxCommandEvent& event )
{
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
            break;
    }
    
    if (prevState == kss_FullText || mCurrentSearchState == kss_FullText)
        updateSearchResults();
    
    if (searchResults.size() > 0) {
        updateTableView();
        // myTableView->reloadData(); // TODO:
    }
}

void MainWindow::OnToolbarAction( wxCommandEvent& event )
{
    // TODO: launchProgressIndicator();
    switchTabs(event.GetId());
}

void MainWindow::OnPrintDocument( wxCommandEvent& event )
{
    std::clog << __PRETTY_FUNCTION__ << " " << event.GetId() << std::endl;
}

void MainWindow::OnShowAboutPanel( wxCommandEvent& event )
{
    wxMessageBox(wxString::Format("%s\n%s\nSQLite %s",
             wxGetOsDescription(), wxVERSION_STRING, SQLITE_VERSION),
    APP_NAME, wxOK | wxICON_INFORMATION);
}

void MainWindow::OnUpdateAipsDatabase( wxCommandEvent& event )
{
    // TODO: check if there is an active internet connection
    //std::clog << wxDialupManager::IsOnline() << std::endl;

    wxBusyCursor wait;

    const char * languageCode = UTI::appLanguage();

    downloadTextFileWithName(wxString::Format("amiko_report_%s.html", languageCode));
    
    downloadFileWithName(wxString::Format("drug_interactions_csv_%s.zip", languageCode));
    downloadFileWithName(wxString::Format("amiko_frequency_%s.db.zip", languageCode));
    downloadFileWithName(wxString::Format("amiko_db_full_idx_%s.zip", languageCode));
}

void MainWindow::OnLoadAipsDatabase( wxCommandEvent& event )
{
    std::clog << __PRETTY_FUNCTION__ << std::endl;
}
