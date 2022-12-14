#pragma once

#include "../../lib/logger.h"
#include "../llvm_utils.h"
#include "./ast.hh"
#include "./include.hh"
#include "llvm/IR/Value.h"
#include <map>

#include "../../lib/list.h"
#include "llvm/IR/Type.h"

class Type;

/**
 * A Convenient wrapper around llvm types
 * Also contains the quinoa type from which it derives from
 */
class LLVMType {
  public:
    _Type qn_type;

    llvm::Type* operator->();

    LLVMType(_Type qn_type);
    LLVMType() { this->is_explicitly_constructed = false; }
    LLVMType(llvm::Type* ll, _Type qn) {
        if (!qn)
            except(E_INTERNAL, "(bug) cannot construct an LLVMType without the "
                               "corresponding quinoa type");
        this->cached_type = ll;
        this->qn_type = qn;
    }

    bool operator==(LLVMType& other) const;

    bool is_signed();

    operator bool() const { return is_explicitly_constructed; }

    operator llvm::Type*();
    llvm::Type* get_type() const;

  private:
    bool is_explicitly_constructed = true;
    llvm::Type* cached_type = nullptr;
};

class LLVMValue {
  public:
    LLVMType type;
    llvm::Value* val;

    LLVMValue(llvm::Value* val, LLVMType type) {
        this->val = val;
        this->type = type;
    }

    llvm::Value* operator->() const { return val; }
    operator llvm::Value*() { return val; }
    LLVMValue load();

    void print();
};

class Scope;
class Block;
enum ReturnChance {
    NEVER,
    MAYBE,
    DEFINITE,
};

class Method;

class Statement : public ANode {
  public:
    virtual void generate(Method* qn_fn, llvm::Function* func, VariableTable& vars, ControlFlowInfo CFI) = 0;

    virtual std::string str() = 0;

    virtual std::vector<Statement*> flatten() = 0;

    virtual ReturnChance returns() = 0;
    virtual std::vector<Type*> flatten_types() = 0;
    Scope* scope = nullptr;
    Block* block();
};
/**
 * An Expression is similar to a statement, except:
 * 1. It returns a value
 * 2. It is inherently Typed
 */
class Expr : public Statement {
  public:
    _Type type() {
        // if (recalculate_type || !cached_type) {
        // auto typ = get_type();
        // recalculate_type = false;
        // cached_type = typ;
        // }
        return get_type();
    }

    void generate(Method* qn_fn, llvm::Function* func, VariableTable& vars, ControlFlowInfo CFI) {
        // Generate the expression as a statement
        // This is common for use-cases such as calls (where the function has
        // side effects)
        llvm_value(vars);
    }

    ReturnChance returns() { return ReturnChance::NEVER; }

    virtual LLVMValue llvm_value(VariableTable& vars, LLVMType expected_type = {}) = 0;

    virtual LLVMValue assign_ptr(VariableTable& vars) = 0;

  protected:
    virtual _Type get_type() = 0;

    Expr* parent_expr = nullptr;

  private:
};

enum JumpType {
    BREAK,
    CONTINUE,
    FALLTHROUGH,
};

class ControlFlowJump : public Statement {
  public:
    JumpType type;

    ControlFlowJump(JumpType type) { this->type = type; }
    std::vector<Type*> flatten_types() { return {}; }
    void generate(Method* qn_fn, llvm::Function* func, VariableTable& vars, ControlFlowInfo CFI) {
        switch (type) {
        case JumpType::BREAK: {
            if (!CFI.breakTo)
                except(E_BAD_CONTROL_FLOW, "Cannot break from a non-breakable scope");
            builder()->CreateBr(CFI.breakTo);
            break;
        }
        case JumpType::CONTINUE: {
            if (!CFI.continueTo)
                except(E_BAD_CONTROL_FLOW, "Cannot continue from a non-continuable scope");
            builder()->CreateBr(CFI.continueTo);
            break;
        }
        case JumpType::FALLTHROUGH: {
            if (!CFI.fallthroughTo)
                except(E_BAD_CONTROL_FLOW, "Cannot fallthrough from a non-fallthroughable scope");
            builder()->CreateBr(CFI.fallthroughTo);
            break;
        }
        default:
            except(E_INTERNAL, "Bad control flow jump type");
        }
    }

    std::string str() {
        switch (type) {
        case BREAK:
            return "break";
        case CONTINUE:
            return "continue";
        case FALLTHROUGH:
            return "fallthrough";
        default:
            return "unknown_cfj_op";
        }
    }

    std::vector<Statement*> flatten() { return {this}; }

    ReturnChance returns() { return ReturnChance::NEVER; }
};

/**
 * A 'scope' represents a context in which all type references
 * are equal, and all variable references are also equal
 */
#include <regex>

class Scope {
  public:
    Scope(Scope* parent) { this->parent_scope = parent; }
    Scope() = default;

    void set_type(std::string var_name, _Type typ) { type_table[var_name] = typ; }
    void set_parent(Scope* parent) { this->parent_scope = parent; }
    _Type get_type(std::string var_name) {

        if (type_table.contains(var_name))
            return type_table[var_name];
        else if (parent_scope) {
            return parent_scope->get_type(var_name);
        } else {

            except(E_UNRESOLVED_TYPE, "Failed to get type of '" + var_name + "'", false);
            return _Type(nullptr);
        }
    }
    virtual ~Scope() = default;
    void decl_new_variable(std::string name, _Type type, bool is_constant = false);
    Variable* get_var(std::string name);

    std::map<std::string, Variable> vars;
    std::map<std::string, _Type> type_table;
    Scope* parent_scope = nullptr;
};

class Block : public Scope, public Statement {
  public:
    Vec<Statement> content;
    std::vector<Type*> flatten_types();
    std::string str() {
        std::string output = "{\n";
        for (auto item : content) {
            auto str = "   " + item->str();
            str = std::regex_replace(str, std::regex("\n"), "\n   ");
            output += str + ";";
            output += "\n";
        }
        return output + "}";
    }

    Block() = default;
    Block(Scope* ps) : Scope(ps){};

    void generate(Method* qn_fn, llvm::Function* func, VariableTable& vars, ControlFlowInfo CFI);

    ReturnChance returns() {

        bool maybe_return = false;
        for (auto item : content) {

            //
            // example:
            // ``
            // break;
            // return expr;
            // ``
            // the return is never reached
            //
            if (dynamic_cast<ControlFlowJump*>(item.ptr))
                return ReturnChance::NEVER;

            if (item->returns() == ReturnChance::MAYBE)
                maybe_return = true;
            if (item->returns() == ReturnChance::DEFINITE)
                return ReturnChance::DEFINITE;
        }

        return maybe_return ? ReturnChance::MAYBE : ReturnChance::NEVER;
    }

    Statement* parent_of(Statement* node) {
        auto content = this->flatten();
        if (!includes(content, node))
            return nullptr;

        size_t check_idx = indexof(content, node) - 1;

        while (check_idx >= 0) {
            auto current_node = content[check_idx];
            auto current_children = current_node->flatten();
            if (includes(current_children, node))
                return current_node;
            check_idx--;
        }
        return nullptr;
    }

    std::vector<Statement*> flatten() {
        std::vector<Statement*> ret = {this};
        for (auto c : content)
            for (auto m : c->flatten())
                ret.push_back(m);
        return ret;
    }
};
