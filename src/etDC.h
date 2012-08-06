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


#ifndef _ETDC_H_
#define _ETDC_H_

#include <wx/string.h>

class etDC
{
public:
	etDC();
	virtual ~etDC()=0;
	virtual void DrawLine(double x1, double y1, double x2, double y2);
	virtual void DrawText(wxString text, double x1, double x2);
	virtual void DrawArrow(double x1, double y1, double x2, double y2);
	virtual void GetTextExtent(wxString text, double *width, double* height);
    virtual void FillPolygon(int points, double* x, double* y, 
            int color);
	 
	virtual void SetBounds(double minx, double miny, double maxx, double maxy) 	
		{SetMinX(minx);	SetMinY(miny);	SetMaxX(maxx);  SetMaxY(maxy);};
	virtual void SetMinX(double minx){this->minx = minx;};
	virtual void SetMinY(double miny){this->miny = miny;};
	virtual void SetMaxX(double maxx){this->maxx = maxx;};
	virtual void SetMaxY(double maxy){this->maxy = maxy;};
	virtual void SetDeviceMargins(double top, double left, double right, 
            double bottom);
	virtual void SetSourceCoordinates(double minx,double maxx, double miny, 
            double maxy);
protected:
	virtual double translateX(double x);
	virtual double translateY(double y); 
	virtual double backTranslateX(double x);
	virtual double backTranslateY(double y); 
	
	virtual void NativeDrawLine(double x1, double y1, double x2, double y2) = 0;
	virtual void NativeDrawText(wxString text, double x1, double x2) = 0;
	virtual void NativeDrawArrow(double x1, double y1, double x2, double y2);
	virtual void NativeGetTextExtent(wxString text, double* width, 
            double* height) = 0;
	virtual void NativeDrawTriangle(double x1,double y1, double x2, double y2, 
            double x3, double y3) = 0;
    virtual void NativeFillPolygon(int points, double* x, double* y, 
            int color) = 0;
	
protected:
	double minx,miny,maxx,maxy;
	double leftMargin, topMargin, rightMargin, bottomMargin;
	double deviceMinX, deviceMinY, deviceMaxX, deviceMaxY;
};

#endif //_ETDC_H_
