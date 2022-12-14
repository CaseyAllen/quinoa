#include "./lexer.h"
#include "../../lib/error.h"
#include "../../lib/list.h"
#include "../../lib/logger.h"
#include "../token/token.h"
#include <bits/stdc++.h>
#include <string>
#include <vector>
using namespace std;
int row = 1;
int col = 1;
static std::string filename;
void newline()
{
    row++;
    col = 1;
}
Token make(TokenType type, std::string value = "")
{
    Token tok(type, value, row, col, filename);
    return tok;
}
void trimStart(string& str)
{
    // Ascii Table Shennanigans
    while(str.length() && str[0] <= 32) {
	if(str[0] == '\n')
	    newline();
	else
	    col++;
	popf(str);
    }
}
bool startsWith(string str, std::string substr)
{
    if(substr.size() > str.size())
	return false;
    if(substr.size() == 0)
	return true;
    if(substr.size() == 1)
	return str[0] == substr[0];
    for(unsigned int i = 0; i < substr.size(); ++i) {
	auto tgt = str[i];
	auto cmp = substr[i];
	if(tgt != cmp)
	    return false;
    }
    return true;
}
bool isNumber(char c)
{
    return c >= '0' && c <= '9';
}
bool isAlphaChar(char c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}
bool isExtVarChar(char c)
{
    return c == '_' || c == '$';
}
bool isSymbol(char c)
{
    return !isNumber(c) && !isAlphaChar(c) && !isExtVarChar(c);
}
bool isSymbol(string str)
{
    for(auto c : str) {
	if(!isSymbol(c))
	    return false;
    }
    return true;
}

bool is_valid_octal_char(char c){
    return c >= '0' && c < '8';
}
bool is_valid_hex_char(char c){
    return (c >= '0' && c <= '9') || (c >= 'A' && c<= 'F');
}
int hex_to_int(char h){
    if( h >= '0' && h <= '9')return h - '0';
    else if(h >= 'A' && h <= 'F')return (h - 'A') + 10;
    else except(E_INTERNAL, "bad hexadecimal character: " + std::string()+h);
}
string escapeNextVal(string& str)
{
    auto first = popf(str);


    if(first == 'u') {
        // Unicode sequence parsing
        except(E_INTERNAL, "unicode parser not implemented "  + str);
    }
    else if(first == 'x') {
        // Hex parsing, expects a sequence of 2 valid chars
        char codepoint = 0;
        if(str.size() < 3)except(E_BAD_ESCAPE, "An octal requires at least 3 characters, but only " + std::to_string(str.size()) + " were found");

        if(!is_valid_hex_char(str[0]))except(E_BAD_ESCAPE, "A hex must contain characters in the range 0-F (first character)");
        if(!is_valid_hex_char(str[1]))except(E_BAD_ESCAPE, "A hex must contain characters in the range 0-F (second character)");

        codepoint += hex_to_int(popf(str)) * 16;
        codepoint += hex_to_int(popf(str)) * 1;

        std::string str;
        str.push_back(codepoint);
        return str;
    }
    else if(first == 'o'){
        // octal parsing, expects a sequence of 3 valid chars
        char codepoint = 0;
        if(str.size() < 3)except(E_BAD_ESCAPE, "An octal requires at least 3 characters, but only " + std::to_string(str.size()) + " were found");

        if(!is_valid_octal_char(str[0]))except(E_BAD_ESCAPE, "An octal must contain characters in the range 0-8 (first character)");
        if(!is_valid_octal_char(str[1]))except(E_BAD_ESCAPE, "An octal must contain characters in the range 0-8 (second character)");
        if(!is_valid_octal_char(str[2]))except(E_BAD_ESCAPE, "An octal must contain characters in the range 0-8 (third character)");

        codepoint += (popf(str) - '0') * 64;
        codepoint += (popf(str) - '0') * 8;
        codepoint += (popf(str) - '0') * 1;

        std::string str;
        str.push_back(codepoint);
        return str;

    }

#define X(from, to)case from: return to
    switch(first) {
        X('"', "\"");
        X('n', "\n");
        X('t', "\t");
        X('r', "\r");
        X('f', "\f");
        X('a', "\a");
        X('b', "\b");
        X('\\', "\\");
        X('v', "\v");
        X('0', "\0");
        X('1', "\1");
        X('2', "\2");
        X('3', "\3");
        X('4', "\4");
        X('5', "\5");
        X('6', "\6");
        X('7', "\7");
        X('?', "\?");
    }
#undef X
    except(E_UNESCAPABLE, "Failed To Escape string");
}
bool compareLength(string s1, std::string s2)
{
    return s1.length() > s2.length();
}

