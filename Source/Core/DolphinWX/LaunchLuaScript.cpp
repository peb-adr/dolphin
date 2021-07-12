// Copyright 2013 Dolphin Emulator Project
// Licensed under GPLv2
// Refer to the license.txt file included.

#include <wx/bitmap.h>
#include <wx/defs.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/gdicmn.h>
#include <wx/image.h>
#include <wx/mstream.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/translation.h>
#include <wx/windowid.h>
#include <wx/msgdlg.h>
#include <array>
#include <wx/filepicker.h>
#include <wx/utils.h>

#include "Common/Common.h"
#include "DolphinWX/LaunchLuaScript.h"
#include "Core/Core.h"
#include "Core/HW/Memmap.h"
#include "Common/IniFile.h"

#include "Common/StringUtil.h"
#include "Common/FileUtil.h"
#include "DiscIO/Filesystem.h"
#include "Common/FileSearch.h"

#include "DiscIO/FileSystemGCWii.h"
#include "DiscIO/Volume.h"
#include "DiscIO/VolumeCreator.h"
#include "Core/ConfigManager.h"
#include "Core/Movie.h"
#include "Core/LUA/Lua.h"

#include "DolphinWX/ISOFile.h"
#include "DolphinWX/ISOProperties.h"
#include "DolphinWX/WxUtils.h"

//Dragonbane
BEGIN_EVENT_TABLE(LuaWindow, wxDialog)

EVT_CHOICE(1, LuaWindow::OnSelectionChanged) //Script Selection
EVT_BUTTON(2, LuaWindow::OnButtonPressed) //Start
EVT_BUTTON(3, LuaWindow::OnButtonPressed) //Cancel
EVT_BUTTON(4, LuaWindow::OnOptButtonPressed) //OPT

END_EVENT_TABLE()

LuaWindow::LuaWindow(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
	SetSizeHints(wxDefaultSize, wxDefaultSize);

	// Manual selector
	wxBoxSizer* sizer;
	sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(makeScriptSelect(wxString(wxT("Script File:")), &m_staticText10, &m_choice_script, &m_buttonScriptOpt), 0, wxALIGN_RIGHT);

	sizer->AddSpacer(5);

	// Start/Cancel buttons
	wxBoxSizer* buttonsizer;
	buttonsizer = new wxBoxSizer(wxHORIZONTAL);
	m_button4 = new wxButton(this, 2, wxT("Start"), wxDefaultPosition, wxDefaultSize, 0);
	buttonsizer->Add(m_button4, 0, wxALIGN_RIGHT | wxLEFT, 50);
	buttonsizer->AddSpacer(30);
	m_button5 = new wxButton(this, 3, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
	buttonsizer->Add(m_button5, 0, wxALIGN_CENTER | wxRIGHT, 10);
	sizer->Add(buttonsizer);

	sizer->AddSpacer(20);

	// Slot selectors
	for (int i = 0; i < 10; i++)
	{
		sizer->Add(makeScriptSelect(wxString::Format(_("Slot %d:"), i + 1), &m_staticTextSlots[i], &m_choice_scriptSlots[i], &m_buttonScriptOptSlots[i]), 0, wxALIGN_RIGHT);
	}

	wxBoxSizer* outersizer;
	outersizer = new wxBoxSizer(wxVERTICAL);
	outersizer->Add(sizer, 0, wxALL, 10);
	SetSizer(outersizer);
	Layout();
	outersizer->Fit(this);
	Centre(wxBOTH);

	Bind(wxEVT_CLOSE_WINDOW, &LuaWindow::OnCloseWindow, this);
}

wxBoxSizer* LuaWindow::makeScriptSelect(wxString& label, wxStaticText** text, wxChoice** choice, wxButton** button)
{
	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

	// Label
	*text = new wxStaticText(this, wxID_ANY, label, wxDefaultPosition, wxDefaultSize, 0);
	(*text)->Wrap(-1);
	(*text)->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString));
	sizer->Add(*text, 0, wxRIGHT, 15);

	// Choice
	wxArrayString m_choice_scriptChoices;
	*choice = new wxChoice(this, 1, wxDefaultPosition, wxSize(200, -1), m_choice_scriptChoices, 0);
	sizer->Add(*choice, 0, wxALIGN_RIGHT | wxRIGHT, 5);

	// OPT Button
	*button = new wxButton(this, 4, wxT("OPT"), wxDefaultPosition, wxDefaultSize, 0);
	(*button)->SetClientData(*choice);
	sizer->Add(*button, 0, wxALIGN_RIGHT);

	return sizer;
}


