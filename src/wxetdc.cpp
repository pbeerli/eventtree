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


#include "wxetdc.h"


wxetdc::~wxetdc()
{
}

wxetdc::wxetdc(wxDC* dc) 
{
	minx = -1;
	maxx = 0;
	miny = 0;
	maxy = 1;
	this->dc = dc;
	deviceMinX = 0;
	deviceMinY = 0;
	if ( dc != NULL ) 
		SetDC(dc);	
}

void wxetdc::SetDC(wxDC* dc) 
{
	int x,y;
//	dc->SetUserScale(1,1);
	dc->GetSize(&x,&y);
	deviceMaxX = x;
	deviceMaxY = y;
	dc->SetFont(*wxSWISS_FONT);
	this->dc = dc;
}


void wxetdc::NativeDrawTriangle(double x1, double y1, double x2, double y2,double x3, double y3)
{	
	wxPoint points[] = { wxPoint((int)x1,(int)y1),wxPoint((int)x2, (int)y2), wxPoint((int)x3, (int)y3)}; 
	
	
	wxBrush savebrush = dc->GetBrush();
	dc->SetBrush(*wxBLACK_BRUSH);
	
	dc->DrawPolygon(3, points);
	
	dc->SetBrush(savebrush);
}

void wxetdc::NativeGetTextExtent(wxString text, double* width, double* height)
{
	int x,y;
	dc->GetTextExtent(text,&x,&y);
	*width = x;
	*height = y;	
}

void wxetdc::NativeFillPolygon(int points, double *x, double*y, int color)
{
    int red,green,blue;

    red = (color & 0xff0000) >> 16;
    green = (color & 0xff00) >> 8;
    blue = color & 0xff;

    wxColour col(red,green,blue);
    wxBrush oldBrush = dc->GetBrush();
    wxBrush newBrush(col);
    dc->SetBrush(newBrush);
    wxPoint* p = new wxPoint[points];
    for ( int i = 0 ; i < points ; i++ ) {
        p[i].x = (int)x[i]; 
        p[i].y = (int)y[i]; 
    }
    dc->DrawPolygon(points,p);
    dc->SetBrush(oldBrush);
}
