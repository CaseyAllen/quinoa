
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

enum TokenType{
    
		TT_eof,
		TT_comment,
		TT_identifier,
		TT_void,
		TT_string,
		TT_int8,
		TT_int16,
		TT_int32,
		TT_int64,
		TT_uint8,
		TT_uint16,
		TT_uint32,
		TT_uint64,
		TT_float16,
		TT_float32,
		TT_float64,
		TT_double_quote,
		TT_hashtag,
		TT_quote,
		TT_l_paren,
		TT_r_paren,
		TT_l_brace,
		TT_r_brace,
		TT_l_square_bracket,
		TT_r_square_bracket,
		TT_comma,
		TT_colon,
		TT_semicolon,
		TT_question_mark,
		TT_at_symbol,
		TT_backslash,
		TT_percent,
		TT_postfix_inc,
		TT_postfix_dec,
		TT_subscript,
		TT_prefix_inc,
		TT_prefix_dec,
		TT_instantiate_object,
		TT_bang,
		TT_star,
		TT_plus,
		TT_minus,
		TT_bool_or,
		TT_bool_and,
		TT_dot,
		TT_slash,
		TT_lesser,
		TT_greater,
		TT_lesser_eq,
		TT_greater_eq,
		TT_assignment,
		TT_equals,
		TT_not_equals,
		TT_bitwise_and,
		TT_bitiwse_or,
		TT_bitwise_not,
		TT_bitwise_xor,
		TT_bitwise_shl,
		TT_bitwise_shr,
		TT_underscore,
};

class TokenDefinition{
public:
    TokenType ttype;
    
	std::vector<std::string> alias;
	bool type = false;
	bool ind = false;
	bool dind = false;
	int infix = 0;
	int postfix = 0;
	int prefix = 0;

    TokenDefinition(TokenType ttype, std::vector<std::string> alias, bool type = false, bool ind = false, bool dind = false, int infix = 0, int postfix = 0, int prefix = 0){
        this->ttype = ttype;
		this->alias = alias;
		this->type = type;
		this->ind = ind;
		this->dind = dind;
		this->infix = infix;
		this->postfix = postfix;
		this->prefix = prefix;
    }
};


