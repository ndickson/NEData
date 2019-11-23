#pragma once

// This file declares common types and macros for use in other files.

#include <Types.h>

#define NEDATA_LIBRARY_NAMESPACE        NEData
#define NEDATA_LIBRARY_NAMESPACE_BEGIN  namespace NEDATA_LIBRARY_NAMESPACE {
#define NEDATA_LIBRARY_NAMESPACE_END    }

#if BUILDING_NEDATA_LIBRARY && HAVE_VISIBILITY
#define NEDATA_LIBRARY_EXPORTED __attribute__((__visibility__("default")))
#elif BUILDING_NEDATA_LIBRARY && defined(_MSC_VER)
#define NEDATA_LIBRARY_EXPORTED __declspec(dllexport)
#elif defined(_MSC_VER)
#define NEDATA_LIBRARY_EXPORTED __declspec(dllimport)
#else
#define NEDATA_LIBRARY_EXPORTED
#endif

OUTER_NAMESPACE_BEGIN
NEDATA_LIBRARY_NAMESPACE_BEGIN

template<typename INT_T>
class Spans;

template<typename INT_T>
class Indirection;

NEDATA_LIBRARY_NAMESPACE_END
OUTER_NAMESPACE_END
