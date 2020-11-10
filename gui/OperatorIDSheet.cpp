#include "OperatorIDSheet.h"
#include "Operator.hpp"
#include "MainWindow.h"
#include "DefaultsController.hpp"
#include "Utilities.hpp"
#include "SignatureView.hpp"

// 35
OperatorIDSheet::OperatorIDSheet( wxWindow* parent )
:
OperatorIDSheetBase( parent )
{
    loadSettings();
    Fit();
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
    png.SaveFile(filePath, wxBITMAP_TYPE_PNG);

    // All other settings are saved using NSUserDefaults
    DefaultsController* defaults = DefaultsController::Instance();

    defaults->setString(mTitle->GetValue(), DEFAULTS_DOC_TITLE);
    defaults->setString(mFamilyName->GetValue(), DEFAULTS_DOC_SURNAME);
    defaults->setString(mGivenName->GetValue(), DEFAULTS_DOC_NAME);
    defaults->setString(mPostalAddress->GetValue(), DEFAULTS_DOC_ADDRESS);
    defaults->setString(mZipCode->GetValue(), DEFAULTS_DOC_ZIP);
    defaults->setString(mCity->GetValue(), DEFAULTS_DOC_CITY);
    //defaults->setString(mCountry->GetValue(), DEFAULTS_DOC_COUNTRY);
    defaults->setString(mPhoneNumber->GetValue(), DEFAULTS_DOC_PHONE);
    defaults->setString(mEmailAddress->GetValue(), DEFAULTS_DOC_EMAIL);
    defaults->Flush();
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

    DefaultsController* defaults = DefaultsController::Instance();

    oper->title = defaults->getString(DEFAULTS_DOC_TITLE, "");
    oper->familyName = defaults->getString(DEFAULTS_DOC_SURNAME, "");
    oper->givenName = defaults->getString(DEFAULTS_DOC_NAME, "");
    oper->postalAddress = defaults->getString(DEFAULTS_DOC_ADDRESS, "");
    oper->zipCode = defaults->getString(DEFAULTS_DOC_ZIP, "");
    oper->city = defaults->getString(DEFAULTS_DOC_CITY, "");
    oper->country = defaults->getString(DEFAULTS_DOC_COUNTRY, "");
    oper->phoneNumber = defaults->getString(DEFAULTS_DOC_PHONE, "");
    oper->emailAddress = defaults->getString(DEFAULTS_DOC_EMAIL, "");
    
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
    DefaultsController* defaults = DefaultsController::Instance();
    return defaults->getString(DEFAULTS_DOC_CITY, "");
}

