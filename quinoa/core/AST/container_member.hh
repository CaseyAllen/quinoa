/**
 * Container Members are anything that goes inside a container
 * includes:
 * - Properties
 * - Methods
 */

#pragma once

#include "./constant.hh"
#include "./reference.hh"
#include "type.hh"
#include <string>

class Container;

class Attribute {
  public:
    std::string name;
    Vec<ConstantValue> arguments;
};

class ContainerMember : public ANode {
  public:
    std::unique_ptr<ContainerMemberRef> name;
    Container* parent;
    std::unique_ptr<Scope> scope;
    Vec<Attribute> attrs;
    bool local_only = false;

    bool aliases_resolved = false;
};

class Property : public ContainerMember {
  public:
    std::unique_ptr<ConstantValue> initializer;
    _Type type;
    bool constant = false;
};

class TypeMember : public ContainerMember {
  public:
    _Type refers_to;
    std::vector<std::shared_ptr<Generic>> generic_args;
    TypeMember(_Type t) { this->refers_to = t; }
};

class Param : public ANode {
  public:
    Name name;
    _Type type;
    bool is_variadic = false;

    Param(Name pname, _Type type) : name(pname) { this->type = std::move(type); }
};

struct MethodSignature {
    std::vector<std::shared_ptr<Generic>> generic_params;
    Vec<Param> parameters;
    _Type return_type;

    bool is_variadic() {
        // Check if the last parameter is a var-arg
        if (!parameters.len())
            return false;
        auto& last = parameters[parameters.len() - 1];
        return last.is_variadic;
    }

    Param* get_parameter(size_t idx) {
        if (idx < parameters.len())
            return &parameters[idx];
        else if (is_variadic())
            return &parameters[parameters.len() - 1];
        else
            return nullptr;
    }

    bool must_parameterize_return_val() { return is_preallocated_retval(*return_type); }

    static bool is_preallocated_retval(Type& ty) {
        if (auto pt = ty.get<ParameterizedTypeRef>()) {
            return is_preallocated_retval(*pt->resolves_to);
        }
        return ty.get<StructType>() || ty.get<DynListType>() || ty.get<ListType>() || ty.get<TupleType>();
    }
};

class Method : public MethodSignature, public ContainerMember {
  public:
    std::shared_ptr<TypeRef> acts_upon;
    std::vector<std::shared_ptr<Generic>> acts_upon_generic_args;

    std::unique_ptr<Block> content;

    unsigned application_count = 0;

    void apply_generic_substitution(TypeVec types, TypeVec acts_upon_types = {}) {
        if (types.size() != generic_params.size() || acts_upon_types.size() != acts_upon_generic_args.size()) {
            Logger::debug(std::to_string(types.size()) + " & " + std::to_string(acts_upon_types.size()) + "  vs  " +
                          std::to_string(generic_params.size()) + " & " +
                          std::to_string(acts_upon_generic_args.size()));

            except(E_BAD_SUBSTITUTION,
                   "Cannot substitute generics with mismatched counts\n\t\tIn function: " + this->name->str());
        }
        for (unsigned i = 0; i < types.size(); i++) {
            this->generic_params[i]->temporarily_resolves_to = types[i];
        }
        for (unsigned i = 0; i < acts_upon_types.size(); i++) {
            this->acts_upon_generic_args[i]->temporarily_resolves_to = acts_upon_types[i];
        }
        application_count++;
    }
    GenericTable generics_as_table() {
        GenericTable ret;
        for (auto g : generic_params) {
            if (!g->temporarily_resolves_to)
                except(E_INTERNAL, "Generic does not resolve");
            ret[g->name->str()] = g->temporarily_resolves_to;
        }
        return ret;
    }
    void undo_generic_substitution() {
        for (auto g : generic_params) {
            g->temporarily_resolves_to.reset();
        }
        for (auto g : acts_upon_generic_args) {
            g->temporarily_resolves_to.reset();
        }
        application_count--;
        if (application_count < 0) {
            except(E_INTERNAL, "negative application count");
        }
    }
    // Get the parameter at a specific index
    // this method is smart and accounts for varargs
    // returns `nullptr` if there is no parameter at the given index

    bool is_generic() { return generic_params.size() || this->acts_upon_generic_args.size(); }

    _Type get_target_type() {
        if (!acts_upon)
            return nullptr;
        if (acts_upon->is_generic()) {
            TypeVec resolved_generics;
            for (auto t : acts_upon_generic_args) {
                resolved_generics.push_back(t->drill()->self);
            }
            auto ptref = ParameterizedTypeRef::get(acts_upon, resolved_generics);
            return ptref;
        } else
            return acts_upon;
    }

    std::string source_name() {
        if (this->name->trunc)
            return this->name->str();
        std::string name;
        name += this->name->mangle_str();
        if (this->generic_params.size()) {
            name += "<";
            bool first = true;
            for (auto g : generic_params) {
                if (!first)
                    name += ",";
                name += g->str();
                first = false;
            }
            name += ">";
        }
        for (auto p : parameters) {
            name += "." + p->type->str();
        }
        return name;
    }
    // Similar to source_name, but intended for user-friendly error messages
    std::string signature();
};
