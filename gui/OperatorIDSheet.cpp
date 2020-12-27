#include "OperatorIDSheet.h"
#include "Operator.hpp"
#include "MainWindow.h"
#include "DefaultsController.hpp"
#include "Utilities.hpp"
#include "SignatureView.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include "sync/GoogleSyncManager.hpp"

// 35
OperatorIDSheet::OperatorIDSheet( wxWindow* parent )
:
OperatorIDSheetBase( parent )
{
    loadSettings();
    Fit();

    fsWatcher = new wxFileSystemWatcher();
    wxFileName jsonFilePath = wxFileName(UTI::documentsDirectory(), DOC_JSON_FILENAME);
    wxFileName signatureFilePath = wxFileName(UTI::documentsDirectory(), DOC_SIGNATURE_FILENAME);
    fsWatcher->Add(jsonFilePath);
    fsWatcher->Add(signatureFilePath);
    fsWatcher->SetOwner(this);
}

BEGIN_EVENT_TABLE(OperatorIDSheet, OperatorIDSheetBase)
    EVT_FSWATCHER(wxID_ANY, OperatorIDSheet::OnFileUpdated)
END_EVENT_TABLE()


void OperatorIDSheet::OnFileUpdated( wxFileSystemWatcherEvent& event ) {
    int eventType = event.GetChangeType();
    if (eventType == wxFSW_EVENT_CREATE || eventType == wxFSW_EVENT_DELETE || eventType == wxFSW_EVENT_MODIFY) {
        loadSettings();
    } else {
        std::clog << "oh yes!" << std::endl;
    }
}

// 105
void OperatorIDSheet::remove()
{
#if 1
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
#else
    [NSApp endModalSession:mModalSession];
    [NSApp endSheet:mPanel];
    [mPanel orderOut:nil];
    [mPanel close];
#endif
}

// 157
void OperatorIDSheet::saveSettings()
{
    // Signature is saved as a PNG to Documents Directory within the app
    wxString documentsDirectory = UTI::documentsDirectory();
    wxString filePath = documentsDirectory + wxFILE_SEP_PATH + DOC_SIGNATURE_FILENAME;
    wxImage png = mSignView->getSignaturePNG();
    if (png.IsOk())
        png.SaveFile(filePath, wxBITMAP_TYPE_PNG);

    nlohmann::json json;
    json[DOC_JSON_TITLE] = mTitle->GetValue().ToStdString();
    json[DOC_JSON_SURNAME] = mFamilyName->GetValue().ToStdString();
    json[DOC_JSON_NAME] = mGivenName->GetValue().ToStdString();
    json[DOC_JSON_ADDRESS] = mPostalAddress->GetValue().ToStdString();
    json[DOC_JSON_ZIP] = mZipCode->GetValue().ToStdString();
    json[DOC_JSON_CITY] = mCity->GetValue().ToStdString();
    json[DOC_JSON_PHONE] = mPhoneNumber->GetValue().ToStdString();
    json[DOC_JSON_EMAIL] = mEmailAddress->GetValue().ToStdString();
    // json[DOC_JSON_COUNTRY] = mCountry->GetValue().ToStdString();
    wxString doctorFilePath = documentsDirectory + wxFILE_SEP_PATH + DOC_JSON_FILENAME;
    wxFile *file = new wxFile(doctorFilePath, wxFile::write);
    file->Write(json.dump());
    file->Close();

    GoogleSyncManager::Instance()->requestSync();
}

// 199
void OperatorIDSheet::loadSettings()
{
    wxString documentsDirectory = UTI::documentsDirectory();
    wxString filePath = documentsDirectory + wxFILE_SEP_PATH + DOC_SIGNATURE_FILENAME;
    if (wxFileName::Exists(filePath)) {
        wxImage image;
        if (image.LoadFile(filePath, wxBITMAP_TYPE_PNG)) {
            mSignView->SetBitmap( wxBitmap(image));
            mSignView->SetScaleMode(wxStaticBitmap::Scale_AspectFit);
        }
        else
            std::cerr << __FUNCTION__ << " couldn't load " << filePath << std::endl;
    }

    // 208
    Operator *oper = loadOperator(); // 'operator' is a C++ reserved word
    
    mTitle->SetValue( oper->title);
    mFamilyName->SetValue( oper->familyName);
    mGivenName->SetValue( oper->givenName);
    mPostalAddress->SetValue( oper->postalAddress);
    mZipCode->SetValue( oper->zipCode);
    mCity->SetValue( oper->city);
    //mCountry->SetValue( oper->country);
    mPhoneNumber->SetValue( oper->phoneNumber);
    mEmailAddress->SetValue( oper->emailAddress);
}

