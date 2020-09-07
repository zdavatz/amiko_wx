#include "OperatorIDSheet.h"
#include "Operator.hpp"
#include "MainWindow.h"

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
#if 1
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
#else
    // Signature is saved as a PNG to Documents Directory within the app
    wxString documentsDirectory = UTI::documentsDirectory();
    NSString *filePath = [documentsDirectory stringByAppendingPathComponent:DOC_SIGNATURE_FILENAME];
    NSData *png = [mSignView getSignaturePNG];
    [png writeToFile:filePath atomically:YES];
    
    // All other settings are saved using NSUserDefaults
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [defaults setObject:[mTitle stringValue]        forKey:DEFAULTS_DOC_TITLE];
    [defaults setObject:[mFamilyName stringValue]   forKey:DEFAULTS_DOC_SURNAME];
    [defaults setObject:[mGivenName stringValue]    forKey:DEFAULTS_DOC_NAME];
    [defaults setObject:[mPostalAddress stringValue] forKey:DEFAULTS_DOC_ADDRESS];
    [defaults setObject:[mZipCode stringValue]      forKey:DEFAULTS_DOC_ZIP];
    [defaults setObject:[mCity stringValue]         forKey:DEFAULTS_DOC_CITY];
    [defaults setObject:[mCountry stringValue]      forKey:DEFAULTS_DOC_COUNTRY];
    [defaults setObject:[mPhoneNumber stringValue]  forKey:DEFAULTS_DOC_PHONE];
    [defaults setObject:[mEmailAddress stringValue] forKey:DEFAULTS_DOC_EMAIL];
    // Writes mods to persistent storage
    [defaults synchronize];
#endif
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
Operator * loadOperator()
{
    // Load from user defaults
    Operator *oper = new Operator;
    
#if 1
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
#else
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    oper->title = [defaults stringForKey:DEFAULTS_DOC_TITLE];
    oper->familyName = [defaults stringForKey:DEFAULTS_DOC_SURNAME];
    oper->givenName = [defaults stringForKey:DEFAULTS_DOC_NAME];
    oper->postalAddress = [defaults stringForKey:DEFAULTS_DOC_ADDRESS];
    oper->zipCode = [defaults stringForKey:DEFAULTS_DOC_ZIP];
    oper->city = [defaults stringForKey:DEFAULTS_DOC_CITY];
    oper->country = [defaults stringForKey:DEFAULTS_DOC_COUNTRY];
    oper->phoneNumber = [defaults stringForKey:DEFAULTS_DOC_PHONE];
    oper->emailAddress = [defaults stringForKey:DEFAULTS_DOC_EMAIL];
#endif
    
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
#if 1
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
    return wxEmptyString;
#else
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    return [defaults stringForKey:@"city"];
#endif
}

