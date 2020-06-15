#include <wx/wx.h>
#include <wx/stdpaths.h>

#include "MainWindow.h"
#include "customURLConnection.hpp"

#include "../res/xpm/CoMed.xpm"

MainWindow::MainWindow( wxWindow* parent )
:
MainWindowBase( parent )
{
    if (APP_NAME == "CoMed") {
        m_toolAbout->SetLabel("CoMed Desitin");
        m_tbMain->SetToolNormalBitmap(wxID_ABOUT, wxBitmap( CoMed_xpm ));
    }

    fiPanel->SetPage("<html><body>Fachinfo</body></html>");
    fiPanel->Fit();
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
    downloadTextFileWithName("amiko_report_de.html");

    downloadFileWithName("drug_interactions_csv_de.zip");
    downloadFileWithName("amiko_frequency_de.db.zip");
    downloadFileWithName("amiko_db_full_idx_de.db.zip");
}

void MainWindow::OnLoadAipsDatabase( wxCommandEvent& event )
{
    std::clog << __PRETTY_FUNCTION__ << std::endl;
}
