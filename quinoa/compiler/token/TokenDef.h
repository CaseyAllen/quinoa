
/**
    THIS FILE IS AUTOMATICALLY GENERATED
    PLEASE DO NOT MAKE EDITS TO THIS FILE
    THEY WILL BE OVERRIDED

    to edit this file, instead edit the template at `scripts/generateTokenDefinitions.py`
    and afterwards, run the script to generate the changes
*/
#pragma once

#include<string>
#include<vector>
#include "../../GenMacro.h"

enum TokenType{
	DEFINITIONS_ENUM_MEMBERS
};

class TokenDefinition{
public:
    TokenType ttype;
    std::string name;
    
	DEFINITIONS_STR;

    TokenDefinition(TokenType ttype, std::string name DEFINITIONS_ARGS){
        this->ttype = ttype;
		this->name=name;
		DEFINITIONS_DEFAULT_ASSIGNMENTS;
    }
};


static std::vector<TokenDefinition*> defs{
    DEFINITIONS_INITIALIZERS
};
