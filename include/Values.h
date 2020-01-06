#pragma once

#include "NEData.h"
#include <atomic>

OUTER_NAMESPACE_BEGIN
NEDATA_LIBRARY_NAMESPACE_BEGIN

using namespace OUTER_NAMESPACE :: COMMON_LIBRARY_NAMESPACE;

template<typename T,size_t PAGE_BITS,bool UNIFORM_ALLOWED,bool SHARING_ALLOWED>
class PagedValues {
	struct PageTableEntry {
		T* data;
		std::atomic<uint64>* refCount;
	};
	struct EmbeddedUniform {
		constexpr static bool isValid = (sizeof(PageTableEntry)-sizeof(T)) >= alignof(T);
		char padding[isValid ? (sizeof(PageTableEntry)-sizeof(T))/alignof(T)*alignof(T) : 1];

		T uniformValue;
	};

	PageTableEntry* pageTable;
	size_t size;
public:
	constexpr static size_t PAGE_SIZE = (size_t(1)<<PAGE_BITS);
	constexpr static size_t PAGE_INDEX_MASK = PAGE_SIZE-1;

	INLINE PagedValues() : pageTable(nullptr), size(0) {}

	~PagedValues() {
		// Round up to include all pages.
		const size_t numPages = ((size + (PAGE_SIZE-1))>>PAGE_BITS);
		for (size_t pagei = 0; pagei < numFullPages; ++pagei) {
			PageTableEntry*const pageTableEntry = pageTable + pagei;
			T* data = pageTableEntry->data;
			if (UNIFORM_ALLOWED && (uintptr_t(data) & 1)) {
				if constexpr (EmbeddedUniform::isValid) {
					if (!std::is_trivially_destructible<T>::value) {
						// Destruct the one element
						reinterpret_cast<const EmbeddedUniform*>(pageTableEntry)->uniformValue.~T();
					}
				}
				else if constexpr (SHARING_ALLOWED) {
					// Decrement the reference count.
					std::atomic<uint64>*const refCount = pageTableEntry->refCount;
					const uint64 newRefCount = --(*refCount);
					if (newRefCount == 0) {
						// Delete the reference count.
						delete refCount;
						// Delete the one element.
						T*const data = (T*)(uintptr_t(data) & ~uintptr_t(1));
						delete data;
					}
				}
				else {
					// Delete the one element.
					data = (T*)(uintptr_t(data) & ~uintptr_t(1));
					delete data;
				}
			}
			else if constexpr (SHARING_ALLOWED) {
				// Decrement the reference count.
				std::atomic<uint64>*const refCount = pageTableEntry->refCount;
				const uint64 newRefCount = --(*refCount);
				if (newRefCount == 0) {
					// Delete the reference count.
					delete refCount;
					// Delete the page of elements.
					delete [] data;
				}
			}
			else {
				// Delete the page of elements.
				delete [] data;
			}
		}
	}

	INLINE const T& operator[](size_t i) const {
		const size_t pagei = (i >> PAGE_BITS);
		const PageTableEntry*const pageTableEntry = pageTable + pagei;
		if (UNIFORM_ALLOWED && (uintptr_t(pageTableEntry->data) & 1)) {
			if constexpr (EmbeddedUniform::isValid) {
				return reinterpret_cast<const EmbeddedUniform*>(pageTableEntry)->uniformValue;
			}
			else {
				return *(const T*)(uintptr_t(pageTableEntry->data) & ~uintptr_t(1))
			}
		}
		return pageTableEntry->data[i & PAGE_INDEX_MASK];
	}
};

NEDATA_LIBRARY_NAMESPACE_END
OUTER_NAMESPACE_END
