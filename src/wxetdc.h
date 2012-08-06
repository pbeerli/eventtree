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


#ifndef WXETDC_H_
#define WXETDC_H_

#include "etDC.h"
#include <wx/dc.h>

class wxetdc : public etDC
{
public:
	wxetdc(wxDC* dc = NULL);	
	virtual ~wxetdc();
	virtual void NativeDrawLine(double x1, double y1, double x2, double y2) { dc->DrawLine((int)x1,(int)y1,(int)x2,(int)y2); }
	virtual void NativeDrawText(wxString text, double x, double y){dc->DrawText(text, (int)x, (int)y);}
	virtual void NativeDrawTriangle(double x1, double y1, double x2, double y2,double x3, double y3);
	virtual void NativeGetTextExtent(wxString text, double *width, double* height);
	virtual void NativeFillPolygon(int points, double *x, double*y, int color);
protected:
	virtual void SetDC(wxDC* dc);
	wxDC* dc;
};

#endif /*WXETDC_H_*/
