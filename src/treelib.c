/***************************************************************************
 *            treelib.c
 *
 *  Fri Oct 21 13:11:18 2005
 *  Copyright  2005  Michal Palczewski
 *  Email michalp@gmail.com
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "treelib.h"

#define SPECIAL_CHARS "();:,[]&"
#define WHITESPACE_CHARS "\r\n\t "
#define NUMBER_CHARS "-0123456789."
#define SPECIAL_NUMBERCHAR "e"

#define TRUE 1
#define FALSE 0

typedef enum { CHAR, STRING, NUMBER } Token_type;

typedef struct Token _Token;

typedef struct Token {
	Token_type type;
	char *data;
	double fdata;
	_Token *next;
	char cdata;
} Token;


static Node *get_new_node()
{
	Node *thenode = (Node *) malloc(sizeof(Node));
	thenode->branches = NULL;
	thenode->name = NULL;
	thenode->events = 0;
	thenode->tip = 0;
	thenode->coordinates = NULL;
	return thenode;
}


static Token *get_new_token()
{
	Token *thetoken;
	thetoken = (Token *) malloc(sizeof(Token));
	thetoken->data = NULL;
	thetoken->fdata = 0;
	thetoken->next = NULL;
	return thetoken;
}


static Token *parse_next_token(const char *newick, int *location,
			       int strlength, int length)
{
	Token *thetoken = get_new_token();
	char *end, *str;
	int i, strlen;

	if (*location >= strlength)
		return NULL;
	while (strchr(WHITESPACE_CHARS, newick[*location])
	       && *location < strlength)
		(*location)++;
	if (*location >= strlength)
		return NULL;

	if (strchr(SPECIAL_CHARS, newick[*location])) {
		thetoken->cdata = newick[*location];
		thetoken->type = CHAR;
		(*location)++;
		return thetoken;
	}

	if (strchr(NUMBER_CHARS, newick[*location]) && length) {
		thetoken->fdata = strtod(newick + *location, &end);
		*location = end - newick;
		thetoken->type = NUMBER;
		return thetoken;
	}

	for (i = *location; i < strlength; i++) {
		if (strchr(SPECIAL_CHARS, newick[i]))
			break;
		if (strchr(WHITESPACE_CHARS, newick[i]))
			break;
	}

	strlen = i - *location;
	str = (char *) malloc(strlen * sizeof(char) + 1);
	for (i = 0; i < strlen; i++, (*location)++)
		str[i] = newick[*location];
	str[i] = 0;
	thetoken->data = str;
	thetoken->type = STRING;
	return thetoken;
}


static Token *tokenize(const char *newick)
{
	int i = 0, j = 0;
	int length = strlen(newick);
	Token *cur_token = NULL;
	Token *first_token = NULL;
	first_token = parse_next_token(newick, &i, length, 0);
	cur_token = first_token;
	while (cur_token != NULL) {
		cur_token->next = parse_next_token(newick, &i, length,
			cur_token->type == CHAR && (cur_token->cdata == ':' )); 
		cur_token = cur_token->next;
		j++;
	}
	return first_token;
}


static void free_token(Token * token)
{
	switch (token->type) {
	case STRING:
		free(token->data);
		free(token);
		break;
	case CHAR:
	case NUMBER:
	default:
		free(token);
		break;
	}
}


static void free_tokens(Token * tokens)
{
	Token *cur_token = tokens;
	while (cur_token != NULL) {
		tokens = cur_token->next;
		free_token(cur_token);
		cur_token = tokens;
	}
}


static int count_branches(Node * n)
{
	Branch **current = n->branches;
	int i = 0;
	while (current != NULL && *current != NULL) {
		current++;
		i++;
	}
	return i;
}


static void add_branch(Node * n, Branch * branch)
{
	int numbranches = count_branches(n);
	n->branches =
	    (Branch **) realloc(n->branches,
				sizeof(Branch *) * (numbranches + 2));
	n->branches[numbranches + 1] = NULL;
	n->branches[numbranches] = branch;
}


static Branch *get_new_branch()
{
	Branch *b = (Branch *) malloc(sizeof(Branch));
	b->connections[0] = NULL;
	b->connections[1] = NULL;
	b->length = -1;
	b->events = NULL;
	return b;
}


/* funny X11 pollutes the namespase so bad that connect makes the program
    crash */
