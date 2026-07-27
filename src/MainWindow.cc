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



#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "MainWindow.h"
#include <stdlib.h>

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/artprov.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/print.h>
#include <wx/printdlg.h>
#include <wx/dcprint.h>
#include "wxetdc.h"
#include "Treeview.h"
#include "TreeFileReader.h"
#include "eticon.xpm"
#include "printdc.h" 
#include "PaperOptionsDialog.h"
#include "PdfDC.h"
#include "EpsDC.h"
#include "SvgDC.h"
#include "WmfDC.h"
#include "DnDFile.h"
#include "AboutDialog.h"

static const int FileQuit = 1;
static const int FileOpen = 2;
static const int FileClose =5;
static const int PrintPreview = 6;
static const int PrevTree = 7;
static const int NextTree = 8;
static const int Print = 9;
static const int PageSetup = 10;
static const int Export = 14;
static const int ToolOpen = 15;
static const int notebookId = 16;


// shamelessly borrowed from the printing.cpp/printing.h from the wxwidgets 
// samples
class MyPrintout: public wxPrintout
{
	public:
		MyPrintout(const wxChar *title, Treeview* tv, wxPrintData* pd);
		bool OnPrintPage(int page);
		bool hasPage(int page);
		void GetPageInfo(int* minPage, int* maxPage, int *selPageFrom, 
			int* selPageTo);
	private:
		Treeview* tv;			
		wxPrintData* printdata;
};


MyPrintout::MyPrintout(const wxChar* title, Treeview* tv, wxPrintData* pd)
	:wxPrintout(title) 
{
	this->tv = tv;
	printdata = pd;
}


bool MyPrintout::OnPrintPage(int page)
{
	
	wxDC* dc = GetDC();

	wxetdc *etdc = new printdc(dc);
	if ( dc ) { 
		tv->DrawTree(*etdc);
		return true;
	} 
	else return false;
}


bool MyPrintout::hasPage(int pageNum) 
{
	return (pageNum == 1);
}


void MyPrintout::GetPageInfo(int* minPage, int* maxPage, int* selPageFrom, 
	int* selPageTo)
{
	*minPage = 1;
	*maxPage = 1;
	*selPageFrom = 1;
	*selPageTo = 1;
}


MainWindow::MainWindow() :wxFrame(NULL, -1, wxString::FromAscii(PACKAGE), 
	wxPoint(50,50), wxSize(550,540),wxDEFAULT_FRAME_STYLE ) 
{
	SetStatusBar(new wxStatusBar(this,wxID_ANY));
	SetMenuBar(CreateMenuBar());
	SetToolBar(MakeToolBar());
	notebook = new wxNotebook(this,notebookId);	
	wxIcon* ico= new wxIcon(eticon);
	SetIcon(*ico);
	printData = new wxPrintData;
	pageSetupData = new wxPageSetupDialogData;
	SetDropTarget(new DnDFile(*this));
	UpdateWindow();
	Show(true);
	SendSizeEvent();
} 


void MainWindow::OnPageSetup(wxCommandEvent& event)
{
	*pageSetupData = *printData;
	wxPageSetupDialog pageSetupDialog(this, pageSetupData);
	pageSetupDialog.ShowModal();
	(*printData) =  pageSetupDialog.GetPageSetupData().GetPrintData();
    (*pageSetupData) = pageSetupDialog.GetPageSetupData();
}


void MainWindow::AddView(wxString file) 
{
	TreeFileReader* tfr = new TreeFileReader((const char*)file.ToAscii());
	if ( tfr->GetNumTrees() == 0 )  {
		delete tfr;
		wxMessageBox(_T("Unable to find a tree in that file"),
				_T("Invalid file"),wxOK,this);
		return;
	}
	Treeview* tv = new Treeview(tfr,notebook);
	wxFileName wfn(file);	
	notebook->AddPage(tv,wfn.GetFullName());
	notebook->SetSelection(notebook->GetPageCount()-1);
	UpdateWindow();
}