void LuaWindow::StartScriptSlot(int scriptSlot)
{
	wxString selectedScriptName = m_choice_scriptSlots[scriptSlot - 1]->GetStringSelection();
	StartScript(selectedScriptName);
}

void LuaWindow::CancelScriptSlot(int scriptSlot)
{
	wxString selectedScriptName = m_choice_scriptSlots[scriptSlot - 1]->GetStringSelection();
	CancelScript(selectedScriptName);
}

void LuaWindow::StartScript(wxString scriptName)
{
	if (!CheckScript(scriptName))
		return;

	std::string FileName = WxStrToStr(scriptName);

	if (File::Exists(File::GetExeDirectory() + "\\Scripts\\" + FileName) == false)
	{
		wxMessageBox("Script file does not exist!");
		return;
	}

	if (Lua::IsScriptRunning(FileName))
	{
		wxMessageBox("Script is already running!");
		return;
	}

	Lua::LoadScript(FileName);
}

void LuaWindow::CancelScript(wxString scriptName)
{
	if (!CheckScript(scriptName))
		return;

	std::string FileName = WxStrToStr(scriptName);

	if (Lua::IsScriptRunning(FileName) == false)
	{
		wxMessageBox("Script is not loaded!");
		return;
	}

	Lua::TerminateScript(FileName);
}

bool LuaWindow::CheckScript(wxString scriptName)
{
	if (!Core::IsRunningAndStarted())
	{
		wxMessageBox("A game needs to be running in order to execute scripts!");
		return false;
	}

	if (scriptName == wxEmptyString || scriptName.Len() < 3)
	{
		wxMessageBox("No script selected!");
		return false;
	}

	return true;
}

void LuaWindow::OnSelectionChanged(wxCommandEvent& event)
{
	if (event.GetId() == 1) //Script Selection
	{
	}
}

void LuaWindow::OnButtonPressed(wxCommandEvent& event)
{
	wxString selectedScriptName = m_choice_script->GetStringSelection();

	if (event.GetId() == 2) //Start
		StartScript(selectedScriptName);

	if (event.GetId() == 3) //Cancel
		CancelScript(selectedScriptName);
}

void LuaWindow::OnOptButtonPressed(wxCommandEvent& event)
{
	wxButton* b = (wxButton*) event.GetEventObject();
	wxChoice* cc = (wxChoice*)b->GetClientData();
	wxString selectedScriptName = cc->GetStringSelection();

	std::string filename = File::GetExeDirectory() + "\\Scripts\\" + selectedScriptName.ToStdString() + ".opt";
	if (File::Exists(filename))
		(new OptionsDialog(this, selectedScriptName, filename))->Show();
	else
		wxMessageBox("Script options file does not exist!");
}

void LuaWindow::Shown()
{
	//Refresh Script List
	m_choice_script->Clear();

	//Find all Lua files
	CFileSearch::XStringVector Directory;
	Directory.push_back(File::GetExeDirectory() + "\\Scripts");

	CFileSearch::XStringVector Extension;
	Extension.push_back("*.lua");

	CFileSearch FileSearch(Extension, Directory);
	const CFileSearch::XStringVector& rFilenames = FileSearch.GetFileNames();

	if (rFilenames.size() > 0)
	{
		for (u32 i = 0; i < rFilenames.size(); i++)
		{
			std::string FileName;
			SplitPath(rFilenames[i], nullptr, &FileName, nullptr);

			if (FileName.substr(0, 1).compare("_") && FileName.compare("Superswim"))
			{
				m_choice_script->Append(StrToWxStr(FileName + ".lua"));
				for (int i = 0; i < 10; i++)
				{
					m_choice_scriptSlots[i]->Append(StrToWxStr(FileName + ".lua"));
				}
			}
		}
	}
}

