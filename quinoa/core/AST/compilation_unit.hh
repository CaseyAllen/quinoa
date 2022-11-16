#pragma once
#include "./include.hh"
#include "./container.hh"
#include "./container_member.hh"

class CompilationUnit: public ANode{
public:
    Vec<TopLevelEntity> members;

    std::vector<Container*> get_containers(){
        std::vector<Container*> ret;
        for(auto m:members){
            if(auto mod = dynamic_cast<Container*>(m.ptr)){
                ret.push_back(mod);
            }            
        }
        return ret;
    }
    std::vector<std::unique_ptr<TopLevelEntity>> transfer(){
        std::vector<std::unique_ptr<TopLevelEntity>> ret;
        for(auto m : members){
            std::unique_ptr<TopLevelEntity> ptr(m.ptr);
            ret.push_back(std::move(ptr));
        }
        return ret;
    }

    std::vector<Method*> get_methods(){
        std::vector<Method*> ret;
        for(auto* cont : get_containers()){
            for(auto method : cont->get_methods()){
                ret.push_back(method);
            }
        }
        return ret;
    }
    std::vector<Property*> get_properties(){
        std::vector<Property*> ret;
        for(auto* cont : get_containers()){
            for(auto prop : cont->get_properties()){
                ret.push_back(prop);
            }
        }
        return ret;
    }
    std::vector<TypeMember*> get_types(){
        std::vector<TypeMember*> ret;
        for(auto cont : get_containers()){
            for(auto member : cont->members){
                if(auto type = dynamic_cast<TypeMember*>(member.ptr)){
                    ret.push_back(type);
                }
            }
        }
        return ret;
    }
    std::vector<ContainerMember*> get_hoists(){
        std::vector<ContainerMember*> ret;
        for(auto m : get_methods())ret.push_back(m);
        for(auto p : get_properties())ret.push_back(p);
        return ret;
    }


};