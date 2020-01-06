#pragma once

#include "../NEData.h"
#include "../Indirection.h"
#include <Types.h>

OUTER_NAMESPACE_BEGIN
NEDATA_LIBRARY_NAMESPACE_BEGIN

using namespace OUTER_NAMESPACE :: COMMON_LIBRARY_NAMESPACE;

// Fills in the indirection array (if non-null) with the indirection representing
// positions for a quad grid with optional row or column wrapping.
//
// NOTE: For the indirection array for texture UV coordinates,
// just specify wrapRows and wrapCols both false.
template<typename INT_T>
constexpr inline size_t createGridIndirection(const size_t numVtxRows, const bool wrapRows, const size_t numVtxCols, const bool wrapCols, INT_T* indirection) {
	if (indirection != nullptr) {
		INT_T i = 0;
		for (size_t row = 0; row < numVtxRows-1; ++row) {
			for (size_t col = 0; col < numVtxCols-1; ++col, ++i) {
				indirection[0] = i;
				indirection[1] = i+1;
				indirection[2] = i+numVtxCols+1;
				indirection[3] = i+numVtxCols;
				indirection += 4;
			}
			if (wrapCols) {
				indirection[0] = i;
				indirection[1] = i+1-numVtxCols;
				indirection[2] = i+1;
				indirection[3] = i+numVtxCols;
				indirection += 4;
			}
			++i;
		}
		if (wrapRows) {
			for (size_t col = 0; col < numVtxCols-1; ++col, ++i) {
				indirection[0] = i;
				indirection[1] = i+1;
				indirection[2] = col+1;
				indirection[3] = col;
				indirection += 4;
			}
			if (wrapCols) {
				indirection[0] = i;
				indirection[1] = i+1-numVtxCols;
				indirection[2] = 0;
				indirection[3] = numVtxCols-1;
			}
		}
	}
	return numVtxCols*numVtxRows;
}

NEDATA_LIBRARY_NAMESPACE_END
OUTER_NAMESPACE_END
