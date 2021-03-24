#include "AboutDialog.h"

#include <sqlite3.h>
#include <wx/app.h>
#include <curl/curl.h>
#include "config.h"
#include "main.hpp"
#include "../res/xpm/AmiKo.xpm"

size_t writefuncAbout(void *ptr, size_t size, size_t nmemb, std::string *s) 
{
  s->append(static_cast<char *>(ptr), size*nmemb);
  return size*nmemb;
}


AboutDialog::AboutDialog( wxWindow* parent )
:
AboutDialogBase( parent )
{
    mVersionLabel->SetLabel(wxString::Format("AmiKo Desitin wx build %s\n%s\n%s\nSQLite %s",
                              PROJECT_VER,
                              wxGetOsDescription(),
                              wxVERSION_STRING,
                              SQLITE_VERSION));

    mUpdateButton->Enable(false);
    mUpdateButton->SetLabel(_("Checking for update"));

    mLogoBitmap->SetScaleMode(wxStaticBitmap::Scale_AspectFit);
    
    wxGetApp().CallAfter([=]{
        CheckForUpdate();
    });
    this->Layout();
}

void AboutDialog::OnCloseClick( wxCommandEvent& event ) {
    EndModal(wxID_OK);
}

void AboutDialog::CheckForUpdate() {
    CURL *curl = curl_easy_init();
    std::string s;
    curl_easy_setopt(curl, CURLOPT_URL, "https://github.com/zdavatz/amiko_wx/releases/latest");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefuncAbout);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    wxArrayString parts = wxSplit(wxString(s), '"');

    if (parts.GetCount() < 2) return;
    auto url = parts[1];

    wxArrayString urlParts = wxSplit(url, '/');
    if (urlParts.GetCount() < 1) return;
    auto last = urlParts.Last();

    if (last != PROJECT_VER) {
        mUpdateButton->Enable(true);
        mUpdateButton->SetLabel(_("Update"));
    } else {
        mUpdateButton->Enable(false);
        mUpdateButton->SetLabel(_("Already up-to-date"));
    }
    this->Layout();
}

void AboutDialog::OnUpdateClick(wxCommandEvent& event) {
    wxLaunchDefaultBrowser("https://github.com/zdavatz/amiko_wx/releases/latest");
}
