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


#include "WmfDC.h"
#include "PaperOptionsDialog.h"
#include <wx/metafile.h>

#ifdef __MXMSW__
static double marginPercent = .05;
#endif

WmfDC::WmfDC(wxString file)
{
#ifdef __WXMSW__
	dc = new wxMetafileDC(file);
	SetDeviceMargins(dc->MaxY()*marginPercent, dc->MaxX()*marginPercent, 
		dc->MaxX()*marginPercent, dc->MaxY()*marginPercent);
	deviceMaxX = dc->MaxX();
	deviceMaxY = dc->MaxY();
	SetDC(dc);
	this->file = file;
#endif
}

WmfDC::~WmfDC()
{
#ifdef __WXMSW__
	WxMetafile* wmf = dc->Close();
	wxMakeMetafilePlaceable(file,0,0,dc->MaxX(),dc->MaxY());
	delete dc;
#endif
}