static void tree_connect(Node * from, Node * to)
{
	Branch *newbranch = get_new_branch();
	add_branch(from, newbranch);
	add_branch(to, newbranch);
	newbranch->connections[0] = from;
	newbranch->connections[1] = to;
}


static Token *finish_comment(Token * tokens)
{
	while (tokens != NULL
	       && !(tokens->type == CHAR && tokens->cdata == ']'))
		tokens = tokens->next;
	return tokens->next;
}

static Event *get_new_interval_event(void)
{
    Interval_Event *ievt;
    Event* evt = (Event *)malloc(sizeof(Event));
    evt->type = INTERVAL;
    evt->next = NULL;
    evt->data = malloc(sizeof(Interval_Event));
    ievt = (Interval_Event*) evt->data;
    ievt->from = 0;
    ievt->to = 0;
    return evt;
}

static Event *get_new_migration_event(void)
{
	Migration_Event *mevt;
	Event *evt = (Event *) malloc(sizeof(Event));
	evt->type = MIGRATION;
	evt->next = NULL;
	evt->data = malloc(sizeof(Migration_Event));
	mevt = (Migration_Event *) evt->data;
	mevt->from = NULL;
	mevt->to = NULL;
	mevt->v = -1;
	return evt;
}

static Event *get_new_label_event()
{
	Label_Event *levt;
	Event *evt = (Event *) malloc(sizeof(Event));
	evt->type = LABEL;
	evt->next = NULL;
	evt->data = malloc(sizeof(Label_Event));
	levt = (Label_Event *)evt->data;
	levt->label = NULL;
	return evt;
}

static void free_interval_event(Event* evt)
{
    free(evt->data);
    free(evt);
}

static void free_migration_event(Event * evt)
{
    Migration_Event *mevt = (Migration_Event*)evt;
    if ( mevt->from != NULL )
        free(mevt->from);
    if ( mevt->to != NULL) 
        free(mevt->to);
	free(evt->data);
	free(evt);
}


static void append_branch_event(Branch * n, Event * evt)
{
	Event *cur_event;
	if (n->events == NULL) {
		n->events = evt;
		return;
	}

	for (cur_event = n->events; cur_event->next != NULL;
	     cur_event = cur_event->next);

	cur_event->next = evt;
}


static Token *parse_migration_event(Token * tokens, Node * cur_node)
{
	Migration_Event *mevt;
	Event *evt = get_new_migration_event();
	mevt = (Migration_Event *) evt->data;
	tokens = tokens->next;
	evt->type = MIGRATION;

	if (tokens == NULL || tokens->type != STRING) {
		free_migration_event(evt);
		return finish_comment(tokens);
	}
	mevt->from = strdup(tokens->data);
	tokens = tokens->next;
	if (tokens == NULL || tokens->type != STRING) {
		free_migration_event(evt);
		return finish_comment(tokens);
	}
	mevt->to = strdup(tokens->data);
	tokens = tokens->next;
	if (tokens == NULL) {
		free_migration_event(evt);
		return finish_comment(tokens);
	}
	tokens = tokens->next;
	if (tokens == NULL || tokens->type != NUMBER) {
		free_migration_event(evt);
		return finish_comment(tokens);
	}
	mevt->v = tokens->fdata;
	append_branch_event(cur_node->branches[0], evt);
	return finish_comment(tokens);
}


