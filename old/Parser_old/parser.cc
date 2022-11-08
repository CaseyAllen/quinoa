#include "./parser.h"
#include "../../lib/list.h"
#include "../../lib/logger.h"
#include "../token/token.h"
#include "./util.hh"
#include <vector>
using namespace std;

void printToks(std::vector<Token> toks, bool oneLine = false)
{
    if(oneLine) {
	string output;
	for(auto t : toks) {
	    output += t.value;
	}
    } else {
	for(auto t : toks) {
	    printf("-> [%s] %s\n", getTokenTypeName(t.type).c_str(), t.value.c_str());
	}
    }
}
CompoundIdentifier* parse_compound_ident(vector<Token>& toks, SourceBlock* ctx)
{
    expects(toks[0], TT_identifier);
    CompoundIdentifier ident;
    bool expectDot = false;
    for(auto t : toks) {
	if(expectDot) {
	    if(!t.is(TT_double_colon))
		break;
	    expectDot = false;
	    continue;
	}
	if(t.is(TT_identifier)) {
	    ident.push_back(Ident::get(t.value, ctx));
	    expectDot = true;
	    continue;
	}
	break;
    }
    for(unsigned int i = 0; i < ident.size() * 2 - 1; i++)
	popf(toks);
    auto id = new CompoundIdentifier(ident);
    id->ctx = ctx;
    return id;
}
vector<vector<Token>> parse_cst(vector<Token>& toks)
{
    if(toks.size() == 0)
	return {};
    vector<TokenType> indt;
    vector<TokenType> dindt;
    for(auto d : defs) {
	if(d->ind)
	    indt.push_back(d->ttype);
	else if(d->dind)
	    dindt.push_back(d->ttype);
    }
    vector<vector<Token>> retVal;
    vector<Token> temp;
    int ind = 0;
    while(toks.size()) {
	auto t = popf(toks);
	if(includes(indt, t.type))
	    ind++;
	if(includes(dindt, t.type))
	    ind--;

	if(ind == 0 && t.is(TT_comma)) {
	    retVal.push_back(temp);
	    temp.clear();
	} else {
	    temp.push_back(t);
	}
    }
    retVal.push_back(temp);
    return retVal;
}
Expression* parse_expr(vector<Token>& toks, SourceBlock* ctx);
Type* parse_type(vector<Token>& toks, SourceBlock* ctx)
{
    if(!toks.size())
	except(E_BAD_TYPE, "Failed To Parse Type");
    Type* ret = nullptr;
    // Special case for strings
    if(toks[0].is(TT_string)) {
	popf(toks);
	ret = new TPtr(Primitive::get(PR_int8));
    } else if(toks[0].isTypeTok())
	ret = Primitive::get(primitive_mappings[popf(toks).type]);
    else {
	bool is_inst = true;
	if(toks[0].is(TT_at_symbol)) {
	    is_inst = false;
	    popf(toks);
	}
	auto references = parse_compound_ident(toks, ctx);
	auto ct = new CustomType(references);
	ret = ct;
	if(is_inst) {
	    ret = new ModuleInstanceType(ct);
	}
    }

    while(toks.size() && toks[0].is(TT_star)) {
	popf(toks);
	ret = new TPtr(ret);
    }
    if(toks.size() && toks[0].is(TT_l_square_bracket)) {
	// Type Array
	auto size = readBlock(toks, IND_square_brackets);
	if(size.size() == 0)
	    ret = new ListType(ret, nullptr);
	else
	    ret = new ListType(ret, parse_expr(size, ctx));
    }

    // Generic Parameter to type
    if(toks.size() && toks[0].is(TT_lesser)) {
	if(!ret->custom())
	    except(E_INTERNAL, "You can only pass type-parameters to a named type-reference");
	auto argsBlock = readBlock(toks, IND_generics);
	auto csv = parse_cst(argsBlock);
	Block<Type> args;
	for(auto gp : csv) {
	    auto gt = parse_type(gp, ctx);
	    args.push_back(gt);
	}
	ret->custom()->type_args = args.take();
    }
    return ret;
}

