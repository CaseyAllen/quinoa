#include "./codegen.hh"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Linker/Linker.h"
#include <string>




llvm::GlobalValue *make_global(
	Property *prop,						   
	llvm::Module *mod,
	llvm::GlobalValue::LinkageTypes linkage = llvm::GlobalValue::LinkageTypes::LinkOnceAnyLinkage
	)
{
	auto type = prop->type->llvm_type();
	llvm::Constant *const_initializer = nullptr;
	if (prop->initializer) const_initializer = prop->initializer->const_value(type);
	else{
        except(E_ERR, "Property " + prop->name->str() + " must have an initializer");
    }
	
	auto global = new llvm::GlobalVariable(*mod, type, false, linkage, const_initializer, prop->name->str());
	return global;
}

VariableTable generate_variable_table(llvm::Function *fn, CompilationUnit &ast, Method *method)
{
	if (fn == nullptr)
		except(E_INTERNAL, "Cannot varify the args of a null function");
	VariableTable vars;

	// Inject the peer properties as non-prefixed variables
	for (auto prop : method->parent->get_properties())
	{
		auto prop_name = prop->name->member->str();
		auto init = (llvm::AllocaInst *)fn->getParent()->getGlobalVariable(prop->name->str());
		vars[prop_name] = Variable(prop->type, init);
	}



	// Inject all properties as full variables
	for (auto prop : ast.get_properties())
	{
		auto prop_name = prop->name->str();
		auto init = (llvm::AllocaInst *)fn->getParent()->getGlobalVariable(prop->name->str());
		vars[prop_name] = Variable(prop->type, init);
	}

    // Inject All Enum Members as variables
    for(auto type : ast.get_types()){
        if(auto _enum = dynamic_cast<EnumType*>(type->refers_to.get())){
            for(auto member : _enum->get_members()){
                auto alloc = new llvm::GlobalVariable(*fn->getParent(), member.second->getType(), false, llvm::GlobalValue::LinkOnceODRLinkage, member.second);
                auto full_name = type->name->str() + "::" + member.first;
                alloc->setName(full_name);
                vars[full_name] = Variable(type->refers_to, (llvm::AllocaInst*)alloc, true);

                if(type->parent == method->parent){
                    auto local_name = type->name->member->str() + "::" + member.first;
                    vars[local_name] = Variable(type->refers_to, (llvm::AllocaInst*)alloc, true);
                }
            }
        }
    }

    if(auto ty = method->get_target_type()){
        auto arg = fn->getArg( method->must_parameterize_return_val() ? 1 : 0);
        auto alloc = builder()->CreateAlloca(arg->getType(), nullptr, "self");
        builder()->CreateStore(arg, alloc);
        vars["self"] = Variable(Ptr::get(ty), alloc);
    }

	// Inject the args as variables

    //TODO: add the get_parameter_offset function as doing this repeatedly is bound to create bugs if new features are added
    int diff = method->acts_upon ? 1 : 0;
    if(method->must_parameterize_return_val())diff++;

	for (unsigned int i = 0; i < method->parameters.len(); i++)
	{
		auto& param = method->parameters[i];
		auto arg = fn->getArg(i+diff);
		auto alloc = builder()->CreateAlloca(arg->getType(), nullptr, "!" + arg->getName().str());

		builder()->CreateStore(arg, alloc);
		vars[param.name.str()] = Variable(param.type, alloc);
	}

	return vars;
}

void generate_method(Method* fn, CompilationUnit& ast, llvm::Module* ll_mod, bool allow_generic = false){
    if(fn->is_generic() && !allow_generic)return;

    auto fname = fn->source_name();
    auto ll_fn = ll_mod->getFunction(fname);
    if (ll_fn == nullptr)
    {
    ll_mod->print(llvm::outs(), nullptr);
        except(E_MISSING_FUNCTION, "Function " + fname + " could not be found");
    }
    if(ll_fn->getBasicBlockList().size()){
      return;
    }
    auto entry_block = llvm::BasicBlock::Create(*llctx(), "entry_block", ll_fn);

    builder()->SetInsertPoint(entry_block);
    auto vars = generate_variable_table(ll_fn, ast, fn);

    for(auto g : fn->generic_params){
        Logger::debug(">\t" + g->name->str() + " >< " + g->temporarily_resolves_to->str());  
    }
    fn->content->generate(fn, ll_fn, vars, {});

    auto retc = fn->content->returns();
    if (ll_fn->getReturnType()->isVoidTy() && (retc == ReturnChance::NEVER || retc == ReturnChance::MAYBE || fn->must_parameterize_return_val()))
        builder()->CreateRetVoid();
}
std::unique_ptr<llvm::Module> generate_module(Container &mod, CompilationUnit &ast)
{
  Logger::debug("GENERATE MODULE: " + mod.full_name().str());
	if(mod.type != CT_MODULE)except(E_INTERNAL, "cannot generate non-module container");
	auto llmod = std::make_unique<llvm::Module>(mod.name->str(), *llctx());

	// Write hoisted definitions
	for(auto hoist : ast.get_hoists()){
		if(auto method = dynamic_cast<Method*>(hoist)){
			make_fn(*method, llmod.get());
		}
		else if(auto prop = dynamic_cast<Property*>(hoist)){
			make_global(prop, llmod.get());
		}
		else except(E_INTERNAL, "Attempt to hoist unrecognized node");
	}

  // Pass one: concrete functions
	for(auto method : mod.get_methods()){
        if(method->generic_params.size())continue;
        if(!method->content || !method->content->content.len())continue;
        generate_method(method, ast, llmod.get());
	}

	return llmod;
}
llvm::Module *Codegen::codegen(CompilationUnit &ast)
{
	auto rootmod = new llvm::Module("Quinoa Program", *llctx());
	std::vector<TopLevelEntity*> definitions;



	// Generate all the modules, and link them into the root module
  
  std::vector<std::unique_ptr<llvm::Module>> mods;
  std::map<Container*, llvm::Module*> container_module_mappings;

	for(auto container : ast.get_containers()){
        if(container->type != CT_MODULE)continue;
        auto generated_mod = generate_module(*container, ast);
        container_module_mappings[container] = generated_mod.get();
        mods.push_back(std::move(generated_mod));
	}

  while(auto impl = ast.get_next_impl()){

    Logger::debug("######### GENERATING GENERIC IMPL FOR " + impl->target->source_name());
    Logger::debug("Generic Arguments:");
    for(auto a : impl->substituted_method_type_args){
      Logger::debug("\t> " + a->str());
    }
    Logger::debug("Target Arguments:");
    for(auto a : impl->substituted_target_type_args){
      Logger::debug("\t> " + a->str());
    }
    impl->target->apply_generic_substitution(impl->substituted_method_type_args, impl->substituted_target_type_args);

    make_fn(*impl->target, container_module_mappings[impl->target->parent], llvm::GlobalValue::ExternalLinkage, true);
    generate_method(impl->target, ast, container_module_mappings[impl->target->parent], true);

    impl->target->undo_generic_substitution();
    impl->has_impl = true;
  }
  
  for(auto& mod : mods){
   llvm::Linker::linkModules(*rootmod, std::move(mod));
  }
	return rootmod;
}
