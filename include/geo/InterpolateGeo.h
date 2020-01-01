#pragma once

#include "../NEData.h"
#include "../Indirection.h"
#include "../Curve.h"
#include <Types.h>
#include <Vec.h>

OUTER_NAMESPACE_BEGIN
NEDATA_LIBRARY_NAMESPACE_BEGIN

using namespace OUTER_NAMESPACE :: COMMON_LIBRARY_NAMESPACE;

template<typename INTERP_T,typename INT_T,typename ARRAY_TYPE>
constexpr INLINE auto interpolateEdge(const INTERP_T& t, INT_T i0, INT_T i1, const Indirection<INT_T>& indirection, const ARRAY_TYPE& values) {
	i0 = indirection[i0];
	i1 = indirection[i1];
	const auto& v0 = values[i0];
	const auto& v1 = values[i1];
	return interpolate(t, v0, v1);
}

// Parametric space of the triangle:
// (0,1)
//  v2......
//  | \    .
//  |  \   .
//  |   \  .
//  |    \ .
//  |     \.
//  v0-----v1
// (0,0)   (1,0)
template<typename INTERP_T,typename INT_T,typename ARRAY_TYPE>
constexpr INLINE auto interpolateTri(const Vec2<INTERP_T>& st, const INT_T begin, const Indirection<INT_T>& indirection, const ARRAY_TYPE& values) {
	const INT_T i0 = indirection[begin];
	const INT_T i1 = indirection[begin+1];
	const INT_T i2 = indirection[begin+2];
	const auto& v0 = values[i0];
	const auto& v1 = values[i1];
	const auto& v2 = values[i2];
	// NOTE: This is used instead of (1-st[0]-st[1])*v0 + st[0]*v1 + st[1]*v2
	// in order to preserve values accurately in the case where all 3 values are equal.
	return v0 + st[0]*(v1-v0) + st[1]*(v2-v0);
}

// Parametric space of the quadrilateral:
// (0,1)   (1,1)
//  v3-----v2
//  |      |
//  |      |
//  |      |
//  |      |
//  |      |
//  v0-----v1
// (0,0)   (1,0)
template<typename INTERP_T,typename INT_T,typename ARRAY_TYPE>
constexpr INLINE auto interpolateQuad(const Vec2<INTERP_T>& st, const INT_T begin, const Indirection<INT_T>& indirection, const ARRAY_TYPE& values) {
	const INT_T i0 = indirection[begin];
	const INT_T i1 = indirection[begin+1];
	const INT_T i2 = indirection[begin+2];
	const INT_T i3 = indirection[begin+3];
	const auto& v0 = values[i0];
	const auto& v1 = values[i1];
	const auto& v2 = values[i2];
	const auto& v3 = values[i3];
	// NOTE: This is used instead of a simple linear combination
	// in order to preserve values accurately in the case where all 4 values are equal.
	return interpolate(st[1], interpolate(st[0], v0, v1), interpolate(st[0], v3, v2));
}

// Parametric space of the tetrahedron:
// v0 at (0,0,0)
// v1 at (1,0,0)
// v2 at (0,1,0)
// v3 at (0,0,1)
//
// The scalar triple product:
// ((v1-v0) x (v2-v0)) . (v3-v0)
// for a tetrahedron with these coordinates will be positive,
// so this will represent an uninverted tetrahedron.
template<typename INTERP_T,typename INT_T,typename ARRAY_TYPE>
constexpr INLINE auto interpolateTet(const Vec3<INTERP_T>& st, const INT_T begin, const Indirection<INT_T>& indirection, const ARRAY_TYPE& values) {
	const INT_T i0 = indirection[begin];
	const INT_T i1 = indirection[begin+1];
	const INT_T i2 = indirection[begin+2];
	const INT_T i3 = indirection[begin+3];
	const auto& v0 = values[i0];
	const auto& v1 = values[i1];
	const auto& v2 = values[i2];
	const auto& v3 = values[i3];
	// NOTE: This is used instead of (1-st[0]-st[1]-st[2])*v0 + st[0]*v1 + st[1]*v2 + st[2]*v3
	// in order to preserve values accurately in the case where all 4 values are equal.
	return v0 + st[0]*(v1-v0) + st[1]*(v2-v0) + st[2]*(v3-v0);
}

NEDATA_LIBRARY_NAMESPACE_END
OUTER_NAMESPACE_END
