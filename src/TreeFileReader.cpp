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


#include "ncl/ncl.h"
#include "TreeFileReader.h"
#define block 1024 // kilobyte
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>

using namespace std;

const string specialChars = "();[]";

TreeFileReader::TreeFileReader(std::string filename):
	treefile(filename.c_str(),ios::binary) 
{   // I tried this less convoluted but gave up in the interest of just writing it

	char* nexus = "#NEXUS";
	unsigned length = string(nexus).length();
	char* buf = new char[length+1];

	treefile.read(buf,length);
	buf[length] = 0;
	for ( unsigned i = 1 ; i < length ; i++ )
		buf[i] = toupper(buf[i]);

	string cmp(buf);
	if ( string(buf) == string(nexus) )  { 
		type = NEXUS;
		ReadNexus();
		
	} else {
		type = PHYLIP;
		ReadPhylip();

	}
	treefile.close();
	delete[] buf;
}


void TreeFileReader::ReadNexus()
{
	treefile.seekg(0);
	TaxaBlock taxa;
	TreesBlock treesBlock(&taxa);

	NexusToken token(treefile);
	Nexus nexus;
	nexus.Add(&taxa);
	nexus.Add(&treesBlock);

	nexus.Execute(token);

	for ( unsigned i = 0 ; i < treesBlock.GetNumTrees() ; i++ )   {
		trees.push_back(treesBlock.GetTreeDescription(i));
        translationMaps.push_back(treesBlock.getTranslateList());
    }
}


void TreeFileReader::ReadPhylip()
{
	treefile.seekg(0);
	vector<Token> tokens = TokenizePhylip();
	
	for (unsigned i = 0 ; i < tokens.size() ; i++ ) {
		if ( tokens[i].GetType() != Token::CHAR )
			continue;

		// comments that sit between the trees, for example migrate's
		// "[& Locus 3, best ln(L) = -105593.947787 (c=coalescent node, ...) ]"
		// headers, are skipped completely.  Without this the parenthesis
		// inside such a comment would be mistaken for the start of a tree.
		if ( tokens[i].GetChar() == '[' )  {
			while ( i < tokens.size() &&
					!(tokens[i].GetType() == Token::CHAR &&
						tokens[i].GetChar() == ']' ))
				i++;
			continue;  // the for loop steps over the ']'
		}

		if ( tokens[i].GetChar() != '(' )
			continue;

		int numBraces = 1;
		i++;
		string tree = "(";
		bool done = false;
		
		while( i < tokens.size() && !done )  {
			if ( tokens[i].GetType() == Token::WORD )
				tree += tokens[i].GetWord();
			else switch (tokens[i].GetChar()) {
				case '(':
					numBraces++;
					tree += "(";
					break;
				case ';':
					while ( numBraces-- > 0 ) 
						tree += ")";
					tree += ";";
					done = true;
					break;
				case ')':
					numBraces--;
					tree += ')';
					//if ( numBraces == 0 ) {
				//		tree += ';';
				//		done = true;
				//	}
					break;
				case '[':
					while ( i < tokens.size() && 
							!(tokens[i].GetType() == Token::CHAR && 
								tokens[i].GetChar() == ']' )) {
						tree += tokens[i].GetWord();
						i++;
					} 
					tree += "]";
					break;
				case ']': //hmm shouldn't happen, but deal with it anyway
					tree += "]";
					break;
			}
			i++;
		}
		if ( !done )  {
			while ( numBraces-- > 0 ) 
				tree += ")";
			tree += ";";
		}
		trees.push_back(tree);
	}
}

vector<Token> TreeFileReader::TokenizePhylip() 
{
	vector<Token> tokens;
	string curString = "";
	char c;
	
	while ( treefile.get(c) ) {
		string theChar(1,c);
		if ( specialChars.find(theChar) == string::npos ) 
			curString += theChar;
		else {
			if ( curString != "" ) 
				tokens.push_back(Token(curString));
			tokens.push_back(Token(theChar[0]));
			curString = "";
		}
	}
    if ( curString != "" ) 
        tokens.push_back(Token(curString));
	return tokens;
}


string TreeFileReader::getNewick(int i) 
{
	return trees[i];
}


TreeFileReader::~TreeFileReader() {
}

void TreeFileReader::TranslateTree(Tree* t,unsigned treenum)
{
    if ( translationMaps.size() > treenum )
        for ( int i = 0 ; i < t->n_tips ; i++ ) {
        char* newname=strdup(
                translationMaps[treenum][t->tips[i]->name].c_str()); 
        if ( strlen(newname) > 0 )  {
            free(t->tips[i]->name);
            t->tips[i]->name = newname;

        }
        else  free(newname);
        }
    else  {
        for ( int i = 0 ; i < t->n_tips ; i++ ) {
            if ( t->tips[i]->name != NULL ) {
                for ( unsigned j = 0 ; j < strlen(t->tips[i]->name) ; j++ ) {
                    if ( t->tips[i]->name[j] == '_' ) 
                        t->tips[i]->name[j] = ' ';
                }
            }
        }
    }
}