struct Identifier_Segment {
    std::string name;
    Block<Type> type_args;
};

Identifier_Segment parse_ident_segment(vector<Token>& toks, SourceBlock* ctx)
{
    auto name = popf(toks);
    expects(name, TT_identifier);
    Identifier_Segment seg;
    seg.name = name.value;

    Block<Type> generic_args;

    if(toks[0].is(TT_lesser)) {
	auto block = readBlock(toks, IND_generics);
	auto cst = parse_cst(block);
	for(auto typ : cst) {
	    auto type = parse_type(typ, ctx);
	    generic_args.push_back(type);
	}
    }
    seg.type_args = generic_args;
    return seg;
}

vector<Identifier_Segment> parse_segmented_identifier(vector<Token>& toks, SourceBlock* ctx)
{
    vector<Identifier_Segment> segs;
    while(toks.size()) {
	auto id = parse_ident_segment(toks, ctx);
	segs.push_back(id);
	if(!toks[0].is(TT_double_colon))
	    break;
	popf(toks);
    }
    return segs;
}

TLCMemberRef* parse_memberref_from_segments(vector<Identifier_Segment> segments)
{
    if(segments.size() < 1)
	except(E_INTERNAL, "Call must consist of at least 1 segment");
    Identifier_Segment end = segments[segments.size() - 1];
    vector<Identifier_Segment> modname(segments.begin(), segments.end() - 1);
    if(segments.size() == 1) {
	modname.clear();
    }
    Block<Type> mod_type_args;
    CompoundIdentifier modname_ident;
    int i = 0;
    for(auto p : modname) {
	modname_ident.push_back(Ident::get(p.name));
	if(i == 0) {
	    mod_type_args = p.type_args;
	} else if(p.type_args.size()) {
	    except(E_BAD_ARGS, "Cannot pass Type Arguments to non-module");
	}
	i++;
    }

    TLCRef* modref = nullptr;
    if(modname_ident.size()) {
	modref = new TLCRef();
	modref->name = new CompoundIdentifier(modname_ident);
    }
    auto ref = new TLCMemberRef(modref, Ident::get(end.name));
    return ref;
}

Expression* parse_expr(vector<Token>& toks, SourceBlock* ctx)
{
    if(toks.size() == 0)
	except(E_INTERNAL, "Cannot Generate an Expression from no tokens");
    if(toks.size() == 1) {
	switch(toks[0].type) {
	case TT_literal_int:
	    return new Integer(stoull(toks[0].value));
	case TT_literal_true:
	    return new Boolean(true);
	case TT_literal_false:
	    return new Boolean(false);
	case TT_literal_str:
	    return new String(toks[0].value);
	case TT_literal_float:
	    return new Float(stold(toks[0].value));
	case TT_identifier: {
	    return Ident::get(toks[0].value, ctx);
	}
	default:
	    except(E_INTERNAL,
	        "Failed To Generate an Appropriate Constant Value for '" + getTokenTypeName(toks[0].type) + "'");
	}
    }

    auto c = toks[0];

    // Unwrap Parenthesis
    if(c.is(TT_l_paren)) {
	auto initial = toks;
	auto block = readBlock(toks, IND_parens);
	if(toks.size() == 0)
	    return parse_expr(block, ctx);

	toks = initial;
    }
    // Create a function call
    // Function Calls can come in many forms
    //
    // foo::bar();
    // foo<T>::bar();
    // foo::bar<T>();
    // foo<T>::bar<U>();
    // foo::bar<T>::baz();
    // foo::bar<T>::baz<U>();
    // foo::bar::baz<T>();
    //
    if(c.is(TT_identifier)) {
	auto initial = toks;
	auto segments = parse_segmented_identifier(toks, ctx);
	auto generic_args = segments[segments.size() - 1].type_args;
	auto memberRef = parse_memberref_from_segments(segments);
	if(toks[0].is(TT_l_paren)) {
	    Block<Expression> parameters;
	    auto params_block = readBlock(toks, IND_parens);
	    if(toks.size() == 0) {
		auto params_cst = parse_cst(params_block);
		for(auto pt : params_cst) {
		    auto param_expr = parse_expr(pt, ctx);
		    param_expr->ctx = ctx;
		    parameters.push_back(param_expr);
		}

		auto call = new MethodCall();
		call->ctx = ctx;
		call->generic_params = generic_args;
		call->params = parameters;
		call->name = memberRef;
		return call;
	    }
	}

	toks = initial;
    }

    // Compound variable (property access [remote / explicit])
    if(c.is(TT_identifier)) {
	auto initial = toks;
	auto ident = parse_compound_ident(toks, ctx);
	if(!toks.size()) {
	    ident->ctx = ctx;
	    return ident;
	}
	toks = initial;
    }

}