wxToolBar* MainWindow:: MakeToolBar() 
{
	wxToolBar* tb = CreateToolBar();
	
	tb->AddTool(FileOpen,wxT("Open"),
		wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_TOOLBAR));
	closeButton = tb->AddTool(FileClose,wxT("Close"),
		wxArtProvider::GetBitmap(wxART_CROSS_MARK, wxART_TOOLBAR));
	printButton = tb->AddTool(Print, wxT("Print"), 
		wxArtProvider::GetBitmap(wxART_PRINT, wxART_TOOLBAR));
	tb->AddSeparator();
	prevButton = tb->AddTool(PrevTree, wxT("Previous"), 
		wxArtProvider::GetBitmap(wxART_GO_BACK, wxART_TOOLBAR));
	nextButton = tb->AddTool(NextTree, wxT("Forward"), 
		wxArtProvider::GetBitmap(wxART_GO_FORWARD, wxART_TOOLBAR));
	tb->AddSeparator();
	tb->AddTool(FileQuit, wxT("Quit"),
		wxArtProvider::GetBitmap(wxART_QUIT, wxART_TOOLBAR));
	tb->Realize();
	return tb; 
}


wxMenuBar* MainWindow::CreateMenuBar() 
{
	wxMenuBar* mb = new wxMenuBar();
	
	wxMenu* fileMenu = new wxMenu();
	mb->Append(fileMenu, wxT("&File"));
	
	wxMenuItem* mi = new wxMenuItem(fileMenu, FileOpen, wxT("Open\tCTRL+O"));
	mi->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_MENU));	
	fileMenu->Append(mi);
	
	closeMenu = new wxMenuItem(fileMenu, FileClose, wxT("Close\tCTRL+W"));
	closeMenu->SetBitmap(wxArtProvider::GetBitmap(wxART_CROSS_MARK, 
				wxART_MENU));
	fileMenu->Append(closeMenu);
	
	fileMenu->AppendSeparator();
	
	printPreviewMenu = new wxMenuItem(fileMenu, PrintPreview, 
			wxT("Print Preview"));
	fileMenu->Append(printPreviewMenu);
	
	pageSetupMenu = new wxMenuItem(fileMenu, PageSetup, wxT("Page &Setup"));
	fileMenu->Append(pageSetupMenu);
	
	printMenu = new wxMenuItem(fileMenu, Print, wxT("Print\tCTRL+P"));
	printMenu->SetBitmap(wxArtProvider::GetBitmap(wxART_PRINT, wxART_MENU));
	fileMenu->Append(printMenu);
	
	
	fileMenu->AppendSeparator();
	
	exportMenuItem = new wxMenuItem(fileMenu, Export, _T("&Export ..."));
	fileMenu->Append(exportMenuItem);

	fileMenu->AppendSeparator();
	
	mi = new wxMenuItem(fileMenu, FileQuit, wxT("Quit\tCTRL+Q"));
	mi->SetBitmap(wxArtProvider::GetBitmap(wxART_QUIT, wxART_MENU));	
	fileMenu->Append(mi);
	
	wxMenu* helpMenu = new wxMenu();
	mb->Append(helpMenu, wxString::FromAscii("&Help"));
	
	mi= new wxMenuItem(helpMenu, wxID_ABOUT, wxT("About\tCTRL+A"));
	//mi->SetBitmap(wxArtProvider::GetBitmap(wxART_ABOUT));
	helpMenu->Append(mi);	
	
	
	return mb;
}


void MainWindow::OnFileQuit	(wxCommandEvent& event) 
{
	exit(0);
}


void MainWindow::OnFileOpen (wxCommandEvent& event)
{
	wxFileDialog fd(this);
	long styleFD = wxFD_OPEN | wxFD_FILE_MUST_EXIST ; 
	fd.SetWindowStyle(fd.GetWindowStyle() | styleFD); 

	//fd.SetStyle(wxOPEN|wxFILE_MUST_EXIST);
	if ( fd.ShowModal() != wxID_OK )
		return;
	AddView(fd.GetPath());
}


