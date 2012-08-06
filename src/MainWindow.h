//    Event Tree, a program to draw events on evolutionary trees
//    Copyright (C) 2006  Michal Palczewski

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin Street, Fifth Floor, 
//    Boston, MA  02110-1301  USA


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/notebook.h>
#include <wx/toolbar.h>
#include <wx/print.h>

class MainWindow : public wxFrame  {
public:
	MainWindow();
	void AddView (wxString file);

private:
	void UpdateWindow();

	// Event Handling 
	void OnSave(wxCommandEvent& event);
	void OnNext(wxCommandEvent& event);
	void OnPrev(wxCommandEvent& event);
	void OnClose(wxCommandEvent& event);
	void OnPrintPreview(wxCommandEvent& event);
	void OnPageSetup(wxCommandEvent& event);
	void OnPrint(wxCommandEvent& event);
	void OnFileQuit	(wxCommandEvent& event); 
	void OnFileOpen (wxCommandEvent& event); 
	void OnHelpAbout (wxCommandEvent& event); 
	void OnNotebook(wxNotebookEvent& event) {UpdateWindow();}

	DECLARE_EVENT_TABLE();

	wxToolBar* MakeToolBar();
	wxMenuBar* CreateMenuBar();
	wxNotebook* notebook;
	wxPrintData* printData;

	wxPageSetupData* pageSetupData;
	wxToolBarToolBase* nextButton;
	wxToolBarToolBase* prevButton;
	wxToolBarToolBase* closeButton;
	wxToolBarToolBase* printButton;

	wxMenuItem* printMenu;
	wxMenuItem* printPreviewMenu;
	wxMenuItem* pageSetupMenu;
	wxMenuItem* closeMenu;
	wxMenuItem* exportMenuItem;
};

#endif //MAIN_WINDOW_H
