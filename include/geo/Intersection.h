#pragma once

#include "../NEData.h"
#include "../Indirection.h"
#include <Types.h>
#include <Vec.h>

OUTER_NAMESPACE_BEGIN
NEDATA_LIBRARY_NAMESPACE_BEGIN

using namespace OUTER_NAMESPACE :: COMMON_LIBRARY_NAMESPACE;

template<typename FLOAT_T,typename INT_T,typename ARRAY_TYPE,typename RESULT_T>
constexpr inline bool intersectTri(
	const Vec2<FLOAT_T>& rayOrigin2D,
	const Vec3<FLOAT_T>& rayX,
	const Vec3<FLOAT_T>& rayY,
	const INT_T begin, const Indirection<INT_T>& indirection, const ARRAY_TYPE& values,
	Vec2<RESULT_T>& hitST
) {
	// The faster-but-not-robust approach to ray intersection would be
	// to find the ray-plane intersection, then check the barycentric coordinates
	// of the intersection point and check if they're inside the triangle.
	// However, an approach that reduces double-hits or double-misses near edges
	// is to project the triangle into the 2D space perpendicular to the ray,
	// and check whether the 2D ray origin is inside the triangle or not,
	// by checking all 3 signed areas of the triangles between an edge and the origin.
	// If all 3 areas are positive, it's a hit on one side of the triangle;
	// if all 3 areas are negative, it's a hit on the other side of the triangle.
	// The barycentric coordinates remain the same (under exact arithmetic), 
	// so the coordinates can be computed from the signed triangle areas.

	const INT_T i0 = indirection[begin];
	const INT_T i1 = indirection[begin+1];
	const INT_T i2 = indirection[begin+2];
	const auto p0 = values[i0];
	const auto p1 = values[i1];
	const auto p2 = values[i2];
	const Vec2<decltype(rayX.dot(p0))> xy0(rayX.dot(p0), rayY.dot(p0));
	const Vec2<decltype(rayX.dot(p1))> xy1(rayX.dot(p1), rayY.dot(p1));
	const Vec2<decltype(rayX.dot(p2))> xy2(rayX.dot(p2), rayY.dot(p2));
	const auto area0 = (xy2 - xy1).cross(rayOrigin2D - xy1);
	const auto area1 = (xy0 - xy2).cross(rayOrigin2D - xy2);
	const auto area2 = (xy1 - xy0).cross(rayOrigin2D - xy0);
	const bool in0 =  (area0 > 0);
	const bool out0 = (area0 < 0);
	const bool in1 =  (area1 > 0);
	const bool out1 = (area1 < 0);
	const bool in2 =  (area2 > 0);
	const bool out2 = (area2 < 0);

	bool hit = false;
	if ((in0 || out0) && (in1 || out1) && (in2 || out2)) {
		// Common case: either in or out on all sides
		if (in0 && in1 && in2) {
			hit = true;
		}
		else if (out0 && out1 && out2) {
			// Backface hit
			// TODO: Optionally exclude backface hits!
			hit = true;
		}
	}
	else {
		// Uncommon case: at least one zero or NaN value
		const bool zero0 = (area0 == 0);
		const bool zero1 = (area1 == 0);
		const bool zero2 = (area2 == 0);
		if (zero0) {
			if ((in1 || out1) && (in2 || out2)) {
				// Hit on edge 1->2: only count if index 1 is less than index 2,
				// to avoid double-hits.
				// TODO: Optionally exclude backface hits by only accepting in1 && in2!
				hit = (i1 < i2) && (in1 == in2);
			}
			else {
				// Corner hit or degenerate triangle: accept all hits
				// if nondegenerate, even though there may be multiple-hits.
				hit = (zero1 && (in2 || out2)) || (zero2 && (in1 || out1));
			}
		}
		else if (zero1) {
			if ((in2 || out2) && (in0 || out0)) {
				// Hit on edge 2->0: only count if index 2 is less than index 0,
				// to avoid double-hits.
				// TODO: Optionally exclude backface hits by only accepting in2 && in0!
				hit = (i2 < i0) && (in2 == in0);
			}
			else {
				// Corner hit or degenerate triangle: accept all hits
				// if nondegenerate, even though there may be multiple-hits.
				// NOTE: zero0 is false here.
				hit = (zero2 && (in0 || out0));
			}
		}
		else if (zero2) {
			if ((in0 || out0) && (in1 || out1)) {
				// Hit on edge 0->1: only count if index 0 is less than index 1,
				// to avoid double-hits.
				// TODO: Optionally exclude backface hits by only accepting in0 && in1!
				hit = (i0 < i1) && (in0 == in1);
			}
			// NOTE: No else needed, since zero0 and zero1 are both false here.
		}
	}

	if (hit) {
		// The parametric coordinates of the hit inside the triangle are
		// just the barycentric coordinates 1 and 2.
		const auto sum = area0 + area1 + area2;
		hitST = Vec2<RESULT_T>(area1/sum, area2/sum);
	}

	return hit;
}

NEDATA_LIBRARY_NAMESPACE_END
OUTER_NAMESPACE_END
