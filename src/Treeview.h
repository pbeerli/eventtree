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


#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <wx/scrolwin.h>
#include <wx/event.h>
#include <wx/dc.h>
#include "treelib.h"
#include "TreeFileReader.h"
#include "etDC.h"

// in version 2 this is a scrolled window supporting zoom
class Treeview : public wxWindow {
public:
	Treeview(TreeFileReader* t,wxWindow* parent);
	virtual void OnPaint(wxPaintEvent& event);
	virtual void OnSize(wxSizeEvent& event);
	virtual void DrawTree(etDC& dc);
	virtual ~Treeview();
	virtual int GetCurTree() { return curTree; }
	virtual int GetNumTrees(){ return tfr->GetNumTrees();}
	virtual void SetTree(int n);
	virtual void SetNext() { SetTree(HasNext()?curTree+1:curTree);}
	virtual void SetPrev() { SetTree(HasPrev()?curTree-1:curTree);}
	virtual bool HasNext() { return !(curTree == GetNumTrees());}
	virtual bool HasPrev() { return !(curTree==0); }
	
private:
	Primitive* drawing;
	Tree* tree;
	TreeFileReader* tfr;
	int curTree;

	DECLARE_EVENT_TABLE();
};

#endif //TREEVIEW_H
