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

#include "AboutDialog.h"
#include <config.h>
#include <wx/gbsizer.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include "eticon.xpm"
#include <wx/button.h>

void ShowAboutDialog(wxWindow* parent) 
{
    AboutDialog ad(parent);
    ad.ShowModal();
}

const wxString abtitle=wxT("About " PACKAGE); 
const wxString aboutstring=wxT("Event Tree " VERSION "\n"
		"Copyright 2006-2008\n"
		"Michal Palczewski\n"
		"Peter Beerli\n"
		"Florida State University\n"
		"Licensed under the GPL\n");

AboutDialog::AboutDialog(wxWindow* parent):wxDialog(parent,wxID_ABOUT,abtitle,
	wxDefaultPosition,wxDefaultSize,
	wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* hSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBitmap* icon = new wxBitmap(eticon);
    wxStaticBitmap* wsb = new wxStaticBitmap(this,wxID_ANY,*icon);
    wxStaticText *wst = new wxStaticText(this, wxID_ANY,aboutstring);
    hSizer->Add(wsb,0,wxALL,10);
    hSizer->Add(wst,0,wxALL,10);
    mainSizer->Add(hSizer,0,wxALL,10);
    wxButton *okb = new wxButton(this,wxID_OK, wxT("OK"));
    mainSizer->Add(okb,0,wxALIGN_CENTER | wxALL, 10);
    SetSizer(mainSizer);
    mainSizer->SetSizeHints(this);
}