static Token *parse_interval_event(Token * tokens, Node * cur_node)
{
	Interval_Event *ievt;
	Event *evt = get_new_interval_event();
	ievt = (Interval_Event *) evt->data;
	tokens = tokens->next;
	evt->type = INTERVAL;
	
    if (tokens == NULL || tokens->type != STRING) {
		free_interval_event(evt);
		return finish_comment(tokens);
	}
	ievt->from = atof(tokens->data);
	tokens = tokens->next;
	if (tokens == NULL || tokens->type != STRING) {
		free_interval_event(evt);
		return finish_comment(tokens);
	}
	ievt->to = atof(tokens->data);
	
    append_branch_event(cur_node->branches[0], evt);
	return finish_comment(tokens);
}


static Token *parse_comment(Token * tokens, Node * cur_node)
{				/* we expect the first token to be a '[' */
	tokens = tokens->next;
	if (tokens == NULL
	    || !(tokens->type == CHAR && tokens->cdata == '&'))
		return finish_comment(tokens);
	tokens = tokens->next;
	if (tokens->type != STRING)
		return finish_comment(tokens);
	if (strcmp(tokens->data, "M") == 0)
		return parse_migration_event(tokens, cur_node);
	if (strcmp(tokens->data, "I") == 0)
		return parse_interval_event(tokens, cur_node);

	return finish_comment(tokens);
}


static int get_ntips_in_sub(Node * n, Branch * b)
{
	int i;
	Branch *cur;
	Node *child;
	int curdepth = 0, has_sub = 0;


	for (i = 0; n->branches && n->branches[i] != NULL; i++) {
		cur = n->branches[i];
		if (cur == b)
			continue;
		has_sub = 1;
		if (cur->connections[0] == n)
			child = cur->connections[1];
		else
			child = cur->connections[0];
		curdepth += get_ntips_in_sub(child, cur);
	}
	if (has_sub == 0)
		return 1;
	return curdepth;
}


/** fills in the tree's tips array */
static void do_tipsR(Tree * tree, Node * n, Branch * anc, int *i)
{
	int j, has_sub = 0;
	Node *child;

	for (j = 0; n->branches && n->branches[j] != NULL; j++) {
		if (n->branches[j] == anc)
			continue;
		if (n->branches[j]->connections[0] == n)
			child = n->branches[j]->connections[1];
		else
			child = n->branches[j]->connections[0];
		do_tipsR(tree, child, n->branches[j], i);
		has_sub = 1;
	}

	if (has_sub == 0) {
		tree->tips[*i] = n;
		(*i)++;
	}
}

static void do_tips(Tree * tree)
{
	int i = 0;

	tree->n_tips = get_ntips_in_sub(tree->root, NULL);

	tree->tips = (Node **) malloc(tree->n_tips * sizeof(Node *));
	do_tipsR(tree, tree->root, NULL, &i);
}


