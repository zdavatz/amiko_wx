///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.9.0 Nov 11 2020)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statline.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class myLabelPanel2
///////////////////////////////////////////////////////////////////////////////
class myLabelPanel2 : public wxPanel
{
	private:

	protected:
		wxStaticText* labelDoctor;
		wxStaticLine* m_staticline1;
		wxStaticText* labelPatient;
		wxStaticText* labelMedicine;
		wxStaticText* labelComment;
		wxStaticText* labelSwissmed;
		wxStaticText* labelPrice;

	public:

		myLabelPanel2( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 252,200 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );
		~myLabelPanel2();

};

///////////////////////////////////////////////////////////////////////////////
/// Class myLabelPanel
///////////////////////////////////////////////////////////////////////////////
class myLabelPanel : public wxFrame
{
	private:

	protected:
		wxStaticText* labelDoctor;
		wxStaticLine* m_staticline1;
		wxStaticText* labelPatient;
		wxStaticText* labelMedicine;
		wxStaticText* labelComment;
		wxStaticText* labelSwissmed;
		wxStaticText* labelPrice;

	public:

		myLabelPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 252,200 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~myLabelPanel();

};