void MainWindow::OnHelpAbout(wxCommandEvent& event) 
{
//	wxString about = wxT(PACKAGE " " VERSION 
//			"\nA tree drawing program for events"
//			"\nCopyright © 2006"
//			"\nMichal Palczewski, Florida State University"
//			"\nLicensed under the GPL"
//			"\nEvent Tree comes with ABSOLUTELY NO WARRANTY"
//			"\nThis is free software, and you are welcome to"
//			"\nredistribute it under certain conditions");

	//wxMessageBox(about, wxT("About" PACKAGE), wxOK, this);
    ShowAboutDialog(this);
}


void MainWindow::OnClose(wxCommandEvent& event)
{
	int current = notebook->GetSelection();
	if ( current == -1 ) return;
	notebook->DeletePage(current);
	UpdateWindow();
}


void MainWindow::OnPrintPreview(wxCommandEvent& event) 
{
	wxPrintDialogData printDialogData(* printData);
	
	Treeview* tv = (Treeview*)notebook->GetCurrentPage();
	if ( tv == NULL )
		return;
	size_t curPage = notebook->GetSelection(); 
	wxString curText = notebook->GetPageText(curPage);
	
    wxPrintPreview *preview = new wxPrintPreview(
    	new MyPrintout(curText,tv, printData),
    	new MyPrintout(curText,tv, printData), & printDialogData);
    if (!preview->Ok())
    {
        delete preview;
        wxMessageBox(_T("There was a problem previewing.\n"
					"Perhaps your current printer is not set correctly?"), 
        	_T("Previewing"), wxOK,this);
        return;
    }

    wxPreviewFrame *frame = new wxPreviewFrame(preview, this, 
    	_T("Print Preview"), wxPoint(100, 100), wxSize(600, 650));
    frame->Centre(wxBOTH);
    frame->Initialize();
    frame->Show();
	
}


void MainWindow::OnPrint(wxCommandEvent& event)
{
	wxPrintDialogData printDialogData(*printData);

    Treeview* tv = (Treeview*)notebook->GetCurrentPage();
    if ( tv == NULL )
	return;
    wxPrinter printer(& printDialogData);
    size_t curPage = notebook->GetSelection();
    wxString curText = notebook->GetPageText(curPage);
    MyPrintout printout(curText,tv, printData);
    if (!printer.Print(this, &printout, true /*prompt*/)) {
        if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
            wxMessageBox(_T("There was a problem printing.\n"
						"Perhaps your current printer is not set correctly?"), 
            			_T("Printing"), wxOK,this);
    }
    else 
        (*printData) = printer.GetPrintDialogData().GetPrintData();
}


