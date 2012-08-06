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


/***************************************************************************
 *            treeview.cc
 *
 *  Sun Oct 23 18:59:25 2005
 *  Copyright  2005  Michal Palczewski
 *  michalp@gmail.com
 ****************************************************************************/
#include "Treeview.h"
#include <wx/colour.h>
#include <wx/dcclient.h>
#include <wx/filedlg.h>
#include <wx/ffile.h>
#include <wx/dcps.h>
#include <wx/cmndata.h>
#include "wxetdc.h"

const int border = 25;

Treeview::Treeview(TreeFileReader* tfr, wxWindow * parent) : 
	wxWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
			wxHSCROLL | wxVSCROLL | wxNO_FULL_REPAINT_ON_RESIZE),tfr(tfr)
{
	tree = NULL;
	drawing = NULL;
	SetTree(0);
	SetBackgroundColour(*wxWHITE);
}


void Treeview::SetTree(int n) 
{
	if ( tree != NULL )
		free_tree(tree);
	if ( drawing != NULL )
		free_drawing(drawing);
	if ( n >= GetNumTrees() ) 
		n = GetNumTrees() - 1;
	curTree = n;
	tree = get_tree_from_newick(tfr->getNewick(n).c_str());
    tfr->TranslateTree(tree,n); 

	if ( tree != NULL && tree->is_valid )
		drawing = get_drawing_from_tree(tree);
	else
		drawing = NULL;	
	Refresh(true);
}


void Treeview::DrawTree(etDC& dc)
{
	dc.SetBounds(-1,0,0,1);
	double maxx = -1;
	
	if ( tree->is_valid == 0 ) {
		dc.DrawText(_T("Tree is not valid:") + 
				wxString::FromAscii(tree->why_not),-1,0);
		return;
	}

	for ( int i = 0 ; i < tree->n_tips ; i++ ) {
		double width,height;
		dc.GetTextExtent(wxString::FromAscii(tree->tips[i]->name),&width,&height);
		double curx = tree->tips[i]->coordinates->x + width;
		if ( curx > maxx ) maxx = curx;
	}
	maxx = maxx / (1 - maxx); /* when we resize the thing, maxx also needs to scale */
	dc.SetBounds(-1,0,maxx,1);
	
	for ( int i = 0 ; i < tree->n_tips ; i++ ) {
		double width,height;
		dc.GetTextExtent(wxString::FromAscii(tree->tips[i]->name), &width, &height);
		dc.DrawText(wxString::FromAscii(tree->tips[i]->name),tree->tips[i]->coordinates->x, tree->tips[i]->coordinates->y - height /2);
	}
	
	for ( Primitive* cur = drawing; cur != NULL ; cur = cur->next ) {
		switch (cur->type) {
            case POLYGON:
                {
                    tlPolygon* p = (tlPolygon*)cur->data;
                    double *x,*y;
                    x = new double[p->num_points];
                    y = new double[p->num_points];
                    for ( int i = 0 ; i < p->num_points ; i++ ) {
                        x[i] = p->points[i].x;
                        y[i] = p->points[i].y;
                    }
                    dc.FillPolygon(p->num_points, x, y, p->color);
                    delete[] x;
                    delete[] y;
                    break;
                }
			case LINE: 
				{ 
					Line* l = (Line*)cur->data;	
					dc.DrawLine(l->point1.x,l->point1.y,l->point2.x,
                            l->point2.y);			
				}
				break;
			case ARROW:
				{
					Arrow* a = (Arrow*)cur->data;
					dc.DrawArrow(a->from.x,a->from.y,a->to.x,a->to.y);
				}
				break;
			case TEXT:
			{
				Text* t = (Text*)cur->data;
				double width, height;
				dc.GetTextExtent(wxString::FromAscii(t->text),&width,&height);
				width = t->halign  *width / 2;
				height = (2 - t->valign) * height / 2;
				
				dc.DrawText(wxString::FromAscii(t->text),t->where.x - width,t->where.y - height);
			}
			break;
			default:
				break;
		}
	}	
}


void Treeview::OnPaint(wxPaintEvent& event) {
	wxPaintDC dc(this);
    dc.Clear();
	wxetdc etdc(&dc);
	etdc.SetDeviceMargins(border,border,border,border);
	DrawTree(etdc);
}


void Treeview::OnSize(wxSizeEvent& event)
{
    Refresh();
}

Treeview::~Treeview() 
{
	delete tfr;
}

BEGIN_EVENT_TABLE(Treeview, wxWindow)
	EVT_PAINT (Treeview::OnPaint)
	EVT_SIZE (Treeview::OnSize)
END_EVENT_TABLE()