static Tree *parse_tree(Token * tokens)
{
	Token *cur_token = tokens, *old_token;
	Node *cur_node = NULL;
	Node *prev_node = NULL;
	Branch *cur_branch = NULL;
	int length,label = 0;
	Tree *t = (Tree *) malloc(sizeof(Tree));
	t->tips = 0;
	t->is_valid = 1;

	if (cur_token == 0) {
		t->is_valid = 0;
		t->why_not = "No tree found";
		return t;
	}

	while (cur_token->type == CHAR && cur_token->cdata == '[')
		cur_token = parse_comment(cur_token, NULL);
	if (cur_token->type != CHAR && cur_token->cdata != '(') {
		t->is_valid = 0;
		t->why_not = "A tree is expected to start with (";
		return t;
	}
	cur_node = t->root = get_new_node();

	while (cur_token != NULL) {
		if ( cur_token->type != NUMBER ) length = FALSE;
		switch (cur_token->type) {
		case CHAR:
			switch (cur_token->cdata) {
			case '(':
				prev_node = cur_node;
				cur_node = get_new_node();
				tree_connect(prev_node, cur_node);
				cur_token = cur_token->next;
                label = FALSE;
				break;
			case ')':
				cur_branch = cur_node->branches[0];
				cur_node =
				    cur_node->branches[0]->connections[0];
				cur_token = cur_token->next;
                label = TRUE;
				break;
			case ';':
				if (cur_node == t->root) 
					return t;
				t->is_valid = 0;
				t->why_not = "Unmatched parenthesis";
				return t;

			case ',':
				prev_node =
				    cur_node->branches[0]->connections[0];
				cur_node = get_new_node();
				tree_connect(prev_node, cur_node);
				cur_token = cur_token->next;
                label = FALSE;
				break;
			case '[':
				old_token = cur_token;
				cur_token =
				    parse_comment(cur_token, cur_node);

				break;
			case ']':
				t->is_valid = 0;
				t->why_not = "Spurious ]";
				return t;
				break;
			case '&':
				t->is_valid = 0;
				t->why_not = "Spurious &";
				return t;
			case ':':
				length = TRUE;
				cur_token = cur_token->next;
                label = FALSE;
				break;
			}
			break;
		case STRING:
			if (cur_node == NULL) {
				t->is_valid = 0;
				t->why_not = "Tree must start with '('";
				return t;
			}
            if ( label ) {
                Event *evt = get_new_label_event();
                Label_Event *levt = (Label_Event*)evt->data;
                levt->label = strdup(cur_token->data);
                append_branch_event(cur_node->branches[0],evt);
                cur_token = cur_token->next;
            }
            else {
                cur_node->name = strdup(cur_token->data);
                cur_token = cur_token->next;
            }
			break;
		case NUMBER:
			if (length) {
                if ( cur_node == t->root ) {
                    cur_node = get_new_node();
                    tree_connect(cur_node,t->root);
                    t->root=cur_node; 
                }
				cur_node->branches[0]->length =
				    cur_token->fdata;
				length = 0;
			}
			cur_token = cur_token->next;
			break;
		}
	}
	if (cur_node == t->root) 
		return t;
	t->is_valid = 0;
	t->why_not = "Unmatched ( and tree didn't end with ;";

	return t;
}


Tree *get_tree_from_newick(const char *newick)
{
	Tree *thetree;
	Token *tokens;
	tokens = tokenize(newick);
	thetree = parse_tree(tokens);
	free_tokens(tokens);
	do_tips(thetree);
	return thetree;
}

/** returns the distance till the furthest tip */
static double get_max_depth(Node * n, Branch * b)
{
	Branch *cur;
	double maxlength = 0;
	double curlength = 0;
	Node *child;
    Event* evt;
    Interval_Event* ievt;
	int i;

	if (b && b->length == -1)
		b->length = 1;	// handle trees with a missing length gracefully
	for (i = 0; n->branches && n->branches[i] != NULL; i++) {
		cur = n->branches[i];
		if (cur == b)
			continue;
		if (cur->connections[0] == n)
			child = cur->connections[1];
		else
			child = cur->connections[0];
		curlength = get_max_depth(child, cur);
		if (curlength > maxlength)
			maxlength = curlength;
	}
	if (b) {
		maxlength += b->length;
        evt = b->events;
        while ( evt ) {
            if ( evt->type == INTERVAL )  {
                ievt = (Interval_Event*)evt->data;
                maxlength += ievt->to - b->length;
            }
            evt = evt->next;
        }
    }
	return maxlength;
}



static int count_sub_trees(Node * n, Branch * b)
{
	int i;
	int j = 0;
	for (i = 0; n->branches && n->branches[i] != NULL; i++) {
		if (n->branches[i] == b)
			continue;
		else
			j++;
	}
	return j;

}


static Primitive *create_line_primitive(double x1, double y1, double x2,
					double y2)
{
	Primitive *p = (Primitive *) malloc(sizeof(Primitive));
	Line *l = (Line *) malloc(sizeof(Line));

	l->point1.x = x1;
	l->point1.y = y1;
	l->point2.x = x2;
	l->point2.y = y2;
	p->data = l;
	p->type = LINE;
	p->next = NULL;

	return p;
}