SourceBlock* parse_source(vector<Token> toks, SourceBlock* predecessor, LocalTypeTable typeinfo)
{
    auto block = new SourceBlock;
    auto type_info = new LocalTypeTable;
    *type_info = typeinfo;
    block->local_types = type_info;
    while(toks.size()) {
	auto first = toks[0];
	if(first.is(TT_while)) {
	    popf(toks);
	    auto expr = readBlock(toks, IND_parens);
	    auto exec = readBlock(toks, IND_braces);
	    auto cond = parse_expr(expr, block);
	    auto content = parse_source(exec, block, *type_info);
	    auto loop = new WhileCond;
	    cond->ctx = block;
	    loop->local_types = new LocalTypeTable;
	    loop->gobble(content);
	    loop->cond = cond;
	    loop->ctx = block;
	    // For some reason the loop becomes inactive at this point
	    loop->active = true;
	    block->push_back(loop);
	    continue;
	}
	if(first.is(TT_if)) {
	    popf(toks);
	    auto cond = readBlock(toks, IND_parens);
	    auto condExpr = parse_expr(cond, block);
	    condExpr->ctx = block;

	    auto iff = new IfCond;
	    iff->cond = condExpr;

	    auto does = readBlock(toks, IND_braces);
	    auto doesA = parse_source(does, block, *type_info);
	    if(!doesA)
		except(E_BAD_CONDITIONAL, "Failed to parse conditional block");
	    iff->does = doesA;
	    if(toks[0].is(TT_else)) {
		popf(toks);
		auto otherwise = readBlock(toks, IND_braces);
		iff->otherwise = parse_source(otherwise, block, *type_info);
	    }
	    block->push_back(iff);
	    continue;
	}
	if(first.is(TT_for)) {
	    popf(toks);
	    auto inner = readBlock(toks, IND_parens);
	    auto exec = readBlock(toks, IND_braces);

	    auto init = readUntil(inner, TT_semicolon);
	    auto sc = popf(inner);
	    init.push_back(sc);
	    auto check = readUntil(inner, TT_semicolon, true);
	    inner.push_back(sc);
	    auto initCode = parse_source(init, block, *type_info);
	    block->gobble(initCode);

	    auto checkCode = parse_expr(check, block);
	    auto incCode = parse_source(inner, block, *type_info);
	    auto source = parse_source(exec, block, *type_info);
	    auto loop = new ForRange;

	    loop->cond = checkCode;
	    loop->inc = incCode;
	    loop->ctx = block;
	    loop->local_types = new LocalTypeTable;
	    loop->gobble(source);
	    block->push_back(loop);

	    continue;
	}
	auto f = toks[0];
	auto line = readUntil(toks, TT_semicolon, true);
	if(line.size() == 0)
	    expects(f, TT_notok);
	f = line[0];

	if(f.is(TT_return)) {
	    popf(line);
	    Expression* returnValue = nullptr;
	    if(line.size()) {
		returnValue = parse_expr(line, block);
		returnValue->ctx = block;
	    }
	    auto ret = new Return(returnValue);
	    ret->ctx = block;
	    block->push_back(ret);
	    continue;
	} else if(f.is(TT_let) || f.is(TT_const)) {
	    popf(line);
	    auto varname = popf(line).value;
	    Type* vartype;
	    if(line[0].is(TT_colon)) {
		popf(line);
		vartype = parse_type(line, block);
	    } else
		vartype = nullptr;
	    auto name = Ident::get(varname);
	    name->ctx = block;

	    // Add the variable to the type table
	    (*type_info)[varname] = vartype;
	    auto init = new InitializeVar(vartype, name);
	    init->ctx = block;
	    init->constant = f.is(TT_const);
	    if(line.size() != 0) {
		expects(popf(line), TT_assignment);
		auto val = parse_expr(line, block);
		val->ctx = block;
		init->initializer = val;
	    } else if(init->constant) {
		except(E_UNINITIALIZED_CONST, "Uninitialized Constant: " + init->varname->str());
	    }
	    block->push_back(init);
	    continue;
	}

	// Default to expression parsing
	auto expr = parse_expr(line, block);
	expr->ctx = block;

	block->push_back(expr);
    }

    return block;
}

