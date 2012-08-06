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


#ifndef ABOUT_H
#define ABOUT_H

#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/notebook.h>
#include <wx/toolbar.h>
#include <wx/print.h>

void ShowAboutDialog(wxWindow* parent);

class AboutDialog : public wxDialog  {
public:
	AboutDialog(wxWindow* parent);

private:

};

#endif //MAIN_WINDOW_H
