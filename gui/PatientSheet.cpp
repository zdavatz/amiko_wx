#include <iostream>

#include "PatientSheet.h"
#include "Patient.hpp"

PatientSheet::PatientSheet( wxWindow* parent )
: PatientSheetBase( parent )
, mSelectedPatient(nullptr)
{

}

// 261
Patient *PatientSheet::retrievePatient()
{
    std::clog << __PRETTY_FUNCTION__ << " TODO" << std::endl;

//    if (!mSelectedPatient)
//        mSelectedPatient = new Patient; // still crash

#if 0 // Issue #7
    return mSelectedPatient;
#else
    return nullptr;
#endif
}