Param* parse_param(vector<Token>& toks)
{
    bool isVarParam = false;
    if(!toks.size())
	except(E_BAD_PARAMETER, "Cannot parse parameter with no tokens");
    if(toks[0].is(TT_ellipsis)) {
	isVarParam = true;
	popf(toks);
    }
    auto name = popf(toks);
    expects(name, TT_identifier);
    expects(popf(toks), TT_colon);
    auto type = parse_type(toks, nullptr);

    if(toks.size()) {
	printToks(toks);
	except(E_BAD_PARAMETER, "Failed to Parse Parameter");
    }
    auto p = new Param(type, Ident::get(name.value));
    p->isVariadic = isVarParam;
    return p;
}

Generic* parse_generic(vector<Token>& toks, SourceBlock* ctx)
{
    auto gen_name = popf(toks);
    expects(gen_name, TT_identifier);
    auto gen = new Generic(Ident::get(gen_name.value));

    if(toks.size()) {
	auto colon = popf(toks);
	expects(colon, TT_colon);
	if(!toks.size())
	    expects(colon, TT_notok);
	auto constraint = parse_type(toks, ctx);
	gen->constraint = constraint;
    }
    return gen;
}

/**
 * Generalized Implementation for parsing the content of modules / seeds / any future containers
 *
 */