void LuaWindow::OnCloseWindow(wxCloseEvent& event)
{
	if (event.CanVeto())
	{
		event.Skip(false);
		Show(false);
	}
}


BEGIN_EVENT_TABLE(LuaWindow::OptionsDialog, wxDialog)

EVT_TEXT(10, LuaWindow::OptionsDialog::OnTextChanged) // Changed
EVT_BUTTON(11, LuaWindow::OptionsDialog::OnButtonPressed) // Save
EVT_BUTTON(12, LuaWindow::OptionsDialog::OnButtonPressed) // Discard

END_EVENT_TABLE()

LuaWindow::OptionsDialog::OptionsDialog(wxWindow* parent, const wxString& scriptName, std::string filename, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
: wxDialog(parent, id, wxString::Format("Options file %s.opt", scriptName), pos, size, style), m_scriptName(scriptName), m_filename(filename), m_changed(false)
{
	wxBoxSizer* sizer;
	sizer = new wxBoxSizer(wxVERTICAL);
	// comment hint
	wxString hintT("HINT:");
	m_commentHintT = new wxStaticText(this, wxID_ANY, hintT, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
	wxString hint("Everything between '--' and the end of the line is considered a comment and will be ignored by the Lua engine (:");
	m_commentHint = new wxStaticText(this, wxID_ANY, hint, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
	m_commentHint->Wrap(300);
	sizer->Add(m_commentHintT, 0, wxALIGN_CENTER | wxUP, 5);
	sizer->Add(m_commentHint, 0, wxALIGN_CENTER, 0);
	// text box
	std::string filecontent;
	File::ReadFileToString(m_filename, filecontent);
	m_textBox = new wxTextCtrl(this, 10, wxString(filecontent), wxDefaultPosition, wxSize(300, 200), wxTE_MULTILINE | wxTE_DONTWRAP | wxTE_PROCESS_TAB);
	sizer->Add(m_textBox, 0, wxALIGN_CENTER | wxALL, 5);
	// buttons
	wxBoxSizer* buttonsizer;
	buttonsizer = new wxBoxSizer(wxHORIZONTAL);
	m_buttonSave = new wxButton(this, 11, wxT("Save"));
	m_buttonDiscard = new wxButton(this, 12, wxT("Close"));
	buttonsizer->Add(m_buttonSave, 0, wxALIGN_RIGHT);
	buttonsizer->Add(m_buttonDiscard, 0, wxALIGN_RIGHT);

	sizer->Add(buttonsizer, 0, wxALIGN_CENTER | wxDOWN, 5);
	SetSizer(sizer);
	Layout();
	sizer->Fit(this);
	Centre(wxBOTH);

	Bind(wxEVT_CLOSE_WINDOW, &LuaWindow::OptionsDialog::OnCloseWindow, this);
}

void LuaWindow::OptionsDialog::saveToFile()
{
	File::WriteStringToFile(m_textBox->GetValue().ToStdString(), m_filename);
	m_changed = false;
}

void LuaWindow::OptionsDialog::OnButtonPressed(wxCommandEvent& event)
{
	if (event.GetId() == 11) // Save
		saveToFile();

	if (event.GetId() == 12) // Close
		Close();
}

void LuaWindow::OptionsDialog::OnTextChanged(wxCommandEvent& event)
{
	m_changed = true;
}

void LuaWindow::OptionsDialog::OnCloseWindow(wxCloseEvent& event)
{
	if (!m_changed || wxMessageBox("Changes will be discarded", "Discard?", wxOK | wxCANCEL, this) == wxOK)
		Destroy();
}
