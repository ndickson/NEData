#pragma once

// This file defines a class for representing a sequence of contiguous spans,
// storing either an array of span starts (plus the end), or just the span
// size if all spans are the same size.

#include <NEData.h>
#include <Span.h>
#include <stdint.h>

OUTER_NAMESPACE_BEGIN
NEDATA_LIBRARY_NAMESPACE_BEGIN

template<typename INT_T>
class Spans {
	union UnionType {
		INLINE UnionType() = default;
		constexpr INLINE UnionType(INT_T spanSize_) : value((spanSize_<<1) | 1) {}
		constexpr INLINE UnionType(const INT_T* spanStarts) : pointer(spanStarts) {}

		// This is explicitly the same size as the pointer, to avoid issues with
		// uninitialized data.
		uintptr_t value;

		// NOTE: If !isUniform(), this is a pointer to an array of numSpans+1
		// integers in monotone increasing order (possibly not strictly increasing
		// if there are any zero size spans), where the last value indicates the
		// end of the last span.  The first value is usually zero, but may be
		// non-zero if this contiguous sequence of spans doesn't start at zero.
		//
		// NOTE: The pointer must be to an even memory address, which is the case
		// for all common allocators, as long as sizeof(INT_T) is not odd.
		//
		// NOTE: This pointer is not owned by this class by default.
		const INT_T* pointer;
	} u;
	size_t numSpans;

public:

	// NOTE: The default constructor can't initialize anything if the type is
	// to remain a POD type.
	INLINE Spans() = default;
	constexpr INLINE Spans(INT_T spanSize_, size_t numSpans_) : u(spanSize_), numSpans(numSpans_) {}
	constexpr INLINE Spans(const INT_T* spanStarts, size_t numSpans_) : u(spanStarts), numSpans(numSpans_) {}
	constexpr INLINE Spans(const Spans& that) = default;
	constexpr INLINE Spans(Spans&& that) = default;
	constexpr INLINE Spans& operator=(const Spans& that) = default;
	constexpr INLINE Spans& operator=(Spans&& that) = default;

	using Span = OUTER_NAMESPACE :: COMMON_LIBRARY_NAMESPACE :: Span<INT_T>;

	[[nodiscard]] constexpr INLINE bool isUniform() const {
		return u.value & 1;
	}
	[[nodiscard]] constexpr INLINE INT_T uniformSpanSize() const {
		return INT_T(u.value >> 1);
	}
	[[nodiscard]] constexpr INLINE INT_T uniformSpanStart(size_t i) const {
		return uniformSpanSize() * i;
	}

	[[nodiscard]] constexpr INLINE const INT_T& nonuniformSpanStart(size_t i) const {
		return u.pointer[i];
	}
	[[nodiscard]] constexpr INLINE INT_T spanStart(size_t i) const {
		return isUniform() ? uniformSpanStart(i) : nonuniformSpanStart(i);
	}
	[[nodiscard]] constexpr INLINE INT_T spanSize(size_t i) const {
		if (isUniform()) {
			return uniformSpanSize();
		}
		return nonuniformSpanStart(i+1) - nonuniformSpanStart(i);
	}
	[[nodiscard]] constexpr INLINE INT_T spanEnd(size_t i) const {
		++i;
		return isUniform() ? uniformSpanStart(i) : nonuniformSpanStart(i);
	}
	[[nodiscard]] constexpr INLINE Span span(size_t i) const {
		if (isUniform()) {
			const INT_T size = uniformSpanSize();
			const INT_T start = size*i;
			return Span(start, start+size);
		}
		return Span(nonuniformSpanStart(i), nonuniformSpanStart(i+1));
	}
	[[nodiscard]] constexpr INLINE size_t size() const {
		return numSpans;
	}
};

NEDATA_LIBRARY_NAMESPACE_END
OUTER_NAMESPACE_END
