#ifndef __OperatorIDSheet__
#define __OperatorIDSheet__

/**
@file
Subclass of OperatorIDSheetBase, which is generated by wxFormBuilder.
*/

#include "MainWindowBase.h"

//// end generated include

/** Implementing OperatorIDSheetBase */
class OperatorIDSheet : public OperatorIDSheetBase
{
	protected:
		// Handlers for OperatorIDSheetBase events.
		void OnClearSignature( wxCommandEvent& event );
		void OnLoadSignature( wxCommandEvent& event );
		void OnSaveOperator( wxCommandEvent& event );
	public:
		/** Constructor */
		OperatorIDSheet( wxWindow* parent );
	//// end generated class members

    void saveSettings();
    void remove();
};

#endif // __OperatorIDSheet__
