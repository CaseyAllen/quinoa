#pragma once

#include "../include.h"


std::pair<bool, int> qualify_calls(Method &code,
                  CompilationUnit* unit) {
  auto flat = code.flatten();
  int resolvedCount = 0;
  bool success = true;
  for (auto item : flat) {
    if (instanceof<MethodCall>(item)) {
      auto call = (MethodCall *)item;
      // Don't do redundant qualification
      if(call->target )continue;
      if(call->builtin())continue;
      Logger::debug("Qualify call to " + call->name->str());
      call->qualify(unit, *code.local_types);
      if(call->target ){
        auto method = call->target->belongsTo;
        if(!method->public_access){
          auto method_mod = method->memberOf;
          auto my_mod = code.memberOf;
          if(method_mod != my_mod)
          error("Cannot call private method " + method->sig->name->str() + " from " + code.sig->name->str());
        }
        resolvedCount++;
      }
      else success = false;
    }
  }
  return {success, resolvedCount};
}

std::map<std::string, MethodSignature*> fetch_signatures(CompilationUnit unit){
    // Construct a table of all call names -> their signatures
  std::map<std::string, MethodSignature *> sigs;

  // All Implemented Functions
  for (auto method : unit.getAllMethods()) {
    auto name = method->sig->sourcename();
    sigs[name] = method->sig;
  }

  return sigs;
}
std::pair<bool, int> qualify_calls(CompilationUnit &unit) {

  auto sigs = fetch_signatures(unit);
  int count = 0;
  bool success = true;
  // Attempt to Qualify all Calls
  for (auto method : unit.getAllMethods()) {
      auto result = qualify_calls(*method, sigs);
      if(!result.first)success=false;
      count+=result.second;
  }
  return {success, count};
}