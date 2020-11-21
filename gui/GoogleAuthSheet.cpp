#include "GoogleAuthSheet.h"
#include "../sync/GoogleSyncManager.hpp"

GoogleAuthSheet::GoogleAuthSheet( wxWindow* parent )
:
GoogleAuthSheetBase( parent )
{
    LoadAuthURL();
}

void GoogleAuthSheet::LoadAuthURL() {
    GoogleSyncManager *g = GoogleSyncManager::Instance();
    std::string url = g->authURL();
    myWebView->LoadURL(wxString(url));
}

void GoogleAuthSheet::OnCloseClicked( wxCommandEvent& event ) {
    EndModal(wxID_CANCEL);
}

BEGIN_EVENT_TABLE(GoogleAuthSheet, GoogleAuthSheetBase)
    EVT_WEBVIEW_TITLE_CHANGED(wxID_ANY, GoogleAuthSheet::OnWebViewTitleChanged)
END_EVENT_TABLE()

void GoogleAuthSheet::OnWebViewTitleChanged( wxWebViewEvent& event ) {
    // When the app is authorized, the title of the page is changed to something like this:
    // Success state=1&code=<code>&scope=<scope>&authuser=0&hd=<...>&prompt=consent
    // We need to extract the code for further processing

    auto title = event.GetString().ToStdString();
    auto position = title.find("code=");
    if (position != std::string::npos) {
        auto codeAndRest = title.substr(position + 5);
        auto pos = codeAndRest.find("&");
        if (pos != std::string::npos) {
            auto code = codeAndRest.substr(0, pos);
            GoogleSyncManager *g = GoogleSyncManager::Instance();
            g->receivedAuthCode(code);
            EndModal(wxID_OK);
        }
    }
}
