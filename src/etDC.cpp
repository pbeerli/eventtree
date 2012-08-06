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


#include "etDC.h"
#include "math.h"

etDC::etDC() {
	leftMargin = 0;
	topMargin = 0;
	rightMargin = 0;
	bottomMargin = 0;
	
	minx = 0; 
	miny = 1;
	maxx = 0;
	maxy = 1;
}

etDC::~etDC()
{
}


void etDC::SetDeviceMargins(double top, double left, double right, double bottom) 
{  /* these are in the devices coordinates */
	leftMargin = left;
	topMargin = top;
	rightMargin = right;
	bottomMargin = bottom;
}

double etDC::translateX(double x) 
{
	double percentage = (x - minx) / (maxx - minx);
	double margintot = (rightMargin + leftMargin);
	double newx = deviceMinX + leftMargin + percentage * (deviceMaxX - deviceMinX - margintot);
	return newx; 
}

double etDC::backTranslateX(double x) 
{
	double margintot = (rightMargin + leftMargin);
	double percentage = (x - leftMargin - deviceMinX) / (deviceMaxX - deviceMinX - margintot);
	double newx = percentage * (maxx - minx) + minx;
	return newx;
}


double etDC::translateY(double y)
{
	double percentage = (y - miny) / (maxy - miny);
	double margintot = (topMargin + bottomMargin);
	double newy = deviceMinY + topMargin + percentage * ( deviceMaxY - deviceMinY - margintot);
	return newy;
}


double etDC::backTranslateY(double y) 
{	
	double margintot = (topMargin + bottomMargin);
	double percentage = (y - topMargin - deviceMinY) / (deviceMaxY - deviceMinY - margintot);
	double newy = percentage * (maxy - miny) + miny;
	return newy;
}

void etDC::SetSourceCoordinates(double minx, double maxx, double miny, double maxy) 
{
	SetMinX(minx);
	SetMaxX(maxx);
	SetMinY(miny);
	SetMaxY(maxy);	
}

void etDC::DrawLine(double x1, double y1, double x2, double y2)
{	
	int newx1 = (int)translateX(x1);
	int newy1 = (int)translateY(y1);
	int newx2 = (int)translateX(x2);
	int newy2 = (int)translateY(y2);
	NativeDrawLine(newx1,newy1,newx2,newy2);
}

void etDC::DrawArrow(double x1, double y1, double x2, double y2)
{
	double newx1 = translateX(x1);
	double newy1 = translateY(y1);
	double newx2 = translateX(x2);
	double newy2 = translateY(y2);
	NativeDrawArrow(newx1,newy1,newx2,newy2);
}


void etDC::FillPolygon(int points, double* x, double* y, int color)
{
    double *newx = new double[points];
    double *newy = new double[points];
    for ( int i = 0 ; i < points ; i++ ) {
        newx[i] = translateX(x[i]);
        newy[i] = translateY(y[i]);
    }
    NativeFillPolygon(points, newx,newy, color);
}


void etDC::DrawText(wxString text, double x, double y)
{
	int newx = (int)translateX(x);
	int newy = (int)translateY(y);
	NativeDrawText(text, newx+1, newy); //FIXME asses ugliness of +1 check formats
}

void etDC::GetTextExtent(wxString text, double *width, double* height)
{	
	double x,y;
	NativeGetTextExtent(text,&x,&y);
	*width  = x / (deviceMaxX - deviceMinX - leftMargin - rightMargin) * (maxx - minx);
	*height = y / (deviceMaxY - deviceMinY - topMargin - bottomMargin) * (maxy - miny);
//	*width = backTranslateX(x + deviceMinX + leftMargin) - minx;
//	*height = backTranslateY(y + deviceMinY + bottomMargin) - miny;
}

// I have it setup this way in case a future format has native support for an arrow primitive
void etDC::NativeDrawArrow(double x1, double y1, double x2, double y2)
{
	double hpercentage = 0.25;
	double wpercentage = 0.25;
	
	
	double arrowbottomx = x2 - hpercentage * (x2 - x1);
	double arrowbottomy = y2 - hpercentage * (y2 - y1);
	
	double length = sqrt( (x2 - x1)*(x2 - x1) + (y2 - y1)*(y2-y1));
	
	double width = length * wpercentage;
	
	double ly,ry,invslope;
	
	if ( x2 != x1 ) {
		double slope = (y2-y1) /(x2 -x1);
		invslope = -1/slope;
	}
	else {
		invslope = 0;
	}
		
	/* The tricky part
	 * http://mathforum.org/dr.math/faq/formulas/faq.trig.html
	 * basically we want to move a certain amount at a certain slope
	 * define a right triangle,  xmov, ymov and width are the dimensions
	 * arctan of the angle of interest is the invslope
	 * since we know the width, and two of the angles we can solve for
	 * solve for xmov and ymov 
	 * if speed becomes an issue, the obvious optimization is left ot the reader */
	double xmov = width/2 * 1 / sqrt(1 + invslope*invslope); 
	double ymov = width/2 * invslope / sqrt(1+invslope*invslope);
	
	double lx = arrowbottomx - xmov;
	double rx = arrowbottomx + xmov;
	
	ly = arrowbottomy - ymov;
	ry = arrowbottomy + ymov;

	NativeDrawLine(x1,y1,arrowbottomx,arrowbottomy);

	NativeDrawTriangle(x2,y2,lx,ly,rx,ry);
}

