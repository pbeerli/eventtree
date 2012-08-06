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


#include "PaperOptionsDialog.h"

#include <wx/panel.h>
#include <wx/gbsizer.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/choice.h>
#include <wx/textctrl.h>
#include <wx/statline.h>
#include <wx/bmpbuttn.h>
#include <wx/artprov.h>

 
// convenience macros taken from wx samples
#define POS(r, c)        wxGBPosition(c,r) //I made it backwards because IMO x coordinate should come first
#define SPAN(r, c)       wxGBSpan(r,c)

const int defaultMeasurement = 0;
const int defaultPaper = 0;	

const char* letterHeight = "792";
const char* letterWidth = "612";
const char* a4Height = "842";
const char* a4Width = "596";

const char* defaultHeight = letterHeight;
const char* defaultWidth = letterWidth;
const char* defaultMargin = "72";

const char* title = "Paper Options";

const double dpi = 72;
const double dpmm = 2.84;

const int paperChoiceID = 100;
const int unitsChoiceID = 101;

PaperOptionsDialog::PaperOptionsDialog(wxWindow* parent):wxDialog(parent, wxID_ANY, wxString::FromAscii(title), wxPoint(-1,-1),
	wxSize(-1,-1), wxCAPTION | wxSYSTEM_MENU)
{
	wxPanel* p = new wxPanel(this, wxID_ANY);
	
	wxBoxSizer* mainSizer =  new wxBoxSizer(wxVERTICAL);
	
	wxStaticBoxSizer* measurementBox = new wxStaticBoxSizer(wxHORIZONTAL, p, _T("Measurement Units"));
	mainSizer->Add(measurementBox,wxSizerFlags(0).Expand().Border(wxALL,10));
	
	wxArrayString measurementChoices;
	measurementChoices.Add(_T("pixels"));
	measurementChoices.Add(_T("mm"));
	measurementChoices.Add(_T("inches"));
	measurementChoice = new wxChoice(p, unitsChoiceID, wxPoint(-1,-1),wxSize(-1,-1),measurementChoices);
	measurementChoice->SetSelection(defaultMeasurement);
	currentUnits = PIXELS;
	measurementBox->Add(measurementChoice,wxSizerFlags().Expand());
	
	wxStaticBoxSizer* paperBox = new wxStaticBoxSizer(wxHORIZONTAL, p, _T("Paper Options"));
	mainSizer->Add(paperBox,wxSizerFlags(0).Expand().Border(wxALL,10));
	wxGridBagSizer* paperGridSizer = new wxGridBagSizer();
	paperBox->Add(paperGridSizer);
	
	wxArrayString paperChoices;
	paperChoices.Add(_T("Letter"));
	paperChoices.Add(_T("A4"));
	paperChoices.Add(_T("Custom"));
	paperChoice = new wxChoice(p, paperChoiceID,wxPoint(-1,-1),wxSize(-1,-1),paperChoices);
	paperChoice->SetSelection(defaultPaper);
	paperGridSizer->Add(paperChoice,POS(0,1),wxDefaultSpan,0,5);
	
	paperGridSizer->Add(new wxStaticText(p,wxID_ANY,_T("width")),POS(1,0),wxDefaultSpan,wxALIGN_CENTER_HORIZONTAL);
	paperGridSizer->Add(new wxStaticText(p,wxID_ANY,_T("height")),POS(2,0),wxDefaultSpan,wxALIGN_CENTER_HORIZONTAL);
	
	widthEntry = new wxTextCtrl(p, wxID_ANY, wxString::FromAscii(defaultWidth));
	heightEntry = new wxTextCtrl(p, wxID_ANY, wxString::FromAscii(defaultHeight));

	widthEntry->Disable();
	heightEntry->Disable();

	paperGridSizer->Add(widthEntry,POS(1,1),wxDefaultSpan,0,5);
	paperGridSizer->Add(heightEntry,POS(2,1),wxDefaultSpan,0,5);


	wxStaticBoxSizer* marginBox = new wxStaticBoxSizer(wxHORIZONTAL, p, _T("Margin Options"));
	mainSizer->Add(marginBox,wxSizerFlags(0).Expand().Border(wxALL,10));
	wxGridBagSizer* marginGridSizer = new wxGridBagSizer();
	marginBox->Add(marginGridSizer);
	
	marginGridSizer->Add(new wxStaticText(p, wxID_ANY,_T("top")), POS(1,0), wxDefaultSpan, wxALIGN_CENTER_HORIZONTAL);
	marginGridSizer->Add(new wxStaticText(p, wxID_ANY,_T("left")),POS(0,1), wxDefaultSpan, wxALIGN_CENTER_HORIZONTAL);
	marginGridSizer->Add(new wxStaticText(p, wxID_ANY,_T("right")), POS(2,1), wxDefaultSpan, wxALIGN_CENTER_HORIZONTAL);
	marginGridSizer->Add(new wxStaticText(p, wxID_ANY,_T("botom")), POS(1,4), wxDefaultSpan, wxALIGN_CENTER_HORIZONTAL);
	
	topEntry = new wxTextCtrl(p, wxID_ANY,wxString::FromAscii(defaultMargin));
	leftEntry = new wxTextCtrl(p, wxID_ANY,wxString::FromAscii(defaultMargin));
	rightEntry = new wxTextCtrl(p, wxID_ANY,wxString::FromAscii(defaultMargin));
	bottomEntry = new wxTextCtrl(p, wxID_ANY,wxString::FromAscii(defaultMargin));

	marginGridSizer->Add(topEntry,POS(1,1),wxDefaultSpan,0,5);
	marginGridSizer->Add(leftEntry,POS(0,2),wxDefaultSpan,0,5);
	marginGridSizer->Add(rightEntry,POS(2,2),wxDefaultSpan,0,5);
	marginGridSizer->Add(bottomEntry,POS(1,3),wxDefaultSpan,0,5);

	mainSizer->Add(new wxStaticLine(p, wxID_ANY, wxDefaultPosition, wxSize(3,3), wxHORIZONTAL),
		wxSizerFlags().Expand().Border(wxALL,10));	

	wxBoxSizer* buttonBox = new wxBoxSizer(wxHORIZONTAL);
	mainSizer->Add(buttonBox,wxSizerFlags().Right().Border(wxALL,10));
	
	buttonBox->Add(new wxButton(p,wxID_OK));

	p->SetSizer(mainSizer);
	mainSizer->SetSizeHints(this);
}


