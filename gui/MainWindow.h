#ifndef __MainWindow__
#define __MainWindow__

/**
@file
Subclass of MainWindowBase, which is generated by wxFormBuilder.
*/

#include "MainWindowBase.h"

//// end generated include

class DBAdapter;

/** Implementing MainWindowBase */
class MainWindow : public MainWindowBase
{
	protected:
		// Handlers for MainWindowBase events.
		void OnButtonPressed( wxCommandEvent& event );
		void OnToolbarAction( wxCommandEvent& event );
		void OnPrintDocument( wxCommandEvent& event );
		void OnShowAboutPanel( wxCommandEvent& event );
		void OnUpdateAipsDatabase( wxCommandEvent& event );
		void OnLoadAipsDatabase( wxCommandEvent& event );
	public:
		/** Constructor */
		MainWindow( wxWindow* parent );
	//// end generated class members

private:
    void openSQLiteDatabase();
    void fadeInAndShow();
    void resetDataInTableView();
    void setSearchState(int searchState);
    void hideTextFinder();
    void updateSearchResults();
    void updateTableView();
    void switchTabs(int item);

    // 105
    int mUsedDatabase;
    // 110
    bool mSearchInteractions;
    bool mPrescriptionMode;
    DBAdapter *mDb;
    int mCurrentSearchState;
    wxString mCurrentSearchKey;
    std::vector<int> searchResults; // TODO: not int
};

#endif // __MainWindow__
