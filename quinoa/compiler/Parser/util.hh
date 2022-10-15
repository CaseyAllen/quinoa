#pragma once
#include<vector>
#include "../token/token.h"
#include "../../lib/list.h"
#include "../token/TokenDef.h"
#include<map>
#include "../../lib/error.h"
using namespace std;
std::string getTokenTypeName(TokenType t){
    for(auto def:defs){
        if(def->ttype == t){
            if(def->alias.size()){
                return def->alias[0];
            }
            return def->name;
        }
    }
    return "Unknown";
}
vector<Token> readUntil(vector<Token>& toks, TokenType type, bool removeEnding=false){
    vector<Token> retval;
    while(toks.size()){
        if(toks[0].is(type))break;
        retval.push_back(popf(toks));
    }
    if(removeEnding)popf(toks);
    return retval;
}
void expects(Token tok, TokenType expected){
    if(!tok.is(expected)){
        std::string message = tok.position();
        message += " Unexpected "+ getTokenTypeName(tok.type) + " " +tok.value+" ";
        if(expected!=TT_notok)
        message += "( expected '" + getTokenTypeName(expected) + "' )";
        error(message);
    }
}

enum IndType{
    IND_angles,
    INDENTATION_TYPES
};

map<IndType, pair<TokenType, TokenType>> mappings{
    {IND_angles, {TT_lesser, TT_greater}},
    INDENTATION_MAPPINGS
};

vector<Token> readBlock(vector<Token>& toks, IndType typ){
    auto p = mappings[typ];
    auto i = p.first;
    auto u = p.second;
    expects(toks[0], i);
    popf(toks);
    int ind=1;
    vector<Token> ret;
    Token t;
    while(toks.size()){
        t = popf(toks);
        if(t.is(i))ind++;
        if(t.is(u))ind--;
        if(ind==0)break;
        ret.push_back(t);
    }
    if(ind!=0)error("Expected '" + getTokenTypeName(u) + "' at " + t.afterpos());
    return ret;
}