static Primitive *append(Primitive * p, Primitive * q)
{
	Primitive *r;
	if (q == NULL)
		return p;
	if (p == NULL)
		return q;
	for (r = p; r->next != NULL; r = r->next);
	r->next = q;
	return p;
}


static int is_tip(Node *n) 
{
    if ( n->branches[1] == NULL )
        return TRUE;
    else 
        return FALSE;
}


static void calculate_node_coordinates(Node * n, Branch * anc, double treeleft,
			   double top, double bottom, double width)
{
	int i, j, subs, tip_interval;
	double *height = NULL, totalheight = 0, totaldrawheight = top - bottom, 
           curtop, curbottom, child_bottom = 0, child_top = 0;
	Node *child;
    Interval_Event* ievt;
    Event* evt;
	
    subs = count_sub_trees(n, anc);
	n->coordinates = (Point *) malloc(sizeof(Point));
    if ( anc != NULL && is_tip(n) ) {
        for ( evt = anc->events ; evt != NULL && evt->type != INTERVAL 
                ; evt = evt->next);
        if ( evt != NULL && evt->type == INTERVAL ) {
            ievt = (Interval_Event*)evt->data;
            n->coordinates->x = treeleft + 
                (((Interval_Event*)evt->data)->to - anc->length)/width;
            if ( n->coordinates->x < treeleft ) 
                n->coordinates->x = treeleft;
        } else
            n->coordinates->x = treeleft;
    } else n->coordinates->x = treeleft;

	if (subs != 0)
		height = (double *) malloc(subs * sizeof(double));
    
	j = 0;
	for (i = 0; n->branches && n->branches[i] != NULL; i++) {
		if (n->branches[i] == anc)
			continue;
		if (n->branches[i]->connections[0] == n)
			child = n->branches[i]->connections[1];
		else
			child = n->branches[i]->connections[0];
		height[j] = get_ntips_in_sub(child, n->branches[i]);
		j++;
	}

	totalheight = get_ntips_in_sub(n, anc);

	j = 0;
	curbottom = top;
	for (i = 0; n->branches && n->branches[i] != NULL; i++) {
		if (n->branches[i] == anc)
			continue;
		if (n->branches[i]->connections[0] == n)
			child = n->branches[i]->connections[1];
		else
			child = n->branches[i]->connections[0];
		curtop = curbottom;
		curbottom -= height[j] / totalheight * totaldrawheight;
		calculate_node_coordinates(child, n->branches[i], treeleft + 
                n->branches[i]->length / width, curtop, curbottom, width);
		if (j == 0)
			child_bottom = child->coordinates->y;
		else if (j == subs - 1)
			child_top = child->coordinates->y;
		j++;
	}
	if (subs != 0 && subs != 1)
		n->coordinates->y = (child_bottom + child_top) / 2;
	else
		n->coordinates->y = (top + bottom) / 2;
	return;
}


static Primitive *create_arrow_primitive(double x1, double y1, double x2,
					 double y2)
{
	Primitive *p = (Primitive *) malloc(sizeof(Primitive));
	Arrow *l = (Arrow *) malloc(sizeof(Arrow));

	l->from.x = x1;
	l->from.y = y1;
	l->to.x = x2;
	l->to.y = y2;
	p->data = l;
	p->type = ARROW;
	p->next = NULL;
	return p;
}


static Primitive *create_text_primitive(double x, double y, char *text,
					Alignment_type vert,
					Alignment_type horiz)
{
	Primitive *p = (Primitive *) malloc(sizeof(Primitive));
	Text *t = (Text *) malloc(sizeof(Text));

	t->where.x = x;
	t->where.y = y;
	t->valign = vert;
	t->halign = horiz;
	t->text = strdup(text);

	p->data = t;
	p->type = TEXT;
	p->next = NULL;
	return p;
}


