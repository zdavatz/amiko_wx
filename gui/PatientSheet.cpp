#include "PatientSheet.h"

PatientSheet::PatientSheet( wxWindow* parent )
:
PatientSheetBase( parent )
{

}

// 261
Patient *PatientSheet::retrievePatient()
{
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;

    return mSelectedPatient;
}