void MainWindow::OnSave(wxCommandEvent& event)
{
	Treeview* tv = (Treeview*)notebook->GetCurrentPage();
	if ( tv == NULL ) 
		return;
	PaperOptionsDialog* pod = NULL;
	
	wxString filetype = _T("Scalable Vector Graphics (*.svg)|*.svg");
	filetype += _T("|Portable Document Format (*.pdf)|*.pdf");
	filetype += _T("|Encapsulated Postscript (*.eps)|*.eps");
#ifdef __WXMSW__
	filetype += _T("|Windows Meta File (*.wmf)|*.wmf");
#endif
	wxFileDialog fd(this,_T("Save As"),_T(""),_T(""),filetype,wxFD_SAVE);
	
	if ( fd.ShowModal() != wxID_OK)
		return;
	
	wxString path = fd.GetPath();
	wxString suffix = path.SubString(path.Length() - 4, path.Length()-1);

	if ( 
		suffix.CmpNoCase(_T(".pdf")) != 0 &&
		suffix.CmpNoCase(_T(".eps")) != 0 &&
		suffix.CmpNoCase(_T(".svg")) != 0 
#ifdef __WXMSW__
		&& suffix.CmpNoCase(_T("wmf")) != 0 
#endif 
		) {
		wxArrayString as;
		as.Add(_T("Portable Document Format(.pdf)"));
		as.Add(_T("Encapsulated Postscript(.eps)"));
		as.Add(_T("Scalable Vector Graphics(.svg)"));
#ifdef __WXMSW__
		as.Add(_T("Windows Meta File(.wmf)"));
#endif 
		int choice = wxGetSingleChoiceIndex(_T("Export as ..."), 
				_T("Format Choice"), as, this);
		switch ( choice ) { 
			case -1:
				return;
			case 0:
				suffix = _T(".pdf");
				break;
			case 1:
				suffix = _T(".eps");
				break;
			case 2:
				suffix = _T(".svg");
				break;
			case 3:
				suffix = _T(".wmf");
				break;
		}
		path += suffix;
	}
	
	if ( 
		suffix.CmpNoCase(_T(".pdf")) == 0 ||
		suffix.CmpNoCase(_T(".eps")) == 0 ||
		suffix.CmpNoCase(_T(".svg")) == 0 
			) {
		pod = new PaperOptionsDialog(this);
		pod->ShowModal();
	}

	etDC* dc = NULL;
	if ( suffix.CmpNoCase(_T(".pdf")) == 0 ) 
			dc = new PdfDC(*pod,path);
	if ( suffix.CmpNoCase(_T(".svg")) == 0 ) 
			dc = new SvgDC(*pod,path);
	if ( suffix.CmpNoCase(_T(".eps")) == 0 ) 
			dc = new EpsDC(*pod,path);
#ifdef __WXMSXW__
	if ( suffix.CmpNoCase(_T(".wmf")) == 0 ) 
			dc = new WmfDC(path);
#endif
	if ( dc != NULL ) {
		tv->DrawTree(*dc);
		delete dc;
	}
	if ( pod != NULL ) 
		delete pod;
}


void MainWindow::OnNext(wxCommandEvent& event)
{
	Treeview* tv = (Treeview*)notebook->GetCurrentPage();
	if ( tv  == NULL ) 
		return;
	tv->SetNext();
	UpdateWindow();
}


void MainWindow::OnPrev(wxCommandEvent& event)
{
	Treeview* tv = (Treeview*)notebook->GetCurrentPage();
	if ( tv == NULL )
		return;
	tv->SetPrev();
	UpdateWindow();
}


void MainWindow::UpdateWindow() 
{
	Treeview* tv=  (Treeview*)notebook->GetCurrentPage();
	if ( tv == NULL ) {
		nextButton->Enable(false);
		prevButton->Enable(false);
		closeButton->Enable(false);
		printButton->Enable(false);
		printMenu->Enable(false);
		printPreviewMenu->Enable(false);
		pageSetupMenu->Enable(false);
		closeMenu->Enable(false);
		exportMenuItem->Enable(false);
	} else {
		if ( tv->HasNext() ) 
			nextButton->Enable(true);
		else 
			nextButton->Enable(false);
		if ( tv->HasPrev() )
			prevButton->Enable(true);
		else
			prevButton->Enable(false);
		closeButton->Enable(true);
		printButton->Enable(true);
		printMenu->Enable(true);
		printPreviewMenu->Enable(true);
		pageSetupMenu->Enable(true);
		closeMenu->Enable(true);
		exportMenuItem->Enable(true);
	}
}


BEGIN_EVENT_TABLE(MainWindow,wxFrame)
	EVT_MENU (FileQuit, MainWindow::OnFileQuit)	
	EVT_MENU (FileOpen, MainWindow::OnFileOpen)	
	EVT_MENU (ToolOpen, MainWindow::OnFileOpen)	
	EVT_MENU (wxID_ABOUT, MainWindow::OnHelpAbout)
	EVT_MENU (FileClose, MainWindow::OnClose)
	EVT_MENU (PrintPreview, MainWindow::OnPrintPreview)
	EVT_MENU (PageSetup, MainWindow::OnPageSetup)
	EVT_MENU (Print, MainWindow::OnPrint)
	EVT_MENU (Export, MainWindow::OnSave)
	EVT_MENU (NextTree, MainWindow::OnNext)
	EVT_MENU (PrevTree, MainWindow::OnPrev)
	EVT_NOTEBOOK_PAGE_CHANGED(notebookId, MainWindow::OnNotebook)
END_EVENT_TABLE()
