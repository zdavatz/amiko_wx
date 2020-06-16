#include <wx/wx.h>
#include <wx/stdpaths.h>
//#include <wx/dialup.h>

#include "MainWindow.h"
#include "customURLConnection.hpp"
#include "DBAdapter.hpp"

#include "../res/xpm/CoMed.xpm"

MainWindow::MainWindow( wxWindow* parent )
:
MainWindowBase( parent )
{
    if (APP_NAME == "CoMed") {
        m_toolAbout->SetLabel("CoMed Desitin");
        m_tbMain->SetToolNormalBitmap(wxID_ABOUT, wxBitmap( CoMed_xpm ));
    }
    
    SetTitle(APP_NAME);
    
    // TODO: Register applications defaults if necessary
    // TODO: Start timer to check if database needs to be updatd (checks every hour)

    // TODO. Open sqlite database
    openSQLiteDatabase();
    
    // TODO: Open fulltext database
    // openFullTextDatabase();
//    #ifdef DEBUG
//    NSLog(@"Number of records in fulltext database = %ld", (long)[mFullTextDb getNumRecords]);
//    #endif
        
    // TODO: Open drug interactions csv file
    //openInteractionsCsvFile();
//    #ifdef DEBUG
//    NSLog(@"Number of records in interaction file = %lu", (unsigned long)[mInteractions getNumInteractions]);
//    #endif

    fiPanel->SetPage("<html><body>Fachinfo</body></html>");
    fiPanel->Fit();
}

void MainWindow::openSQLiteDatabase()
{
    mDb = new DBAdapter;

    const char * languageCode;
    if (APP_NAME == "CoMed")
        languageCode = "fr";
    else
        languageCode = "de";

    bool ok = mDb->openDatabase(wxString::Format("amiko_db_full_idx_%s", languageCode));
}

void MainWindow::OnButtonPressed( wxCommandEvent& event )
{
    switch (event.GetId()) {
        case wxID_BTN_PREPARATION:
            std::clog << "TODO: setSearchState:kTitle" << std::endl;
            break;
            
        case wxID_BTN_REGISTRATION_OWNER:
            std::clog << "TODO: setSearchState:kAuthor" << std::endl;
            break;
            
        case wxID_BTN_ACTIVE_SUBSTANCE:
            std::clog << "TODO: setSearchState:kAtcCode" << std::endl;
            break;
            
        case wxID_BTN_REGISTATION_NUMBER:
            std::clog << "TODO: setSearchState:kRegNr" << std::endl;
            break;
            
        case wxID_BTN_THERAPY:
            std::clog << "TODO: setSearchState:kTherapy" << std::endl;
            break;
            
        case wxID_BTN_FULL_TEXT:
            std::clog << "TODO: setSearchState:kFullText" << std::endl;
            break;
    }
}

void MainWindow::showAboutPanel( wxCommandEvent& event )
{
    wxMessageBox(wxString::Format("%s\n%s",
             wxGetOsDescription(), wxVERSION_STRING),
    APP_NAME, wxOK | wxICON_INFORMATION);
}

void MainWindow::OnUpdateAipsDatabase( wxCommandEvent& event )
{
    // TODO: check if there is an active internet connection
    //std::clog << wxDialupManager::IsOnline() << std::endl;

    wxBusyCursor wait;

    const char * languageCode;
    if (APP_NAME == "CoMed")
        languageCode = "fr";
    else
        languageCode = "de";

    downloadTextFileWithName(wxString::Format("amiko_report_%s.html", languageCode));
    
    downloadFileWithName(wxString::Format("drug_interactions_csv_%s.zip", languageCode));
    downloadFileWithName(wxString::Format("amiko_frequency_%s.db.zip", languageCode));
    downloadFileWithName(wxString::Format("amiko_db_full_idx_%s.zip", languageCode));
}

void MainWindow::OnLoadAipsDatabase( wxCommandEvent& event )
{
    std::clog << __PRETTY_FUNCTION__ << std::endl;
}
