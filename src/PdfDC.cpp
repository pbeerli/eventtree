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


#include "PdfDC.h"
#include <wx/msgdlg.h>
#include "PaperOptionsDialog.h"

// convenience macro. 
#define CATCHIT catch ( PDF_STD_EXCEPTION& e) { \
	wxMessageDialog(NULL, wxString::FromAscii(e.what()), _T("Error"), wxOK).ShowModal(); \
	exit(0); \
} 


PdfDC::PdfDC(PaperOptionsDialog& pod, wxString file):etDC(),filename(file)
{
	try {
		doc = new PdfDoc();
		doc->NewDoc();
		doc->AddType1Font(new PdfHelveticaFontDef());
		PdfPage* page = doc->AddPage();
	
		deviceMaxX = pod.GetWidth();
		deviceMinY = pod.GetHeight();
		page->SetSize((int)deviceMaxX,(int)deviceMinY);
		canvas = page->Canvas();
		canvas->SetFontAndSize("Helvetica", fsize);
	} CATCHIT
	deviceMinX = 0;
	deviceMaxY = 0;
	SetDeviceMargins(-pod.GetTopMargin(),pod.GetLeftMargin(),pod.GetRightMargin(),-pod.GetBottomMargin());
}


PdfDC::~PdfDC()
{
	try {
		if ( filename != _T("")) 
			doc->WriteToFile(filename.ToAscii());	
		doc->FreeDoc(true);	
	} CATCHIT
	delete doc;
}


void PdfDC::NativeDrawLine(double x1, double y1, double x2, double y2)
{
	try { 
		canvas->MoveTo(x1,y1);
		canvas->LineTo(x2,y2);
		canvas->Stroke();
	} CATCHIT
}


void PdfDC::NativeDrawText(wxString text, double x, double y)
{
	try {
		canvas->BeginText();
		canvas->MoveTextPos(x,y - fsize + fspace);
		canvas->ShowText(text.ToAscii());
		canvas->EndText();
	} CATCHIT
}


void PdfDC::NativeGetTextExtent(wxString text, double* width, double* height)
{
	try {
		canvas->BeginText();
		*width = canvas->TextWidth(text.ToAscii());
		*height = -fsize - 2*fspace;	
		canvas->EndText();
	} CATCHIT
}


void PdfDC::NativeDrawTriangle(double x1, double y1, double x2, double y2, double x3, double y3)
{
	try {
		canvas->MoveTo(x1,y1);
		canvas->LineTo(x2,y2);
		canvas->LineTo(x3,y3);
		canvas->ClosePath();
		canvas->Fill();
	} CATCHIT 
}

void PdfDC::NativeFillPolygon(int points, double *x, double *y,int color)
{
    try {
        canvas->SetRGBFill(125, 125, 125);
        canvas->MoveTo(x[0],y[0]);
        for (int i = 1 ; i < points ; i++ ) {
            canvas->LineTo(x[i],y[i]);
        }
        canvas->ClosePath();
        canvas->Fill();
    } CATCHIT
}


