#pragma once
#include "./ast.hh"
class Import:public TopLevelExpression{
public:
    CompoundIdentifier* target;
    bool isStdLib = false;
    CompoundIdentifier* alias;

    Import(CompoundIdentifier* tgt, bool std, CompoundIdentifier* alias){
        target = tgt;
        isStdLib = std;
        this->alias = alias;
    }
};
class ModuleReference:public Block<Expression>{
public:
    Identifier* name;

};

class Param : public AstNode{
public:
    Type* type = nullptr;
    Ident* name = nullptr;
    bool isVariadic = false;
    Param(Type* type, Ident* name){
        this->type = type;
        this->name = name;
    }
};
class MethodSigStr{
public:
    Ident* name;
    std::vector<Param*> params;

    std::string str(){
        std::string n = "fn_";
        n+=name->str();
        if(params.size()){
            n+="(";
            bool first = true;
            for(auto p:params){
                if(!first)n+=",";
                if(p->isVariadic)n+="...";
                n+=p->type->str();
                first = false;
            }
            n+=")";
        }
        return n;
    }
    bool isVariadic(){
        if(params.size()==0)return false;
        return params[params.size()-1]->isVariadic;
    }
    Param* getParam(int n){
        if(isVariadic()){
            if(n>params.size()-1)return params[params.size()-1];
            else return params[n];
        }
        else{
            if(n > params.size()-1)return nullptr;
            return params[n];            
        }
    }
};

class QualifiedMethodSigStr:public MethodSigStr{
public:
    CompoundIdentifier* space;
    QualifiedMethodSigStr() = default;
};

// Method definitions hold all the info of a method required to call
// and generate definitions for it
class MethodSignature:public AstNode{
public:
    Ident* name = nullptr;
    bool nomangle = false;
    CompoundIdentifier* space = nullptr;
    std::vector<Param*> params;
    Type* returnType = nullptr;

    CompoundIdentifier* fullname(){

        if(space==nullptr)return new CompoundIdentifier({name});
        return  new CompoundIdentifier({space, name});
    }

    std::string sourcename(){
        std::string ret;
        if(space!=nullptr){
            ret+=space->str()+".";
        }
        ret+=sigstr().str();
        return ret;
    }
    QualifiedMethodSigStr sigstr(){
        QualifiedMethodSigStr sigs;
        sigs.name = name;
        sigs.params = params;
        sigs.space = space;
        if(space==nullptr)error("Space is null?", true);
        return sigs;
    }
};
class MethodPredeclaration:public TopLevelExpression{
public:
    MethodSignature* sig;
};
class Method:public ModuleMember, public SourceBlock{
public:
    MethodSignature* sig = nullptr;
    // If nomangle is enabled, matching is done via names directly
    // instead of using the overload chosing algorithm
    std::vector<Type*> paramTypes(){
        std::vector<Type*> ret;
        for(auto p:sig->params){
            ret.push_back(p->type);
        }
        return ret;
    }
    QualifiedMethodSigStr sigstr(){
        return sig->sigstr();
    }
    CompoundIdentifier* fullname(){
        return sig->fullname();
    }
private:
};
class Entrypoint:public TopLevelExpression, public Method{
public:
    MethodSignature* calls;
    Entrypoint(MethodSignature* calls){
        this->calls = calls;
    }

};
class Module:public TopLevelExpression, public Block<ModuleMember>{
public:
    CompoundIdentifier* name;
    std::vector<ModuleReference*> compositors; 


    bool is(std::string comp){
        for(auto c:compositors){
            if(c->name->str() == comp)return true;
        }
        return false;
    }  
    Method* getMethod(std::string name){
        for(auto m:this->items){
            auto mt = (Method*)m;
            if(instanceof<Method>(m) && mt->sig->name->str() == name)return mt;

        }
        return nullptr;
    }
    bool hasMethod(std::string name){
        return this->getMethod(name) != nullptr;
    } 
};



