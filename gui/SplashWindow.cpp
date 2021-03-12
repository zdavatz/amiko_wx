#include "SplashWindow.h"
#include "../res/xpm/splash_1000x670.xpm"

SplashWindow::SplashWindow( wxWindow* parent )
:
SplashWindowBase( parent )
{
    m_slashBitmap->SetBitmap(wxBitmap(splash_xpm));
}
