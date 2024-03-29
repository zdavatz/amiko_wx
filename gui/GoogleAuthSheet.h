#ifndef __GoogleAuthSheet__
#define __GoogleAuthSheet__

/**
@file
Subclass of GoogleAuthSheet, which is generated by wxFormBuilder.
*/

#include "MainWindowBase.h"

//// end generated include

#include "cpp-httplib/httplib.h"

/** Implementing GoogleAuthSheet */
class GoogleAuthSheet : public GoogleAuthSheetBase
{
	public:
		/** Constructor */
		GoogleAuthSheet( wxWindow* parent );
	//// end generated class members

		void LoadAuthURL();

	protected:
		void OnCloseClicked( wxCommandEvent& event );
		void OnClose( wxCloseEvent& event );
		void OnWebViewNavigated( wxWebViewEvent& event );

	private:
		DECLARE_EVENT_TABLE()

		httplib::Server m_localServer;
		int m_port = 0;
		std::thread m_httpThread;
		int SetupLocalServer();
		void CloseServer();
};

#endif // __GoogleAuthSheet__