// /////////////////////////////////////////////////////////////////////////////
// 81
void OperatorIDSheet::OnClearSignature( wxCommandEvent& event )
{
#if 1
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
#else
    mSignView->clear();
#endif
}

// 61
void OperatorIDSheet::OnLoadSignature( wxCommandEvent& event )
{
    wxString wildCard = wxString::Format("PNG %s (*.png;*.PNG)|*.png;*.PNG|GIF %s (*.gif;*.GIF)|*.gif;*.GIF|JPG %s (*.jpg;*.JPG)|*.jpg;*.JPG",
                                         _("files"), _("files"), _("files"));
    wxFileDialog openDlgPanel(this,
                              _("Please select signature file"),
                              wxEmptyString,
                              wxEmptyString,
                              wildCard,
                              wxFD_OPEN | wxFD_FILE_MUST_EXIST, // no wxFD_MULTIPLE
                              wxDefaultPosition);
    
    if (openDlgPanel.ShowModal() != wxID_OK)
        return;
    
    // 74
    // Grab reference to what has been selected
    wxString fileURL = openDlgPanel.GetPath();
    wxImage image;
    image.LoadFile(fileURL);
    mSignView->SetBitmap( wxBitmap(image));
}

// 53
void OperatorIDSheet::OnSaveOperator( wxCommandEvent& event )
{
    saveSettings();
    EndModal(wxID_OK); // remove()

#if 0
    std::clog << __FUNCTION__ << " TODO" << std::endl;
    [[NSNotificationCenter defaultCenter] postNotificationName:@"MLPrescriptionDoctorChanged" object:nil];
#else
    // Call the notification target directly
    // call the notification target directly
    MainWindow* vc = (MainWindow *)wxTheApp->GetTopWindow();
    vc->prescriptionDoctorChanged();
#endif
}

// 180
Operator * OperatorIDSheet::loadOperator()
{
    // Load from user defaults
    Operator *oper = new Operator;

    try {
        std::ifstream i((UTI::documentsDirectory() + wxFILE_SEP_PATH + DOC_JSON_FILENAME).ToStdString());
        nlohmann::json json;
        i >> json;

        oper->title = wxString(json[DOC_JSON_TITLE].get<std::string>());
        oper->familyName = wxString(json[DOC_JSON_SURNAME].get<std::string>());
        oper->givenName = wxString(json[DOC_JSON_NAME].get<std::string>());
        oper->postalAddress = wxString(json[DOC_JSON_ADDRESS].get<std::string>());
        oper->zipCode = wxString(json[DOC_JSON_ZIP].get<std::string>());
        oper->city = wxString(json[DOC_JSON_CITY].get<std::string>());
        oper->country = wxString(json[DOC_JSON_COUNTRY].get<std::string>());
        oper->phoneNumber = wxString(json[DOC_JSON_PHONE].get<std::string>());
        oper->emailAddress = wxString(json[DOC_JSON_EMAIL].get<std::string>());
    } catch (const std::exception& e) {
        // Just in case the file is not initialized
    }
    
    return oper;
}

// 238
wxString OperatorIDSheet::retrieveIDAsString()
{
    Operator *oper = loadOperator(); // 'operator' is a C++ reserved word
    
    if (oper->familyName.size() > 0 &&
        oper->givenName.size() > 0)
        return oper->retrieveOperatorAsString();

    return _("Enter the doctor's address");
}

// 248
wxString OperatorIDSheet::retrieveCity()
{
    wxString cityString;
    try {
        std::ifstream i((UTI::documentsDirectory() + wxFILE_SEP_PATH + DOC_JSON_FILENAME).ToStdString());
        nlohmann::json json;
        i >> json;
        cityString = json[DOC_JSON_CITY].get<std::string>();
    } catch (const std::exception& e) {
        // Just in case the file is not initialized
    }
    return cityString;
}

