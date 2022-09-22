#define DEFINITIONS_STR \
std::vector<std::string> alias;\
bool type = false;\
bool ind = false;\
bool dind = false;\
int infix = 0;\
int postfix = 0;\
int prefix = 0;\


#define DEFINITIONS_ARGS \
\
, std::vector<std::string> alias\
, bool type = false\
, bool ind = false\
, bool dind = false\
, int infix = 0\
, int postfix = 0\
, int prefix = 0

#define DEFINITIONS_DEFAULT_ASSIGNMENTS \
this->alias = alias;\
this->type = type;\
this->ind = ind;\
this->dind = dind;\
this->infix = infix;\
this->postfix = postfix;\
this->prefix = prefix;\


#define DEFINITIONS_INITIALIZERS \
new TokenDefinition(TT_eof, "__eof", {}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_comment, "__comment", {}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_identifier, "__identifier", {}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_literal_str, "__literal_str", {"l_string"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_literal_true, "__literal_true", {"l_true"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_literal_false, "__literal_false", {"l_false"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_literal_int, "__literal_int", {"l_int"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_literal_float, "__literal_float", {"l_float"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_literal_string, "__literal_string", {"l_string"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_literal_char, "__literal_char", {"l_char"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_void, "__void", {"void"}, true, false, false, 0, 0, 0),\
new TokenDefinition(TT_string, "__string", {"string"}, true, false, false, 0, 0, 0),\
new TokenDefinition(TT_int8, "__int8", {"i8", "char"}, true, false, false, 0, 0, 0),\
new TokenDefinition(TT_int16, "__int16", {"i16"}, true, false, false, 0, 0, 0),\
new TokenDefinition(TT_int32, "__int32", {"i32", "int"}, true, false, false, 0, 0, 0),\
new TokenDefinition(TT_int64, "__int64", {"i64"}, true, false, false, 0, 0, 0),\
new TokenDefinition(TT_uint8, "__uint8", {"u8", "byte"}, true, false, false, 0, 0, 0),\
new TokenDefinition(TT_uint16, "__uint16", {"u16"}, true, false, false, 0, 0, 0),\
new TokenDefinition(TT_uint32, "__uint32", {"u32", "uint"}, true, false, false, 0, 0, 0),\
new TokenDefinition(TT_uint64, "__uint64", {"u64"}, true, false, false, 0, 0, 0),\
new TokenDefinition(TT_float16, "__float16", {"f16"}, true, false, false, 0, 0, 0),\
new TokenDefinition(TT_float32, "__float32", {"f32"}, true, false, false, 0, 0, 0),\
new TokenDefinition(TT_float64, "__float64", {"f64", "float"}, true, false, false, 0, 0, 0),\
new TokenDefinition(TT_double_quote, "__double_quote", {"\""}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_hashtag, "__hashtag", {"#"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_quote, "__quote", {"'"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_l_paren, "__l_paren", {"("}, false, true, false, 0, 0, 0),\
new TokenDefinition(TT_r_paren, "__r_paren", {")"}, false, false, true, 0, 0, 0),\
new TokenDefinition(TT_l_brace, "__l_brace", {"{"}, false, true, false, 0, 0, 0),\
new TokenDefinition(TT_r_brace, "__r_brace", {"}"}, false, false, true, 0, 0, 0),\
new TokenDefinition(TT_l_square_bracket, "__l_square_bracket", {"["}, false, true, false, 0, 0, 0),\
new TokenDefinition(TT_r_square_bracket, "__r_square_bracket", {"]"}, false, false, true, 0, 0, 0),\
new TokenDefinition(TT_comma, "__comma", {","}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_colon, "__colon", {":"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_semicolon, "__semicolon", {";"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_question_mark, "__question_mark", {"?"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_at_symbol, "__at_symbol", {"@"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_backslash, "__backslash", {"\\"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_percent, "__percent", {"%"}, false, false, false, 5, 0, 0),\
new TokenDefinition(TT_powerOf, "__powerOf", {"**"}, false, false, false, 5, 0, 0),\
new TokenDefinition(TT_increment, "__increment", {"++"}, false, false, false, 0, 3, 3),\
new TokenDefinition(TT_decrement, "__decrement", {"--"}, false, false, false, 0, 3, 3),\
new TokenDefinition(TT_increment_by, "__increment_by", {"+="}, false, false, false, 16, 0, 0),\
new TokenDefinition(TT_decrement_by, "__decrement_by", {"-="}, false, false, false, 16, 0, 0),\
new TokenDefinition(TT_mul_by, "__mul_by", {"*="}, false, false, false, 16, 0, 0),\
new TokenDefinition(TT_div_by, "__div_by", {"/="}, false, false, false, 16, 0, 0),\
new TokenDefinition(TT_pow_of, "__pow_of", {"**="}, false, false, false, 16, 0, 0),\
new TokenDefinition(TT_bitwise_shl_by, "__bitwise_shl_by", {"<<="}, false, false, false, 16, 0, 0),\
new TokenDefinition(TT_bitwise_shr_by, "__bitwise_shr_by", {">>="}, false, false, false, 16, 0, 0),\
new TokenDefinition(TT_bitwise_or_by, "__bitwise_or_by", {"|="}, false, false, false, 16, 0, 0),\
new TokenDefinition(TT_bitwise_and_by, "__bitwise_and_by", {"&="}, false, false, false, 16, 0, 0),\
new TokenDefinition(TT_bitwise_xor_by, "__bitwise_xor_by", {"^="}, false, false, false, 16, 0, 0),\
new TokenDefinition(TT_subscript, "__subscript", {}, false, false, false, 0, 2, 0),\
new TokenDefinition(TT_instantiate_object, "__instantiate_object", {"create"}, false, false, false, 0, 0, 3),\
new TokenDefinition(TT_bang, "__bang", {"!"}, false, false, false, 0, 0, 3),\
new TokenDefinition(TT_star, "__star", {"*"}, false, false, false, 5, 0, 0),\
new TokenDefinition(TT_plus, "__plus", {"+"}, false, false, false, 6, 0, 0),\
new TokenDefinition(TT_minus, "__minus", {"-"}, false, false, false, 6, 0, 0),\
new TokenDefinition(TT_bool_or, "__bool_or", {"||"}, false, false, false, 15, 0, 0),\
new TokenDefinition(TT_bool_and, "__bool_and", {"&&"}, false, false, false, 14, 0, 0),\
new TokenDefinition(TT_dot, "__dot", {"."}, false, false, false, 4, 0, 0),\
new TokenDefinition(TT_slash, "__slash", {"/"}, false, false, false, 5, 0, 0),\
new TokenDefinition(TT_lesser, "__lesser", {"<"}, false, false, false, 9, 0, 0),\
new TokenDefinition(TT_greater, "__greater", {">"}, false, false, false, 9, 0, 0),\
new TokenDefinition(TT_lesser_eq, "__lesser_eq", {"<="}, false, false, false, 9, 0, 0),\
new TokenDefinition(TT_greater_eq, "__greater_eq", {">="}, false, false, false, 9, 0, 0),\
new TokenDefinition(TT_assignment, "__assignment", {"="}, false, false, false, 16, 0, 0),\
new TokenDefinition(TT_equals, "__equals", {"=="}, false, false, false, 10, 0, 0),\
new TokenDefinition(TT_not_equals, "__not_equals", {"!="}, false, false, false, 10, 0, 0),\
new TokenDefinition(TT_bitwise_and, "__bitwise_and", {"&"}, false, false, false, 11, 0, 0),\
new TokenDefinition(TT_bitiwse_or, "__bitiwse_or", {"|"}, false, false, false, 13, 0, 0),\
new TokenDefinition(TT_bitwise_not, "__bitwise_not", {"~"}, false, false, false, 3, 0, 0),\
new TokenDefinition(TT_bitwise_xor, "__bitwise_xor", {"^"}, false, false, false, 13, 0, 0),\
new TokenDefinition(TT_bitwise_shl, "__bitwise_shl", {"<<"}, false, false, false, 7, 0, 0),\
new TokenDefinition(TT_bitwise_shr, "__bitwise_shr", {">>"}, false, false, false, 7, 0, 0),\
new TokenDefinition(TT_underscore, "__underscore", {"_"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_if, "__if", {"if"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_while, "__while", {"while"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_for, "__for", {"for"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_break, "__break", {"break"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_continue, "__continue", {"continue"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_switch, "__switch", {"switch"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_case, "__case", {"case"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_fallthrough, "__fallthrough", {"fallthrough"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_import, "__import", {"import"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_module, "__module", {"module"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_is, "__is", {"is"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_as, "__as", {"as"}, false, false, false, 0, 0, 0),\
new TokenDefinition(TT_return, "__return", {"return"}, false, false, false, 0, 0, 0),\


#define DEFINITIONS_ENUM_MEMBERS \
TT_eof,\
TT_comment,\
TT_identifier,\
TT_literal_str,\
TT_literal_true,\
TT_literal_false,\
TT_literal_int,\
TT_literal_float,\
TT_literal_string,\
TT_literal_char,\
TT_void,\
TT_string,\
TT_int8,\
TT_int16,\
TT_int32,\
TT_int64,\
TT_uint8,\
TT_uint16,\
TT_uint32,\
TT_uint64,\
TT_float16,\
TT_float32,\
TT_float64,\
TT_double_quote,\
TT_hashtag,\
TT_quote,\
TT_l_paren,\
TT_r_paren,\
TT_l_brace,\
TT_r_brace,\
TT_l_square_bracket,\
TT_r_square_bracket,\
TT_comma,\
TT_colon,\
TT_semicolon,\
TT_question_mark,\
TT_at_symbol,\
TT_backslash,\
TT_percent,\
TT_powerOf,\
TT_increment,\
TT_decrement,\
TT_increment_by,\
TT_decrement_by,\
TT_mul_by,\
TT_div_by,\
TT_pow_of,\
TT_bitwise_shl_by,\
TT_bitwise_shr_by,\
TT_bitwise_or_by,\
TT_bitwise_and_by,\
TT_bitwise_xor_by,\
TT_subscript,\
TT_instantiate_object,\
TT_bang,\
TT_star,\
TT_plus,\
TT_minus,\
TT_bool_or,\
TT_bool_and,\
TT_dot,\
TT_slash,\
TT_lesser,\
TT_greater,\
TT_lesser_eq,\
TT_greater_eq,\
TT_assignment,\
TT_equals,\
TT_not_equals,\
TT_bitwise_and,\
TT_bitiwse_or,\
TT_bitwise_not,\
TT_bitwise_xor,\
TT_bitwise_shl,\
TT_bitwise_shr,\
TT_underscore,\
TT_if,\
TT_while,\
TT_for,\
TT_break,\
TT_continue,\
TT_switch,\
TT_case,\
TT_fallthrough,\
TT_import,\
TT_module,\
TT_is,\
TT_as,\
TT_return,\


#define INDENTATION_TYPES \
IND_parens,\
 IND_braces,\
 IND_square_brackets,\
 

#define INDENTATION_MAPPINGS \
{IND_parens, {TT_l_paren, TT_r_paren}},\
{IND_braces, {TT_l_brace, TT_r_brace}},\
{IND_square_brackets, {TT_l_square_bracket, TT_r_square_bracket}},\


#define INFIX_ENUM_MEMBERS \
BIN_percent, \
BIN_powerOf, \
BIN_increment_by, \
BIN_decrement_by, \
BIN_mul_by, \
BIN_div_by, \
BIN_pow_of, \
BIN_bitwise_shl_by, \
BIN_bitwise_shr_by, \
BIN_bitwise_or_by, \
BIN_bitwise_and_by, \
BIN_bitwise_xor_by, \
BIN_star, \
BIN_plus, \
BIN_minus, \
BIN_bool_or, \
BIN_bool_and, \
BIN_dot, \
BIN_slash, \
BIN_lesser, \
BIN_greater, \
BIN_lesser_eq, \
BIN_greater_eq, \
BIN_assignment, \
BIN_equals, \
BIN_not_equals, \
BIN_bitwise_and, \
BIN_bitiwse_or, \
BIN_bitwise_not, \
BIN_bitwise_xor, \
BIN_bitwise_shl, \
BIN_bitwise_shr, \


#define INFIX_ENUM_MAPPINGS \
{ TT_percent, BIN_percent}, \
{ TT_powerOf, BIN_powerOf}, \
{ TT_increment_by, BIN_increment_by}, \
{ TT_decrement_by, BIN_decrement_by}, \
{ TT_mul_by, BIN_mul_by}, \
{ TT_div_by, BIN_div_by}, \
{ TT_pow_of, BIN_pow_of}, \
{ TT_bitwise_shl_by, BIN_bitwise_shl_by}, \
{ TT_bitwise_shr_by, BIN_bitwise_shr_by}, \
{ TT_bitwise_or_by, BIN_bitwise_or_by}, \
{ TT_bitwise_and_by, BIN_bitwise_and_by}, \
{ TT_bitwise_xor_by, BIN_bitwise_xor_by}, \
{ TT_star, BIN_star}, \
{ TT_plus, BIN_plus}, \
{ TT_minus, BIN_minus}, \
{ TT_bool_or, BIN_bool_or}, \
{ TT_bool_and, BIN_bool_and}, \
{ TT_dot, BIN_dot}, \
{ TT_slash, BIN_slash}, \
{ TT_lesser, BIN_lesser}, \
{ TT_greater, BIN_greater}, \
{ TT_lesser_eq, BIN_lesser_eq}, \
{ TT_greater_eq, BIN_greater_eq}, \
{ TT_assignment, BIN_assignment}, \
{ TT_equals, BIN_equals}, \
{ TT_not_equals, BIN_not_equals}, \
{ TT_bitwise_and, BIN_bitwise_and}, \
{ TT_bitiwse_or, BIN_bitiwse_or}, \
{ TT_bitwise_not, BIN_bitwise_not}, \
{ TT_bitwise_xor, BIN_bitwise_xor}, \
{ TT_bitwise_shl, BIN_bitwise_shl}, \
{ TT_bitwise_shr, BIN_bitwise_shr}, \


#define PRIMITIVES_ENUM_MEMBERS \
PR_void,\
PR_string,\
PR_int8,\
PR_int16,\
PR_int32,\
PR_int64,\
PR_uint8,\
PR_uint16,\
PR_uint32,\
PR_uint64,\
PR_float16,\
PR_float32,\
PR_float64,\


