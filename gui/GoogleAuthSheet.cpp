#include "GoogleAuthSheet.h"
#include "../sync/GoogleSyncManager.hpp"
#include "cpp-httplib/httplib.h"

GoogleAuthSheet::GoogleAuthSheet( wxWindow* parent )
:
GoogleAuthSheetBase( parent )
{
    LoadAuthURL();
}

void GoogleAuthSheet::LoadAuthURL() {
    int port = setupLocalServer();
    GoogleSyncManager *g = GoogleSyncManager::Instance();
    std::string url = g->authURL(port);
    myWebView->LoadURL(wxString(url));
}

void GoogleAuthSheet::OnCloseClicked( wxCommandEvent& event ) {
    EndModal(wxID_CANCEL);
}

BEGIN_EVENT_TABLE(GoogleAuthSheet, GoogleAuthSheetBase)
    EVT_WEBVIEW_NAVIGATED(wxID_ANY, GoogleAuthSheet::OnWebViewNavigated)
END_EVENT_TABLE()

int GoogleAuthSheet::setupLocalServer() {
    if (m_localServer.is_running()) return m_port;
    std::clog << "starting server." << std::endl;

    m_localServer.Get("/", [](const httplib::Request& req, httplib::Response& res) {
        if (req.has_param("code")) {
            auto val = req.get_param_value("code");
        }
        res.set_content("Loading", "text/plain");
    });

    m_port = m_localServer.bind_to_any_port("0.0.0.0");
    std::clog << "server port: " << m_port << std::endl;
    m_httpThread = std::thread([&]() { m_localServer.listen_after_bind(); });
    return m_port;
}

void GoogleAuthSheet::OnWebViewNavigated( wxWebViewEvent& event ) {
    std::string url = myWebView->GetCurrentURL().ToStdString();
    auto position = url.find("code=");
    if (url.rfind("http://127.0.0.1", 0) == 0 && position != std::string::npos) {
        auto codeAndRest = url.substr(position + 5);
        auto pos = codeAndRest.find("&");
        if (pos != std::string::npos) {
            auto code = codeAndRest.substr(0, pos);
            GoogleSyncManager *g = GoogleSyncManager::Instance();
            g->receivedAuthCode(code, m_port);

            m_localServer.stop();
            m_httpThread.join();

            EndModal(wxID_OK);
        }

    }
}