std::vector<TokenDefinition*> defs{
    
	new TokenDefinition(TT_eof, {}, false, false, false, 0, 0, 0),
	new TokenDefinition(TT_comment, {}, false, false, false, 0, 0, 0),
	new TokenDefinition(TT_identifier, {}, false, false, false, 0, 0, 0),
	new TokenDefinition(TT_void, {"void"}, true, false, false, 0, 0, 0),
	new TokenDefinition(TT_string, {"string"}, true, false, false, 0, 0, 0),
	new TokenDefinition(TT_int8, {"i8""char"}, true, false, false, 0, 0, 0),
	new TokenDefinition(TT_int16, {"i16"}, true, false, false, 0, 0, 0),
	new TokenDefinition(TT_int32, {"i32""int"}, true, false, false, 0, 0, 0),
	new TokenDefinition(TT_int64, {"i64"}, true, false, false, 0, 0, 0),
	new TokenDefinition(TT_uint8, {"u8""byte"}, true, false, false, 0, 0, 0),
	new TokenDefinition(TT_uint16, {"u16"}, true, false, false, 0, 0, 0),
	new TokenDefinition(TT_uint32, {"u32""uint"}, true, false, false, 0, 0, 0),
	new TokenDefinition(TT_uint64, {"u64"}, true, false, false, 0, 0, 0),
	new TokenDefinition(TT_float16, {"f16"}, true, false, false, 0, 0, 0),
	new TokenDefinition(TT_float32, {"f32"}, true, false, false, 0, 0, 0),
	new TokenDefinition(TT_float64, {"f64""float"}, true, false, false, 0, 0, 0),
	new TokenDefinition(TT_double_quote, {"\""}, false, false, false, 0, 0, 0),
	new TokenDefinition(TT_hashtag, {"#"}, false, false, false, 0, 0, 0),
	new TokenDefinition(TT_quote, {"'"}, false, false, false, 0, 0, 0),
	new TokenDefinition(TT_l_paren, {"("}, false, true, false, 0, 0, 0),
	new TokenDefinition(TT_r_paren, {")"}, false, false, true, 0, 0, 0),
	new TokenDefinition(TT_l_brace, {"{"}, false, true, false, 0, 0, 0),
	new TokenDefinition(TT_r_brace, {"}"}, false, false, true, 0, 0, 0),
	new TokenDefinition(TT_l_square_bracket, {"["}, false, true, false, 0, 0, 0),
	new TokenDefinition(TT_r_square_bracket, {"]"}, false, false, true, 0, 0, 0),
	new TokenDefinition(TT_comma, {","}, false, false, false, 0, 0, 0),
	new TokenDefinition(TT_colon, {":"}, false, false, false, 0, 0, 0),
	new TokenDefinition(TT_semicolon, {";"}, false, false, false, 0, 0, 0),
	new TokenDefinition(TT_question_mark, {"?"}, false, false, false, 0, 0, 0),
	new TokenDefinition(TT_at_symbol, {"@"}, false, false, false, 0, 0, 0),
	new TokenDefinition(TT_backslash, {"\\"}, false, false, false, 0, 0, 0),
	new TokenDefinition(TT_percent, {"%"}, false, false, false, 5, 0, 0),
	new TokenDefinition(TT_postfix_inc, {"++"}, false, false, false, 0, 3, 0),
	new TokenDefinition(TT_postfix_dec, {"--"}, false, false, false, 0, 3, 0),
	new TokenDefinition(TT_subscript, {}, false, false, false, 0, 2, 0),
	new TokenDefinition(TT_prefix_inc, {"++"}, false, false, false, 0, 0, 3),
	new TokenDefinition(TT_prefix_dec, {"--"}, false, false, false, 0, 0, 3),
	new TokenDefinition(TT_instantiate_object, {"create"}, false, false, false, 0, 0, 3),
	new TokenDefinition(TT_bang, {"!"}, false, false, false, 0, 0, 3),
	new TokenDefinition(TT_star, {"*"}, false, false, false, 5, 0, 0),
	new TokenDefinition(TT_plus, {"+"}, false, false, false, 6, 0, 0),
	new TokenDefinition(TT_minus, {"-"}, false, false, false, 6, 0, 0),
	new TokenDefinition(TT_bool_or, {"||"}, false, false, false, 15, 0, 0),
	new TokenDefinition(TT_bool_and, {"&&"}, false, false, false, 14, 0, 0),
	new TokenDefinition(TT_dot, {"."}, false, false, false, 4, 0, 0),
	new TokenDefinition(TT_slash, {"/"}, false, false, false, 5, 0, 0),
	new TokenDefinition(TT_lesser, {"<"}, false, false, false, 9, 0, 0),
	new TokenDefinition(TT_greater, {">"}, false, false, false, 9, 0, 0),
	new TokenDefinition(TT_lesser_eq, {"<="}, false, false, false, 9, 0, 0),
	new TokenDefinition(TT_greater_eq, {">="}, false, false, false, 9, 0, 0),
	new TokenDefinition(TT_assignment, {"="}, false, false, false, 16, 0, 0),
	new TokenDefinition(TT_equals, {"=="}, false, false, false, 10, 0, 0),
	new TokenDefinition(TT_not_equals, {"!="}, false, false, false, 10, 0, 0),
	new TokenDefinition(TT_bitwise_and, {"&"}, false, false, false, 11, 0, 0),
	new TokenDefinition(TT_bitiwse_or, {"|"}, false, false, false, 13, 0, 0),
	new TokenDefinition(TT_bitwise_not, {"~"}, false, false, false, 3, 0, 0),
	new TokenDefinition(TT_bitwise_xor, {"^"}, false, false, false, 13, 0, 0),
	new TokenDefinition(TT_bitwise_shl, {"<<"}, false, false, false, 7, 0, 0),
	new TokenDefinition(TT_bitwise_shr, {">>"}, false, false, false, 7, 0, 0),
	new TokenDefinition(TT_underscore, {"_"}, false, false, false, 0, 0, 0),
};