inline vector<string> get_aliases()
{
    // Keyword Check (this is black magic, god have mercy on my soul)
    vector<string> aliases;
    // Construct a vector with all the aliases, sorted by length (big->small)
    for(auto def : defs) {
	for(auto alias : def->alias) {
	    aliases.push_back(alias);
	}
    }
    sort(aliases.begin(), aliases.end(), compareLength);
    return aliases;
}

Token readNextToken(string& str)
{
    // Trim
    trimStart(str);
    if(str.length() == 0)
	return make(TT_eof);

    // Comment Check
    if(startsWith(str, "//")) {
	while(str[0] != '\n')
	    popf(str);
	popf(str);
	return readNextToken(str);
    }
    if(startsWith(str, "/*")) {
	while(!startsWith(str, "*/"))
	    popf(str);
	popf(str);
	popf(str);
	return readNextToken(str);
    }

    // string Check
    if(startsWith(str, "\"")) {
        std::string val;
        popf(str);
        bool escapeNext = false;
        while(str.length()) {
            if(escapeNext) {
            val.append(escapeNextVal(str));
            escapeNext = false;
            continue;
            } else if(str[0] == '\\') {
            popf(str);
            escapeNext = true;
            continue;
            } else if(str[0] == '"') {
            popf(str);
            break;
            } else {

            val += popf(str);
            }
        }
        // TODO: This isn't actually accurate due to the width of escaped characters being smaller than their
        // source-code variants
        col += val.size() + 2;
        return make(TT_literal_str, val);
    }

    // Number Check
    if(isNumber(str[0]) || (str.size() >= 2 && (str[0] == '-' && isNumber(str[1])))){
        std::string constructedNumber;
        if(str[0] == '-')constructedNumber+= popf(str);
        bool has_encountered_decimal = false;
        while(isNumber(str[0]) || str[0] == '.') {
            if(str[0] == '.'){
                if(has_encountered_decimal)except(E_UNEXPECTED_DECIMAL, "An unexpected decimal place has been encountered (i.e  123.456.7 < )");
                has_encountered_decimal = true;
            }
            constructedNumber += str[0];

            popf(str);
        }
        col += constructedNumber.size();
        return make(has_encountered_decimal ? TT_literal_float : TT_literal_int, constructedNumber);
    }

    static auto aliases = get_aliases();

    for(auto a : aliases) {
	// Check for a match
	if(startsWith(str, a)) {
	    auto symbol = isSymbol(a);
	    if(!symbol) {
            // ensure the character after the keyword isn't an alphachar
            auto next = str[a.size()];
            if(isAlphaChar(next) || isExtVarChar(next))
                continue;
	    }
	    // Locate the appropriate def object and create an appropriate token type
	    for(auto d : defs) {
		if(includes(d->alias, a)) {
		    // skip n tokens ahead;
		    for(unsigned int i = 0; i < a.size(); i++) {
			popf(str);
		    }
		    col += a.size();
		    return make(d->ttype, a);
		}
	    }
	}
    }

    // Literal Parsing, accepts literally Anything
    if(isAlphaChar(str[0]) || isExtVarChar(str[0])) {
	std::string ident;
	while(str.length() && (isAlphaChar(str[0]) || isNumber(str[0]) || isExtVarChar(str[0]))) {
	    ident += popf(str);
	}
	col += ident.size();
	if(ident != "")
	    return make(TT_identifier, ident);
    }
    std::string s;
    s += str[0];
    except(E_UNREADABLE_CHAR, "Failed To Parse The File, An Unreadable Character '" + s + "' was Encountered");
    throw exception();
}

vector<Token> Lexer::lexify(string source, std::string fname)
{
    vector<Token> toks;
    filename = fname;
    while(1) {
	auto nextToken = readNextToken(source);
	if(nextToken.is(TT_eof))
	    break;
	toks.push_back(nextToken);
    }
    row = 1;
    col = 1;
    return toks;
}