static Primitive *get_rectangle(double x1, double y1, double x2, double y2)
{
    Point *pts;
    tlPolygon* rec;
    Primitive* p;

    pts = (Point*)malloc(sizeof(Point)*4);
    
    pts[0].x = x1;
    pts[0].y = y1;
    pts[1].x = x2;
    pts[1].y = y1;
    pts[2].x = x2;
    pts[2].y = y2;
    pts[3].x = x1;
    pts[3].y = y2;

    p = (Primitive*) malloc(sizeof(Primitive));
    rec = (tlPolygon*) malloc(sizeof(tlPolygon));
    p->type = POLYGON;
    p->data = rec;
    rec->color = 0x7f7f7f;
    rec->num_points = 4;
    rec->points = pts;
    p->next = NULL;
    return p;
}


static Primitive *draw_branch_events(Tree * t, Node * n, Branch * anc,
				     Event * evt, double width, double x)
{
	Primitive *p = NULL, *q = NULL;
	Migration_Event *mevt;
	Label_Event* levt;
    Interval_Event* ievt;
	double height;
	char* pop1, *pop2;
	Alignment_type a1, a2;
	double sx;
    double x1,x2,y1,y2;
    double length, scale;
	if (evt == NULL)
		return NULL;

	switch (evt->type) {
	case MIGRATION:
		mevt = (Migration_Event *) evt->data;
		x = x - mevt->v / width;
		height = 1.0 / 4 / t->n_tips;
		pop1 = mevt->from;
		pop2 = mevt->to;
		if ( strcmp(pop2,pop1) > 0 ) {
			height = -height;
			a1 = FROM;
			a2 = TO;
		} else {
			a1 = TO;
			a2 = FROM;
		}
		q = create_arrow_primitive(x, n->coordinates->y + height,
					   x, n->coordinates->y - height);
		p = append(p, q);
		q = create_text_primitive(x, n->coordinates->y + height,
					  pop1, a1, CENTER);
		p = append(p, q);
		q = create_text_primitive(x, n->coordinates->y - height,
					  pop2, a2, CENTER);
		p = append(p, q);
		break;
	case LABEL:
		levt = (Label_Event *) evt->data;
		sx = (anc->connections[0]->coordinates->x + 
				anc->connections[1]->coordinates->x )/2;
		height = 1.0 / 4 / t->n_tips;
		q = create_text_primitive(sx,n->coordinates->y,
				levt->label, FROM, CENTER);
		p = append(p, q);
		break;
    case INTERVAL: 
		height = 1.0 / 12 / t->n_tips;
        ievt = (Interval_Event *) evt->data;
        length = fabs(anc->connections[0]->coordinates->x - 
            anc->connections[1]->coordinates->x);
        if ( anc && is_tip(n) && ievt->to > anc->length ) 
            scale = length / ievt->to;
        else 
            scale = length / anc->length;  
        x1 = n->coordinates->x + (ievt->from - anc->length)*scale;
        x2 = n->coordinates->x + (ievt->to - anc->length)*scale;
        if ( anc && is_tip(n) ) {
            if ( anc->length > ievt->to )
                x1 = n->coordinates->x - (anc->length - ievt->from)*scale;
            else 
                x1 = n->coordinates->x - (ievt->to - ievt->from)*scale;
            if ( anc->length > ievt->to )
                x2 = n->coordinates->x - (anc->length - ievt->to)*scale;
            else
                x2 = n->coordinates->x;
        }
        y1 = n->coordinates->y - height;
        y2 = n->coordinates->y + height;
        q = get_rectangle(x1,y1,x2,y2);
        p = append(p, q);
        break;
	default:
		break;
	}
	q = draw_branch_events(t, n, anc, evt->next, width, x);
	p = append(p, q);
	return p;
}


