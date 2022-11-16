/**
 * Symbol (Simple) operators, i.e builtin Binary and Unary operations
 */

#pragma once
#include "./primary.hh"
#include "./type.hh"
#include "../../GenMacro.h"
#include "./type_utils.h"
#include "./reference.hh"
#include "./literal.hh"
enum UnaryOpType
{
    UNARY_ENUM_MEMBERS
};

enum BinaryOpType
{
    INFIX_ENUM_MEMBERS
};

static std::map<TokenType, UnaryOpType> prefix_op_mappings{PREFIX_ENUM_MAPPINGS};
static std::map<TokenType, UnaryOpType> postfix_op_mappings{POSTFIX_ENUM_MAPPINGS};
static std::map<TokenType, BinaryOpType> binary_op_mappings{INFIX_ENUM_MAPPINGS};

class UnaryOperation : public Expr
{
public:
    std::unique_ptr<Expr> operand;
    UnaryOpType op_type;
    std::string str()
    {
        return "Some Unary op";
    }

    UnaryOperation(std::unique_ptr<Expr> operand, UnaryOpType kind)
    {
        this->operand = std::move(operand);
        this->op_type = kind;
    }
    std::vector<Statement *> flatten()
    {
        std::vector<Statement *> ret = {this};
        for (auto m : operand->flatten())
            ret.push_back(m);
        return ret;
    }
    llvm::Value *assign_ptr(VariableTable &vars)
    {
        switch (op_type)
        {
        case PRE_star:
            return operand->llvm_value(vars);
        default:
            except(E_BAD_ASSIGNMENT, "Cannot get an assignable reference to unary operation of type " + std::to_string(op_type));
        }
    }
    llvm::Value *llvm_value(VariableTable &vars, llvm::Type *expected)
    {
        auto _bool = Primitive::get(PR_boolean)->llvm_type();

        #define bld (*builder())
        #define val(expected) operand->llvm_value(vars, expected)
        
        #define ret(LL_INST, cast_to) return cast(bld.Create##LL_INST(val(cast_to)), expected);
        switch (op_type)
        {
        case PRE_amperand:
        {
            auto ptr = operand->assign_ptr(vars);
            return cast(ptr, expected);
        }
        case PRE_bang: ret(Not, _bool);
        case PRE_minus:ret(Neg, nullptr);
        case PRE_bitwise_not:ret(Not, nullptr);
        case PRE_star:
        {
            if(!operand->type()->get<Ptr>())except(E_BAD_OPERAND, "Cannot dereference non-ptr operand");
            auto value = operand->llvm_value(vars);
            return builder()->CreateLoad(value->getType()->getPointerElementType(), value);
        }
        case PRE_increment:
        case PRE_decrement:
        case POST_increment:
        case POST_decrement:
        {
            auto ptr = operand->assign_ptr(vars);

            auto val = operand->llvm_value(vars);
            auto one = cast(bld.getInt32(1), val->getType());
            auto changed = op_type == POST_increment || op_type == POST_decrement ? bld.CreateAdd(val, one) : bld.CreateSub(val, one);

            llvm::Value* return_val = op_type == POST_increment || op_type == POST_decrement ? operand->llvm_value(vars, expected) 
                                    : changed;
            
            bld.CreateStore(changed, ptr);
            return return_val;
        }
        
        default:except(E_INTERNAL, "Failed to generate llvalue for unary operation: " + std::to_string(op_type));
        }
        
    }

protected:
    std::shared_ptr<Type> get_type()
    {
        auto bool_t = Primitive::get(PR_boolean);
        auto same_t = operand->type();
        auto same_ptr = Ptr::get(operand->type());
        auto pointee_t = same_t->pointee();
        switch (op_type)
        {
        case PRE_amperand:
            return same_ptr;
        case PRE_bang:
            return bool_t;
        case PRE_star:
            return pointee_t;
        case PRE_minus:
            return same_t;
        case PRE_bitwise_not:
            return same_t;
        case PRE_increment:
            return same_t;
        case PRE_decrement:
            return same_t;
        case POST_increment:
            return same_t;
        case POST_decrement:
            return same_t;
        default:
            except(E_INTERNAL, "Failed to get return type of unary operation: " + std::to_string(op_type));
        }
    }
};

class BinaryOperation : public Expr
{
public:
    std::unique_ptr<Expr> left_operand;
    std::unique_ptr<Expr> right_operand;
    BinaryOpType op_type;

    // The name of the variable that this expression
    // is responsible for initializing (if any)
    bool initializes;

