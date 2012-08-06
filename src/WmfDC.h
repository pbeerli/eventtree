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


#ifndef WMFDC_H_
#define WMFDC_H_

#include "wxetdc.h"

class WmfDC : public wxetdc
{
public:
    WmfDC(wxString file);
    virtual ~WmfDC();
private:
	wxString file;
};

#endif /*XMLDC_H_*/
