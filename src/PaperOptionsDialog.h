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


#ifndef PDFOPTIONSDIALOG_H_
#define PDFOPTIONSDIALOG_H_

#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/event.h>

enum MeasurementUnits {
	PIXELS=0, MM, INCHES
};

class PaperOptionsDialog : public wxDialog
{
public:
	PaperOptionsDialog(wxWindow* parent); 
	virtual ~PaperOptionsDialog();
	int GetHeight();
	int GetWidth();
	int GetTopMargin();
	int GetLeftMargin();
	int GetRightMargin();
	int GetBottomMargin();
	void OnUnits(wxCommandEvent& evt);
	void OnPaper(wxCommandEvent& evt);
private:
	double ConvertUnits(double unit);	
	double BackConvertUnits(double pixels);
	wxTextCtrl *topEntry, *leftEntry, *rightEntry, *bottomEntry, *widthEntry, *heightEntry;
	wxChoice *paperChoice, *measurementChoice;
	MeasurementUnits currentUnits;
	DECLARE_EVENT_TABLE();
};

#endif /*PDFOPTIONSDIALOG_H_*/
