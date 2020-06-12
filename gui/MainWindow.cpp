#include "MainWindow.h"

MainWindow::MainWindow( wxWindow* parent )
:
MainWindowBase( parent )
{
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
