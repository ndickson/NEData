#pragma once

#include "../NEData.h"
#include "../Spans.h"
#include "../Indirection.h"
#include <Types.h>
#include <Vec.h>

OUTER_NAMESPACE_BEGIN
NEDATA_LIBRARY_NAMESPACE_BEGIN

using namespace OUTER_NAMESPACE :: COMMON_LIBRARY_NAMESPACE;

// This computes a vector whose length is *twice* the maximum area of the polygon
// projected into any plane, and whose direction is the normal of that plane
// in which the polygon goes around counterclockwise, i.e. a right-handed normal.
template<typename INT_T,typename ARRAY_TYPE,typename SUM_T>
constexpr INLINE void polyAreaNormalx2(const Span<INT_T>& span, const Indirection<INT_T>& indirection, const ARRAY_TYPE& positions, Vec3<SUM_T>& normal) {
	const INT_T begin = span[0];
	const INT_T end = span[1];
	const INT_T n = end-begin;
	if (n >= 3) {
		const INT_T i0 = indirection[begin];
		const INT_T i1 = indirection[begin+1];
		const INT_T i2 = indirection[begin+2];
		const Vec3<SUM_T> p0(positions[i0]);
		Vec3<SUM_T> p1(positions[i1]);
		Vec3<SUM_T> p2(positions[i2]);
		if (n == 3) {
			// Single triangle
			normal = (p1 - p0).cross(p2 - p0);
		}
		else {
			const INT_T i3 = indirection[begin+3];
			Vec3<SUM_T> p3(positions[i3]);
			// First quad (possibly only)
			normal = (p2 - p0).cross(p3 - p1);
			if  (n > 4) {
				for (INT_T i = 4; i < n-1; i += 2) {
					// Add another quad at a time
					p1 = p3;
					p2 = Vec3<SUM_T>(positions[indirection[begin+i]]);
					p3 = Vec3<SUM_T>(positions[indirection[begin+i+1]]);
					normal += (p2 - p0).cross(p3 - p1);
				}
				if (n & 1) {
					// Add the final triangle
					p1 = Vec3<SUM_T>(positions[indirection[end-1]]);
					normal += (p3 - p0).cross(p1 - p0);
				}
			}
		}
	}
	else {
		// Not enough vertices, so fall back to zero.
		normal = Vec3<SUM_T>(SUM_T(0));
	}
}

// This computes *twice* the signed area of the 2D polygon, which is positive if
// the polygon goes around counterclockwise, else negative.
template<typename INT_T,typename ARRAY_TYPE,typename SUM_T>
constexpr INLINE void poly2DAreax2(const Span<INT_T>& span, const Indirection<INT_T>& indirection, const ARRAY_TYPE& positions, SUM_T& areax2) {
	const INT_T begin = span[0];
	const INT_T end = span[1];
	const INT_T n = end-begin;
	if (n >= 3) {
		const INT_T i0 = indirection[begin];
		const INT_T i1 = indirection[begin+1];
		const INT_T i2 = indirection[begin+2];
		const Vec2<SUM_T> p0(positions[i0]);
		Vec2<SUM_T> p1(positions[i1]);
		Vec2<SUM_T> p2(positions[i2]);
		if (n == 3) {
			// Single triangle
			areax2 = (p1 - p0).cross(p2 - p0);
		}
		else {
			const INT_T i3 = indirection[begin+3];
			Vec2<SUM_T> p3(positions[i3]);
			// First quad (possibly only)
			areax2 = (p2 - p0).cross(p3 - p1);
			if  (n > 4) {
				for (INT_T i = 4; i < n-1; i += 2) {
					// Add another quad at a time
					p1 = p3;
					p2 = Vec2<SUM_T>(positions[indirection[begin+i]]);
					p3 = Vec2<SUM_T>(positions[indirection[begin+i+1]]);
					areax2 += (p2 - p0).cross(p3 - p1);
				}
				if (n & 1) {
					// Add the final triangle
					p1 = Vec2<SUM_T>(positions[indirection[end-1]]);
					areax2 += (p3 - p0).cross(p1 - p0);
				}
			}
		}
	}
	else {
		// Not enough vertices, so fall back to zero.
		areax2 = SUM_T(0);
	}
}

NEDATA_LIBRARY_NAMESPACE_END
OUTER_NAMESPACE_END