static Primitive *draw_sub_tree(Tree * t, Node * n, Branch * anc,
				double width)
{
	int subs = count_sub_trees(n, anc);
	int i, j = 0, did_it = FALSE;
	Node *child;
	Primitive *p = NULL, *q = NULL;
	Node *ancestor;
	double line_top = 0;
	double line_bottom = 0;
    double scale, difference,bigger; 
    Event* evt;
	
    if ( anc != NULL  ) {
		p = draw_branch_events(t, n, anc, anc->events, width,
				       n->coordinates->x);
		if (anc->connections[0] == n)
			ancestor = anc->connections[1];
		else
			ancestor = anc->connections[0];
    }
    
    if ( anc != NULL && is_tip(n) && anc->events ) {
        for ( evt = anc->events ; evt != NULL && evt->type != INTERVAL 
                ; evt = evt->next);
        if ( evt != NULL && evt->type == INTERVAL ) {
            bigger = ((Interval_Event*)evt->data)->to;
            if ( bigger < anc->length ) 
                bigger = anc->length; 
            scale = (n->coordinates->x - ancestor->coordinates->x) / bigger;
            difference = ((Interval_Event*)evt->data)->to - anc->length;
            q = create_line_primitive(ancestor->coordinates->x, 
                        n->coordinates->y,
                        ancestor->coordinates->x + (anc->length)*scale,
                        n->coordinates->y);
            p = append(p, q);
            did_it = TRUE;
        }
    }
    if (anc != NULL && did_it == FALSE) {
		q = create_line_primitive(ancestor->coordinates->x,
					  n->coordinates->y,
					  n->coordinates->x,
					  n->coordinates->y);
		p = append(p, q);
	}
	j = 0;
	for (i = 0; n->branches && n->branches[i] != NULL; i++) {
		if (n->branches[i] == anc)
			continue;
		if (n->branches[i]->connections[0] == n)
			child = n->branches[i]->connections[1];
		else
			child = n->branches[i]->connections[0];
		if (j == 0)
			line_top = child->coordinates->y;
		if (j == subs - 1)
			line_bottom = child->coordinates->y;
		q = draw_sub_tree(t, child, n->branches[i], width);
		p = append(p, q);
		j++;
	}
	if (subs != 0) {
		q = create_line_primitive(n->coordinates->x, line_top,
					  n->coordinates->x, line_bottom);
		p = append(p, q);
	}
	return p;
}

/* returns a "nice" width for the scale bar depending on the width of the 
 * tree
 * in an effort to allow easy tweaking this is written more complicated than
 * it needs to be.  
 * Generate candidates and then evaluate them based on a criteria
 * not efficient candidates are produced that have no chance, 
 * no reason to make it faster. 
 * */
#define GSW_VARS  { 1, 5 }
#define GSW_NVARS 2
#define GSW_PASSES 2
#define GSW_GOAL  .25
double get_scale_width(double width)
{
    double sw_vars[] = GSW_VARS;
    int i,j;

    int num_newvars = (GSW_PASSES * 2 + 1)*GSW_NVARS;
    double* new_swvars = malloc(num_newvars * sizeof(double));
        /* candidate variables */
    double* scores = malloc(num_newvars * sizeof(double));
        /* scores of how good the candidates are = difference from goal*/
    double digs = log10(width);
    
    int besti;
    double bestscore = 10000; /* as shitty number as any, not scale invariant,
                               but only bad if trees lengths are bigger */
    double sw;
    
    for ( i = -GSW_PASSES ; i <= GSW_PASSES ; i++ ) {
        int ai = i + GSW_PASSES; 
        for ( j = 0 ; j < GSW_NVARS ; j++ ) {
            new_swvars[ai*GSW_NVARS+j] = sw_vars[j] * pow(10,((int)digs +i));
        }
    }
    
    for ( i = 0 ; i < num_newvars ; i++ ) {
      scores[i] = fabs(exp(new_swvars[i]/width/GSW_GOAL) - exp(1));
        /* read the code carefully or read on 
         * new_swvars[i]/width/GSW_GOAL is how off we are values bigger than 1 
         * are overshoots the exponent makes overshoots more costly
         * subtract exp(1) so that hitting our goal makes our score 0, which 
         * is the best possible score */
    }

    for ( i = 0 ; i < num_newvars ; i++ ) {
        if ( scores[i] < bestscore )  {
            besti = i;
            bestscore = scores[i];
        }
    }
    
    sw =  new_swvars[besti];
    free(new_swvars);
    free(scores);
    return sw;
}


