#include "MainWindow.h"

MainWindow::MainWindow( wxWindow* parent )
:
MainWindowBase( parent )
{
    fiPanel->SetPage("<html><body>Fachinfo</body></html>");
    fiPanel->Fit();
}
