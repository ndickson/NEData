#pragma once

// This file defines a class for representing an optional mapping of contiguous
// integers to non-contiguous integers.  If the mapping is null, no remapping
// is done.

#include <NEData.h>

OUTER_NAMESPACE_BEGIN
NEDATA_LIBRARY_NAMESPACE_BEGIN

template<typename INT_T>
class Indirection {
	// NOTE: This pointer is not owned by this class by default.
	const INT_T* mapping;
public:
	// NOTE: The default constructor can't initialize anything if the type is
	// to remain a POD type.
	INLINE Indirection() = default;
	// NOTE: mapping_ is allowed to be nullptr, in which case the mapping does nothing.
	constexpr INLINE Indirection(const INT_T* mapping_) : mapping(mapping_) {}
	constexpr INLINE Indirection(const Indirection& that) = default;
	constexpr INLINE Indirection(Indirection&& that) = default;
	constexpr INLINE Indirection& operator=(const Indirection& that) = default;
	constexpr INLINE Indirection& operator=(Indirection&& that) = default;

	[[nodiscard]] constexpr INLINE INT_T operator[](size_t i) const {
		return (mapping == nullptr) ? i : mapping[i];
	}
};

NEDATA_LIBRARY_NAMESPACE_END
OUTER_NAMESPACE_END
