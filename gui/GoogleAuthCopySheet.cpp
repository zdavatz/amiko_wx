#include "GoogleAuthCopySheet.h"
#include "../sync/GoogleSyncManager.hpp"

GoogleAuthCopySheet::GoogleAuthCopySheet( wxWindow* parent )
:
GoogleAuthCopySheetBase( parent )
{
    m_okButton->Disable();
}

void GoogleAuthCopySheet::OnClickedOpenInBrowser( wxCommandEvent& event )
{
    GoogleSyncManager *g = GoogleSyncManager::Instance();
    std::string url = g->authURLForCopy();
    wxLaunchDefaultBrowser(url);
}

void GoogleAuthCopySheet::OnCodeChanged( wxCommandEvent& event )
{
    if (mCodeTextControl->IsEmpty()) {
        m_okButton->Disable();
    } else {
        m_okButton->Enable();
    }
}

void GoogleAuthCopySheet::OnClickedCancel( wxCommandEvent& event )
{
    EndModal(wxID_CANCEL);
}

void GoogleAuthCopySheet::OnClickedOK( wxCommandEvent& event )
{
    GoogleSyncManager *g = GoogleSyncManager::Instance();
    try {
        g->receivedAuthCode(mCodeTextControl->GetValue().ToStdString(), true);
    } catch (...) {
        wxMessageDialog dialog(this,
              _("Error"),
              _("The code is incorrect"),
              wxOK | wxICON_WARNING);
        dialog.ShowModal();
        return;
    }
    EndModal(wxID_OK);
}