Block<ModuleMember> parse_tlc_members(vector<Token>& toks, TLContainer* parent)
{
    Block<ModuleMember> ret;
    Block<TopLevelMetadata> metadata;
    bool isPublic = false;
    bool isInstance = false;
    while(toks.size()) {
	auto c = popf(toks);

	// Method parsing
	if(c.is(TT_func)) {
	    auto nameTok = popf(toks);
	    expects(nameTok, TT_identifier);
	    auto method = new Method();
	    method->memberOf = parent;
	    Block<Generic> generic_args;
	    if(toks[0].is(TT_lesser)) {
		auto genericTokens = readBlock(toks, IND_generics);
		auto csv = parse_cst(genericTokens);
		for(auto gen : csv) {
		    auto arg = parse_generic(gen, method);
		    generic_args.push_back(arg);
		}
	    }
	    expects(toks[0], TT_l_paren);
	    auto argsTokens = readBlock(toks, IND_parens);
	    auto argsCSV = parse_cst(argsTokens);
	    Block<Param> params;
	    LocalTypeTable initial_types;

	    // Inject the args
	    for(auto a : argsCSV) {
		auto param = parse_param(a);
		// TODO: Implement fancier generic finder
		// try to resolve parameters to generic types if at all possible
		if(auto tp = param->type->custom()) {
		    for(auto g : generic_args) {
			if(g->name->str() == tp->name->str()) {
			    tp->refersTo = g;
			    break;
			}
		    }
		}

		initial_types[param->name->str()] = param->type;
		params.push_back(param);
	    }

	    Type* returnType;
	    method->local_types = new LocalTypeTable;

	    // Functions implicitly return void
	    if(toks[0].is(TT_arrow)) {
		popf(toks);
		returnType = parse_type(toks, method);
		auto flat = returnType->flatten();
		for(auto styp : flat) {
		    if(auto typ = dynamic_cast<Type*>(styp)) {
			if(auto ct = typ->custom()) {
			    for(auto g : generic_args) {

				if(g->name->str() == ct->name->str()) {
				    ct->refersTo = g;
				    break;
				}
			    }
			}
		    }
		}
	    } else {
		returnType = Primitive::get(PR_void);
	    }

	    // keep track of the arg types too
	    *method->local_types = initial_types;
	    auto sig = new MethodSignature();
	    sig->generics = generic_args;
	    if(toks[0].is(TT_l_brace)) {
		auto contentToks = readBlock(toks, IND_braces);
		auto content = parse_source(contentToks, method, initial_types);
		method->gobble(content);
	    } else {
		expects(toks[0], TT_semicolon);
		popf(toks);
	    }

	    method->sig = sig;

	    auto name = new TLCMemberRef;
	    name->parent = new TLCRef(parent);
	    name->parent->name = parent->fullname();
	    ;
	    name->member = Ident::get(nameTok.value);
	    sig->name = name;

	    sig->params = params.take();
	    sig->returnType = returnType;
	    sig->belongsTo = method;

	    method->metadata = metadata;
	    method->instance_access = isInstance;
	    method->public_access = isPublic;

	    metadata.clear();
	    isInstance = false;
	    isPublic = false;

	    parent->push_back(method);
	    continue;
	}

	// Property Parsing
	if(c.is(TT_let)) {
	    auto line = readUntil(toks, TT_semicolon, true);
	    auto propname_tok = popf(line);
	    expects(propname_tok, TT_identifier);
	    auto name = propname_tok.value;
	    expects(popf(line), TT_colon);
	    auto type = parse_type(line, nullptr);

	    auto prop = new Property;
	    prop->type = type;
	    prop->name = new TLCMemberRef;
	    prop->name->member = Ident::get(name);
	    prop->name->parent = parent->get_ref();
	    prop->instance_access = isInstance;
	    prop->public_access = isPublic;
	    isInstance = false;
	    isPublic = false;
	    // Possible Initializer, not required;
	    if(line.size()) {
		expects(popf(line), TT_assignment);
		auto initial_value = parse_expr(line, nullptr);
		prop->initializer = initial_value;
	    }

	    ret.push_back(prop);
	    continue;
	}
	// Metadata
	if(c.is(TT_hashtag)) {
	    auto content = readBlock(toks, IND_square_brackets);

	    auto name = popf(content);
	    expects(name, TT_identifier);

	    auto paramsToks = readBlock(content, IND_parens);
	    auto paramsCSV = parse_cst(paramsToks);
	    Block<Expression> params;
	    for(auto p : paramsCSV) {
		auto expr = parse_expr(p, nullptr);
		params.push_back(expr);
	    }
	    auto meta = new TopLevelMetadata;
	    meta->name = name.value;
	    meta->parameters = params.take();
	    metadata.push_back(meta);
	    continue;
	}

	if(c.is(TT_public_access)) {
	    if(isPublic)
		expects(c, TT_notok);
	    isPublic = true;
	    continue;
	}
	if(c.is(TT_instance_access)) {
	    if(isInstance)
		expects(c, TT_notok);
	    isInstance = true;
	    continue;
	}

	except(E_ERR, "Failed to generate member");
    }
    return ret;
}

