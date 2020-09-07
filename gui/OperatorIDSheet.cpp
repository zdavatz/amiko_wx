#include "OperatorIDSheet.h"
#include "Operator.hpp"
#include "MainWindow.h"
#include "DefaultsController.hpp"

// 35
OperatorIDSheet::OperatorIDSheet( wxWindow* parent )
:
OperatorIDSheetBase( parent )
{

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
#if 1
    std::clog << __PRETTY_FUNCTION__ << " Line:" << __LINE__ << " TODO: png" << std::endl;
#else
    wxString documentsDirectory = UTI::documentsDirectory();
    NSString *filePath = [documentsDirectory stringByAppendingPathComponent:DOC_SIGNATURE_FILENAME];
    NSData *png = [mSignView getSignaturePNG];
    [png writeToFile:filePath atomically:YES];
#endif

    // All other settings are saved using NSUserDefaults
    DefaultsController* defaults = DefaultsController::Instance();
    std::clog << " Defaults path: <" << defaults->GetPath() << ">" << std::endl;
    //std::clog << " Defaults path: <" << defaults->GetLocalFile() << ">" << std::endl;

    defaults->setString(mTitle->GetValue(), DEFAULTS_DOC_TITLE);
    defaults->setString(mFamilyName->GetValue(), DEFAULTS_DOC_SURNAME);
    defaults->setString(mGivenName->GetValue(), DEFAULTS_DOC_NAME);
    defaults->setString(mPostalAddress->GetValue(), DEFAULTS_DOC_ADDRESS);
    defaults->setString(mZipCode->GetValue(), DEFAULTS_DOC_ZIP);
    defaults->setString(mCity->GetValue(), DEFAULTS_DOC_CITY);
#if 1
    std::clog << __PRETTY_FUNCTION__ << " Line:" << __LINE__ << " TODO: mCountry" << std::endl;
#else
    defaults->setString(mCountry->GetValue(), DEFAULTS_DOC_COUNTRY);
#endif
    defaults->setString(mPhoneNumber->GetValue(), DEFAULTS_DOC_PHONE);
    defaults->setString(mEmailAddress->GetValue(), DEFAULTS_DOC_EMAIL);
    defaults->Flush();
}

// /////////////////////////////////////////////////////////////////////////////
void OperatorIDSheet::OnClearSignature( wxCommandEvent& event )
{
#if 1
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
#else
    // TODO: create new class SignatureView
    mSignView->clear();
#endif
}

void OperatorIDSheet::OnLoadSignature( wxCommandEvent& event )
{
#if 1
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
#else
#endif
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

