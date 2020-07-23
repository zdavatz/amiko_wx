#include "OperatorIDSheet.h"

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
    NSString *documentsDirectory = [MLUtilities documentsDirectory];
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
#if 1
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;
#else
    saveSettings();
    remove();

    // TODO: [[NSNotificationCenter defaultCenter] postNotificationName:@"MLPrescriptionDoctorChanged" object:nil];
#endif
}
