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


#include "EpsDC.h"
#include <wx/ffile.h>

EpsDC::EpsDC(PaperOptionsDialog& pod, wxString filename) :PdfDC(pod,filename)
{
	curdoc = _T("");
	
	char *preamble = "%!PS-Adobe-3.0 EPSF-3.0\n"
		"%%Creator: (ET Event Tree)\n"
		"%%Title: Tree\n" 	
		"%%EndComments\n"
		"%%BeginProlog\n"
		"%%BoundingBox:";
	curdoc.Append(wxString::FromAscii(preamble));
	curdoc.Append(wxString::Format(wxString::FromAscii(" 0 0 %f %f"),deviceMaxX, deviceMinY));
	preamble = "\n"
		"/T {\n"
		"gsave\n"
		"\tmoveto show grestore\n"
		"\t} bind def\n"
		"\n"
		"/L {\n"
		"\tmoveto lineto stroke\n"
		"\t} bind def\n"
		"/R {\n"
		"\tmoveto lineto lineto closepath fill\n"
		"\t} bind def\n"
		"\n"
		"%%EndProlog\n";
	curdoc.Append(wxString::FromAscii(preamble));
	curdoc.Append(wxString::Format(wxString::FromAscii("/Helvetica findfont %d scalefont setfont\n"),(int)fsize));
}


EpsDC::~EpsDC()
{
	curdoc.Append(_T("%%EOF\n"));
	wxFFile ffile(filename,_T("w"));
	ffile.Write(curdoc.ToAscii(),strlen(curdoc.ToAscii()));
	ffile.Close();
	filename = _T("");
}


void EpsDC::NativeDrawLine(double x1, double y1, double x2, double y2)
{
	curdoc.Append(wxString::Format(wxString::FromAscii("%f %f %f %f L\n"), x1, y1, x2, y2));
}


void EpsDC::NativeDrawText(wxString text, double x1, double y1)
{
	curdoc.Append(wxString::Format(wxString::FromAscii("(%s) %f %f T\n"), text.c_str(), x1, y1 - fsize + fspace));
}

void EpsDC::NativeDrawTriangle(double x1, double y1, double x2, double y2,double x3, double y3)
{
	curdoc.Append(wxString::Format(wxString::FromAscii("%f %f %f %f %f %f R\n"), x1, y1, x2, y2, x3, y3));
}
