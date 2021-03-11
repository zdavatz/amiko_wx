#include "SplashWindow.h"

SplashWindow::SplashWindow( wxWindow* parent )
:
SplashWindowBase( parent )
{
    m_slashBitmap->SetBitmap(wxBitmap( wxT("res/splash_1000x670.png"), wxBITMAP_TYPE_ANY ));
}