int scale_on_top(Tree* tree) 
{
    double highest = 0;
    double lowest = 1;
    int i;
    Node* child;
	for (i = 0; tree->root->branches && tree->root->branches[i] != NULL; i++) {
        if ( tree->root->branches[i]->connections[0] == tree->root )
            child = tree->root->branches[i]->connections[1];
        else
            child = tree->root->branches[i]->connections[0];
        if ( child->coordinates->y < lowest )
            lowest = child->coordinates->y;
        if ( child->coordinates->y > highest )
            highest = child->coordinates->y;
    }

    highest = 1-highest;
    
    if ( highest < lowest )
        return TRUE;
    else return FALSE;
}



/* assumes node coordinates are already calculated */
Primitive* draw_scale(Tree* tree, double width)
{
    double sw = get_scale_width(width);
    char buf[100];
    double percent = sw/width;
    Primitive *p,*q;
    if ( scale_on_top(tree) ) {
        p = create_line_primitive(-1,0,percent-1.0,0);
        q = create_line_primitive(percent-1.0,0,percent-1.0,0.01);
        p = append(p,q);
        sprintf(buf, "%g",sw);
        q = create_text_primitive(percent-1.0,0.01,buf,TO,CENTER);
        p = append(p,q);
    } else {
        p = create_line_primitive(-1,1,percent-1.0,1);
        q = create_line_primitive(percent-1.0,1,percent-1.0,0.99);
        p = append(p,q);
        sprintf(buf, "%g",sw);
        q = create_text_primitive(percent-1.0,0.99,buf,FROM,CENTER);
        p = append(p,q);
    }
    return p;
}


Primitive *get_drawing_from_tree(Tree * tree)
{
    Primitive *drawing;
	double width = get_max_depth(tree->root, NULL);
	calculate_node_coordinates(tree->root, NULL, -1, 1, 0, width);
	drawing = draw_sub_tree(tree, tree->root, NULL, width);
    append(drawing, draw_scale(tree, width));
    return drawing;
}

void free_event(Event * evt)
{
	if (evt->next)
		free_event(evt->next);
	if (evt->data)
		free(evt->data);
	free(evt);
}

void free_branch(Branch * b)
{
	if (b->events != NULL)
		free_event(b->events);
	free(b);
}

void free_tree_recurs(Node * n, Branch * anc)
{
	int i;
	Node *child;
	for (i = 0; n->branches && n->branches[i] != NULL; i++) {
		if (n->branches[i] == anc)
			continue;
		if (n->branches[i]->connections[0] == n)
			child = n->branches[i]->connections[1];
		else
			child = n->branches[i]->connections[0];
		free_tree_recurs(child, n->branches[i]);
		free(n->branches[i]);
	}
	if (n->name)
		free(n->name);
	free(n);
}

void free_tree(Tree * t)
{
	free_tree_recurs(t->root, NULL);
	if (t->tips)
		free(t->tips);
	free(t);
}

void free_drawing(Primitive * d)
{
	Primitive *old;
	Text *t;
	while (d != NULL) {
		switch (d->type) {
		case LINE:
		case ARROW:
			free(d->data);
			break;
		case TEXT:
			t = (Text *) d->data;
			free(t->text);
			free(d->data);
			break;
		}
		old = d;
		d = d->next;
		free(d);
	}
}