void parse_mod(vector<Token>& toks, Module* mod)
{
    auto content = parse_tlc_members(toks, mod);
    for(auto m : content)
	mod->push_back(m);
}

void parse_seed(vector<Token>& toks, Seed* seed)
{
}

Compositor* parse_compositor(vector<Token>& toks)
{
    auto name = parse_compound_ident(toks, nullptr);
    auto c = new Compositor;
    c->name = name;

    if(toks.size())
	except(E_ERR, "Failed to parse module compositor");
    return c;
}

CompilationUnit* Parser::makeAst(vector<Token>& toks)
{
    auto unit = new CompilationUnit;
    while(toks.size()) {
	auto c = popf(toks);

	// Top-Level Parsing
	switch(c.type) {
	case TT_import: {
	    // Imports are handled/resolved in the preprocessor
	    auto importExprToks = readUntil(toks, TT_semicolon, true);
	    bool isStd = false;
	    if(importExprToks[0].is(TT_at_symbol)) {
		popf(importExprToks);
		isStd = true;
	    }
	    auto target = parse_compound_ident(importExprToks, nullptr);
	    CompoundIdentifier* alias = target;
	    if(importExprToks.size()) {
		std::vector<Ident*> specific_members;
		if(importExprToks[0].is(TT_double_colon) && importExprToks[1].is(TT_l_brace)) {
		    popf(importExprToks);
		    auto members = readBlock(importExprToks, IND_braces);
		    auto members_csv = parse_cst(members);
		    for(auto m : members_csv) {
			auto name = m[0];
			expects(name, TT_identifier);
			specific_members.push_back(Ident::get(name.value));
		    }
		    if(importExprToks.size())
			expects(importExprToks[0], TT_notok);
		    for(auto member : specific_members) {
			auto import = new Import(target, isStd, new CompoundIdentifier(member->name));
			import->member = member;
			unit->push_back(import);
		    }
		    break;
		}
	    }
	    if(importExprToks.size()) {
		auto as = popf(importExprToks);
		expects(as, TT_as);
		if(importExprToks.size() == 0)
		    error("Expected An Import Alias at " + as.afterpos());
		expects(importExprToks[0], TT_identifier);
		alias = new CompoundIdentifier(popf(importExprToks).value);
		if(importExprToks.size())
		    except(E_BAD_IMPORT_ALIAS, "An import alias may only consist of one token");
	    }
	    unit->push_back(new Import(target, isStd, alias));
	    // Import Path foo.bar.baz
	    break;
	}
	case TT_module:
	case TT_seed: {
	    bool is_seed = c.is(TT_seed);
	    TLContainer* cont = nullptr;
	    if(is_seed)
		cont = new Seed();
	    else
		cont = new Module();

	    cont->unit = unit;
	    auto name = popf(toks);
	    Block<Generic> generic_parameters;
	    if(toks[0].is(TT_lesser)) {
		auto block = readBlock(toks, IND_generics);
		auto generic_params = parse_cst(block);
		for(auto gp : generic_params) {
		    auto generic = parse_generic(gp, nullptr);
		    generic_parameters.push_back(generic);
		}
	    }
	    Block<Compositor> compositors;
	    if(toks[0].is(TT_is)) {
		popf(toks);
		auto compositorToks = readUntil(toks, TT_l_brace);
		auto csv = parse_cst(compositorToks);
		for(auto member : csv) {
		    compositors.push_back(parse_compositor(member));
		}
	    }
	    auto moduleToks = readBlock(toks, IND_braces);
	    cont->name = Ident::get(name.value);
	    cont->compositors = compositors.take();
	    cont->generics = generic_parameters;
	    if(is_seed)
		parse_seed(moduleToks, static_cast<Seed*>(cont));
	    else
		parse_mod(moduleToks, static_cast<Module*>(cont));
	    unit->push_back(cont);
	    break;
	}
	default:
	    error("Failed to parse Token '" + c.value + "'");
	}
    }
    return unit;
}