PaperOptionsDialog::~PaperOptionsDialog()
{
}


int PaperOptionsDialog::GetTopMargin()
{
	return (int)ConvertUnits(atof(topEntry->GetLineText(0).ToAscii()));	
}


int PaperOptionsDialog::GetLeftMargin() 
{	
	return (int)ConvertUnits(atof(leftEntry->GetLineText(0).ToAscii()));		
}


int PaperOptionsDialog::GetRightMargin() 
{
	return (int)ConvertUnits(atof(rightEntry->GetLineText(0).ToAscii()));	
}


int PaperOptionsDialog::GetBottomMargin()
{
	return (int)ConvertUnits(atof(bottomEntry->GetLineText(0).ToAscii()));	
}


int PaperOptionsDialog::GetHeight()
{
	return (int)ConvertUnits(atof(heightEntry->GetLineText(0).ToAscii()));	
}


int PaperOptionsDialog::GetWidth()
{
	return (int)ConvertUnits(atof(widthEntry->GetLineText(0).ToAscii()));	
}


double PaperOptionsDialog::ConvertUnits(double unit)
{
	int newunit = (int)unit; // Just a little something for the compiler
	switch ( currentUnits )  {
		case PIXELS:
			return (int)unit;
		case MM:
			return (dpmm * unit); 
		case INCHES:
			return (dpi * unit); 
	}
	return newunit;
}

double PaperOptionsDialog::BackConvertUnits(double pixels)
{
	double newunit = pixels; // Just a little something for the compiler
	switch( currentUnits ) {
		case PIXELS:
			return pixels;
		case MM:
			return pixels / dpmm;
		case INCHES:
			return pixels / dpi;
	}
	return newunit;
}


void PaperOptionsDialog::OnPaper(wxCommandEvent& evt)
{
	int selection = paperChoice->GetSelection();
	switch (selection)  {
		case 0: // Letter
			heightEntry->Clear();
			heightEntry->AppendText(wxString::Format(_T("%.1f"),BackConvertUnits(atoi(letterHeight))));
			widthEntry->Clear();
			widthEntry->AppendText(wxString::Format(_T("%.1f"),BackConvertUnits(atoi(letterWidth))));
			heightEntry->Disable();
			widthEntry->Disable();
			break;	
		case 1: // A4
			heightEntry->Clear();
			heightEntry->AppendText(wxString::Format(_T("%.1f"),BackConvertUnits(atoi(a4Height))));
			widthEntry->Clear();
			widthEntry->AppendText(wxString::Format(_T("%.1f"),BackConvertUnits(atoi(a4Width))));			
			
			heightEntry->Disable();
			widthEntry->Disable();
			break;
		case 2: //Custom
			heightEntry->Enable();
			widthEntry->Enable();
			break;
	}
}

void PaperOptionsDialog::OnUnits(wxCommandEvent& evt)
{
	int selection = measurementChoice->GetSelection();
	double width = GetWidth();
	double height = GetHeight();
	double top = GetTopMargin();
	double left = GetLeftMargin();
	double right = GetRightMargin();
	double bottom = GetBottomMargin();
	switch (selection) {
		case 0:
			currentUnits = PIXELS;
			break;
		case 1:
			currentUnits = MM;
			break;
		case 2:
			currentUnits = INCHES;
			break;
	}
	widthEntry->Clear();
	heightEntry->Clear();
	topEntry->Clear();
	leftEntry->Clear();
	rightEntry->Clear();
	bottomEntry->Clear();
	
	widthEntry->AppendText(wxString::Format(_T("%.2f"),BackConvertUnits(width)));
	heightEntry->AppendText(wxString::Format(_T("%.2f"),BackConvertUnits(height)));
	topEntry->AppendText(wxString::Format(_T("%.2f"),BackConvertUnits(top)));
	leftEntry->AppendText(wxString::Format(_T("%.2f"),BackConvertUnits(left)));
	rightEntry->AppendText(wxString::Format(_T("%.2f"),BackConvertUnits(right)));
	bottomEntry->AppendText(wxString::Format(_T("%.2f"),BackConvertUnits(bottom)));
	OnPaper(evt);
}

BEGIN_EVENT_TABLE(PaperOptionsDialog,wxDialog)
	EVT_CHOICE(paperChoiceID, PaperOptionsDialog::OnPaper)	
	EVT_CHOICE(unitsChoiceID, PaperOptionsDialog::OnUnits)	
END_EVENT_TABLE()
