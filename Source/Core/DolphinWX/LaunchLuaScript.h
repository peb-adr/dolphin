// Copyright 2013 Dolphin Emulator Project
// Licensed under GPLv2
// Refer to the license.txt file included.

#pragma once

//Dragonbane

#include <wx/defs.h>
#include <wx/dialog.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/translation.h>
#include <wx/windowid.h>
#include <wx/artprov.h>
#include <wx/stattext.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>

#include <wx/artprov.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/frame.h>


#include "Common/CommonTypes.h"
#include <string>


class wxWindow;
class wxButton;

class LuaWindow : public wxDialog
{
private:

	DECLARE_EVENT_TABLE();

protected:
	wxButton* m_buttonStart;
	wxButton* m_buttonCancel;
	wxStaticText* m_scriptLabel;
	wxChoice* m_scriptChoice;
	wxButton* m_scriptOptButton;
	wxStaticText* m_scriptLabelSlots[10];
	wxChoice* m_scriptChoiceSlots[10];
	wxButton* m_scriptOptButtonSlots[10];

	wxStaticText* m_staticTextOpt;

public:

	LuaWindow(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Launch Script by DB"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style  = wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP);
	void StartScript(wxString scriptName);
	void StartScriptSlot(int scriptSlot);
	void CancelScript(wxString scriptName);
	void CancelScriptSlot(int scriptSlot);
	bool CheckScript(wxString scriptName);
	void OnButtonPressed(wxCommandEvent& event);
	void OnOptButtonPressed(wxCommandEvent& event);
	void OnCloseWindow(wxCloseEvent& event);

	void Shown();

private:
	wxBoxSizer* makeScriptSelect(wxString& label, wxStaticText** text, wxChoice** choice, wxButton** button);



public:
	class OptionsDialog : public wxDialog
	{
	private:

		DECLARE_EVENT_TABLE();

	public:

		OptionsDialog(wxWindow* parent, const wxString& scriptName, std::string filename, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP);

		void OnButtonPressed(wxCommandEvent& event);
		void OnTextChanged(wxCommandEvent& event);
		void OnCloseWindow(wxCloseEvent& event);

	protected:

		wxString m_scriptName;
		std::string m_filename;
		bool m_changed;

		wxStaticText* m_commentHintT;
		wxStaticText* m_commentHint;
		wxTextCtrl* m_textBox;
		wxButton* m_buttonSave;
		wxButton* m_buttonDiscard;

	private:
		void saveToFile();

	};

};
