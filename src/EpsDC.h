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


#ifndef EPSDC_H_
#define EPSDC_H_
#include "PdfDC.h"
#include "PaperOptionsDialog.h"

class EpsDC : public PdfDC
{
public:
	EpsDC(PaperOptionsDialog& pod, wxString filename);
	virtual ~EpsDC();
	virtual void NativeDrawLine(double x1, double y1, double x2, double y2);
	virtual void NativeDrawText(wxString text, double x1, double y1);
	virtual void NativeDrawTriangle(double x1, double y1, double x2, double y2,double x3, double y3);
private:
	wxString curdoc;
};

#endif /*EPSDC_H_*/
