#pragma once

#include "./include.h"

#include "./required/compositor_resolver.hh"
#include "./required/array_validator.hh"
#include "./required/call_qualifier.hh"
#include "./required/importer.hh"
#include "./required/type_resolver.hh"
#include "./required/property_resolver.hh"

void process_required(CompilationUnit& unit){
    resolve_imports(unit);
    resolve_compositors(unit);
    resolve_props(unit);
    bool resolvedTypes = false;
    bool resolvedCalls = false;
    while (!(resolvedTypes && resolvedCalls))
    {
      Logger::clearQueue();
      auto typeres = resolve_types(unit);
      auto res = qualify_calls(unit);
      resolvedCalls = res.first;
      resolvedTypes = typeres.first;

      // if no calls or types were resolved this iteration
      if (!res.second && !typeres.second && !(resolvedCalls && resolvedTypes))
      {
        Logger::printQueue();
        error("Type-Call Resolution Failed with " + std::to_string(res.second) + " resolved calls and " + std::to_string(typeres.second) + " resolved types");
      }
    }
      Logger::debug("5");
    Logger::clearQueue();
    Logger::enqueueMode(false);
}