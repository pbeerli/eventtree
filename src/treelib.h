/*    Event Tree, a program to draw events on evolutionary trees
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
*/

/***************************************************************************
 *            treelib.h
 *
 *  Fri Oct 21 13:12:17 2005
 *  Copyright  2005  Michal Palczewski
 *  Email michalp@gmail.com
 ****************************************************************************/
#ifndef TREELIB_H
#define TREELIB_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Node _Node;
typedef struct Branch _Branch;
typedef struct Event _Event;
typedef struct Tree _Tree;
typedef struct Primitive _Primitive;
typedef struct Point _Point;

typedef enum {LINE, ARROW, TEXT, POLYGON} Primitive_type;
typedef enum {FROM,CENTER, TO} Alignment_type;
typedef enum {INTERVAL, MIGRATION, LABEL} Event_type;

typedef struct Node {
	_Branch **branches; /* Array of Branches, in the rooted case the first one 
						* points to the root */
	char *name; 
	_Event* events; /* Array of Events happening right at the Node */
	int tip;
	_Point* coordinates;
} Node;


typedef struct Branch {
	Node *(connections[2]); /* Array of the two nodes that the branch connects, 
						  * in the rooted case the first one points towards 
						  * the root */
	double length;
	_Event* events;      /* List of events that happen on the branch */
} Branch;


typedef struct Event {
	Event_type type;
	void* data;
	_Event* next;
} Event;


typedef struct Migration_Event {
	char* from;
	char* to;
	double v;
} Migration_Event;


typedef struct Interval_Event {
	double from;
	double to;
} Interval_Event;


typedef struct Label_Event {
	char* label;
} Label_Event;


typedef struct Tree {
	Node* root;
	int is_valid;
	char* why_not;
	Node** tips;
	int n_tips;
} Tree;

typedef struct Point {
	double x,y;
} Point;

typedef struct Primitive {
	Primitive_type type;
	void* data;
	_Primitive* next;
} Primitive;

typedef struct Line {
	Point point1;
	Point point2;
} Line;

typedef struct Arrow {
	Point from;
	Point to;
} Arrow;


typedef struct tlPolygon {
    Point* points;
    int num_points;
    int color;
} tlPolygon;


typedef struct Text {
	Point where;
	char* text;
	Alignment_type valign;
	Alignment_type halign;
} Text;

Tree* get_tree_from_newick(const char*);
Primitive* get_drawing_from_tree(Tree*);
Primitive* get_drawing_from_newick(const char*);

void free_drawing(Primitive*);
void free_tree(Tree*);

#ifdef __cplusplus
}
#endif
#endif
