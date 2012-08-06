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


#ifndef TREEFILEREADER_H_
#define TREEFILEREADER_H_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include "treelib.h"
#include "ncl/ncl.h"

class Token {
public:
	Token(std::string word):type(WORD),word(word) {}  
	Token(char character):type(CHAR),word(1,character),character(character){}
	enum TokenType { CHAR, WORD };
	
	TokenType GetType() { return type; };
	std::string GetWord() { return word; };
	char GetChar() { return character;};
private:
	TokenType type;
	std::string word;
	char character;
};


class TreeFileReader {
	public:
		TreeFileReader(std::string filename);
		virtual int GetNumTrees() {return trees.size();}
		virtual std::string getNewick(int i);
		virtual std::vector<std::string> getAllNewicks() {return trees;}
		virtual ~TreeFileReader();
        virtual void TranslateTree(Tree* t, unsigned treenum);	
	
	private:
		enum FileType { PHYLIP, NEXUS };
		FileType type;
		std::vector<std::string> trees;
        std::vector<NxsStringMap > translationMaps;
		std::ifstream treefile;
		
    	void ReadPhylip();
    	void ReadNexus();
	    std::vector<Token> TokenizePhylip();
};

#endif