    BinaryOperation(std::unique_ptr<Expr> left, std::unique_ptr<Expr> right, BinaryOpType op_type)
    {
        left_operand = std::move(left);
        right_operand = std::move(right);
        this->op_type = op_type;
    }
    std::string str()
    {
        return "Some Binary op";
    }
    llvm::Value *assign_ptr(VariableTable &vars)
    {
        if(op_type == BIN_dot){
            auto strct = left_operand->assign_ptr(vars);
            auto strct_t = left_operand->type()->get<StructType>();
            if(!strct_t)except(E_BAD_MEMBER_ACCESS, "Cannot access members of non-struct");

            if(!dynamic_cast<SourceVariable*>(right_operand.get()))except(E_BAD_MEMBER_ACCESS, "You can only index structs with identifiers");
            auto member_name = ((SourceVariable*)right_operand.get())->name->str();
            auto idx = strct_t->member_idx(member_name);

            auto ptr = builder()->CreateStructGEP(strct_t->llvm_type(), strct, idx);
            return ptr;
            strct->print(llvm::outs());
        }
        except(E_BAD_ASSIGNMENT, "Binary Operations are not assignable");
    }
    llvm::Value *llvm_value(VariableTable &vars, llvm::Type *expected_type = nullptr)
    {
        if(op_type == BIN_dot){
            auto ptr = assign_ptr(vars);
            auto load = builder()->CreateLoad(ptr->getType()->getPointerElementType(), ptr);
            return load;
        }
        llvm::Value* value = nullptr;
        if (op_type == BIN_assignment)
        {
            auto assignee = left_operand->assign_ptr(vars);


            // constant assignment check
            if (auto var = dynamic_cast<SourceVariable *>(left_operand.get()))
            {
                auto &va = vars[var->name->str()];
                if (this->initializes && !va.is_initialized)
                {
                    va.is_initialized = true;
                }
                else
                {
                    if (va.is_initialized && va.constant)
                        except(E_BAD_ASSIGNMENT, "Cannot reassign a constant variable: " + var->name->str());
                }
            }

            if(auto literal = dynamic_cast<ArrayLiteral*>(right_operand.get())){
                literal->write_to(assignee, vars);
                // no need to set the value, as the preprocessor enforces that array literals may only
                // be used during initialization
            }
            else{
                value = right_operand->llvm_value(vars, assignee->getType()->getPointerElementType());
                builder()->CreateStore(value, assignee);
            }

            return cast(value, expected_type);
        }

        auto left_t = left_operand->type();
        auto right_t = right_operand->type();

        auto common_t = TypeUtils::get_common_type(left_t, right_t);

        auto left_val = left_operand->llvm_value(vars, common_t->llvm_type());
        auto right_val = right_operand->llvm_value(vars, common_t->llvm_type());

        auto op = get_op(left_val, right_val);
        return cast(op, expected_type);
    }

    std::vector<Statement *> flatten()
    {
        std::vector<Statement *> ret = {this};
        for (auto m : left_operand->flatten())
            ret.push_back(m);
        for (auto m : right_operand->flatten())
            ret.push_back(m);
        return ret;
    }

private:
    llvm::Value *get_op(llvm::Value *l, llvm::Value *r)
    {
#define X(myop, opname) \
    case BIN_##myop:    \
        return builder()->Create##opname(l, r);
        switch (op_type)
        {
            X(plus, Add)
            X(minus, Sub)
            X(star, Mul)
            X(slash, SDiv)
            X(percent, SRem)
            X(lesser, ICmpSLT)
            X(greater, ICmpSGT)
            X(lesser_eq, ICmpSLE)
            X(greater_eq, ICmpSGE)
            X(not_equals, ICmpNE)
            X(equals, ICmpEQ)
            X(bitwise_or, Or)
            X(bitwise_and, And)
            X(bitwise_shl, Shl)
            X(bitwise_shr, AShr)
            X(bitwise_xor, Xor)
            X(bool_and, LogicalAnd)
            X(bool_or, LogicalOr)
        default:
            except(E_INTERNAL, "Failed to generate binary operation");
        }
    }

protected:
    std::shared_ptr<Type> get_type()
    {
        auto boo = Primitive::get(PR_boolean);
        switch (op_type)
        {
            case BIN_bool_and:
            case BIN_bool_or:
            case BIN_greater:
            case BIN_greater_eq:
            case BIN_equals:
            case BIN_not_equals:
            case BIN_lesser:
            case BIN_lesser_eq:
            return boo;

            case BIN_plus:
            case BIN_minus:
            case BIN_star:
            case BIN_slash:
            case BIN_percent:
            case BIN_bitwise_or:
            case BIN_bitwise_and:
            case BIN_bitwise_shl:
            case BIN_bitwise_shr:
            case BIN_bitwise_xor:


                return TypeUtils::get_common_type(left_operand->type(), right_operand->type());


                case BIN_assignment:
                return right_operand->type();
            case BIN_dot:{
                auto left_t = left_operand->type();
                if(!dynamic_cast<SourceVariable*>(right_operand.get()))except(E_BAD_MEMBER_ACCESS, "A struct may only be indexed with identifiers");
                auto key = (SourceVariable*)right_operand.get();
                if(!left_t)return std::shared_ptr<Type>(nullptr);
                if(auto strct = left_t->get<StructType>()){
                    if(strct->member_idx(key->name->str()) == -1)except(E_BAD_MEMBER_ACCESS, "The struct: " + left_operand->str() + " does not have a member: " + key->name->str());
                    return strct->members[key->name->str()];
                }
                else except(E_BAD_MEMBER_ACCESS, "You may only access members of a struct");
            }
            default:
                except(E_INTERNAL, "Failed to get type for op: " + std::to_string(op_type));
            }
    }
};
