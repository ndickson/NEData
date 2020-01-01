#pragma once

#include "NEData.h"

OUTER_NAMESPACE_BEGIN
NEDATA_LIBRARY_NAMESPACE_BEGIN

// Linear interpolation between v0 and v1 as t goes from 0 to 1.
//
// Ideally, this would guarantee that results are monotonic with respect to t between v0 and v1,
// as t goes 0-->1, but that turns out to be much more complicated than it sounds, so as a
// compromise, this at least guarantees for t in [0,1], the return value is:
// * monotone with respect to t
// * exactly v0 when t == 0
// * exactly v0 if v0 == v1
// It may not be exactly v1 when t == 1, though frequently will be, especially if v0 and v1 are
// close to each other in magnitude and equal in sign.
template<typename VALUE_T,typename INTERP_T>
constexpr INLINE VALUE_T interpolate(const INTERP_T& t, const VALUE_T& v0, const VALUE_T& v1) {
	return v0 + t*(v1-v0);
}

// For evaluating the first segment of a regular subdivision curve sequence v0, v1, v2, ...:
template<typename VALUE_T,typename INTERP_T>
constexpr INLINE VALUE_T subdCurveFirstSegment(const INTERP_T& t, const VALUE_T& v0, const VALUE_T& v1, const VALUE_T& v2) {
	const INTERP_T t3 = t*t*t;
	constexpr INTERP_T sixth = INTERP_T(1)/INTERP_T(6);
	const VALUE_T diff01 = (v1-v0);
	const VALUE_T sumdiff1 = ((v2-v1) - diff01);
	return (v0 + t*diff01) + (sixth*t3)*sumdiff1;
}

// For evaluating a beginning segment of a subdivision curve network, where the node at t==1
// may be connected to more than 2 nodes, but the node at t==0 is only connected to the
// single node.
// v1diff is the average difference from neighbours of v1 to v1, (including v0).
template<typename VALUE_T,typename INTERP_T>
constexpr INLINE VALUE_T subdCurveFirstSegmentMulti(const INTERP_T& t, const VALUE_T& v0, const VALUE_T& v1, const VALUE_T& v1diff) {
	const INTERP_T t3 = t*t*t;
	constexpr INTERP_T third = INTERP_T(1)/INTERP_T(3);
	return interpolate(t, v0, v1) + (third*t3)*v1diff;
}

// For evaluating a regular subdivision curve sequence ..., vn1, v0, v1, v2, ...:
template<typename VALUE_T,typename INTERP_T>
constexpr INLINE VALUE_T subdCurveMiddleSegment(const INTERP_T& t, const VALUE_T& vn1, const VALUE_T& v0, const VALUE_T& v1, const VALUE_T& v2) {
	const INTERP_T ti = INTERP_T(1) - t;
	const INTERP_T ti3 = ti*ti*ti;
	const INTERP_T t3 = t*t*t;
	constexpr INTERP_T sixth = INTERP_T(1)/INTERP_T(6);
	const VALUE_T diff01 = (v1-v0);
	const VALUE_T sumdiff0 = ((vn1-v0) + diff01);
	const VALUE_T sumdiff1 = ((v2-v1) - diff01);
	return (v0 + t*diff01) + ((sixth*ti3)*sumdiff0 + (sixth*t3)*sumdiff1);
}

// For evaluating a segment of a subdivision curve network, where both
// the node at t==0 and the node at t==1 may be connected to more than 2 nodes.
// v0diff is the average difference from neighbours of v0 to v0, (including v1).
// v1diff is the average difference from neighbours of v1 to v1, (including v0).
template<typename VALUE_T,typename INTERP_T>
constexpr INLINE VALUE_T subdCurveMiddleSegmentMulti(const INTERP_T& t, const VALUE_T& v0diff, const VALUE_T& v0, const VALUE_T& v1, const VALUE_T& v1diff) {
	const INTERP_T ti = INTERP_T(1) - t;
	const INTERP_T ti3 = ti*ti*ti;
	const INTERP_T t3 = t*t*t;
	constexpr INTERP_T third = INTERP_T(1)/INTERP_T(3);
	return interpolate(t, v0, v1) + ((third*ti3)*v0diff + (third*t3)*v1diff);
}

// For evaluating a regular subdivision curve sequence ..., vn1, v0, v1:
template<typename VALUE_T,typename INTERP_T>
constexpr INLINE VALUE_T subdCurveLastSegment(const INTERP_T& t, const VALUE_T& vn1, const VALUE_T& v0, const VALUE_T& v1) {
	const INTERP_T ti = INTERP_T(1) - t;
	const INTERP_T ti3 = ti*ti*ti;
	constexpr INTERP_T sixth = INTERP_T(1)/INTERP_T(6);
	const VALUE_T diff01 = (v1-v0);
	const VALUE_T sumdiff0 = ((vn1-v0) + diff01);
	return (v0 + t*diff01) + (sixth*ti3)*sumdiff0;
}

// For evaluating an ending segment of a subdivision curve network, where the node at t==0
// may be connected to more than 2 nodes, but the node at t==1 is only connected to the
// single node.
// v0diff is the average difference from neighbours of v0 to v0, (including v1).
template<typename VALUE_T,typename INTERP_T>
constexpr INLINE VALUE_T subdCurveLastSegmentMulti(const INTERP_T& t, const VALUE_T& v0diff, const VALUE_T& v0, const VALUE_T& v1) {
	const INTERP_T ti = INTERP_T(1) - t;
	const INTERP_T ti3 = ti*ti*ti;
	constexpr INTERP_T third = INTERP_T(1)/INTERP_T(3);
	return interpolate(t, v0, v1) + (third*ti3)*v0diff;
}

NEDATA_LIBRARY_NAMESPACE_END
OUTER_NAMESPACE_END
