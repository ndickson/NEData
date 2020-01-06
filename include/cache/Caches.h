#pragma once

#include "../NEData.h"

#include <SharedArray.h>
#include <SharedString.h>

OUTER_NAMESPACE_BEGIN
NEDATA_LIBRARY_NAMESPACE_BEGIN

using namespace COMMON_LIBRARY_NAMESPACE;

using ID = uint64;
using IDArray = SharedArray<ID>;

constexpr static ID INVALID_ID = ~uint64(0);

struct CacheItemPriority {
	uint64 memoryUsed;
	// The last accessed timestamp needs full precision, because it will be
	// subtracted from the current timestamp.
	uint64 lastAccessedTimestamp;
	float cyclesToBuildPerByte;

	constexpr bool isLowerPriorityThan(const CacheItemPriority& that, const uint64 currentTimestamp) const {
		const uint64 thisCyclesSinceAccess = currentTimestamp - lastAccessedTimestamp;
		const uint64 thatCyclesSinceAccess = currentTimestamp - that.lastAccessedTimestamp;
		// Longer to (re)build increases priority to keep an item in the cache.
		// More memory decreases priority to keep an item in the cache.
		// Longer time since last access decreases priority to keep an item in the cache.
		return cyclesToBuildPerByte*float(thatCyclesSinceAccess) < that.cyclesToBuildPerByte*float(thisCyclesSinceAccess);
	}
};

// Data cache
// ID -> data

ID cacheData(CacheItem* item);
CacheItem* lookupCacheItem(ID id);

// Single value caches
// value -> ID
// (int64, double, and possibly text)

ID cacheInteger(int64 value);
int64 lookupCacheInteger(ID id);

ID cacheDouble(double value);
double lookupCacheDouble(ID id);

ID cacheString(const SharedString& value);
ID cacheString(SharedString&& value);
const SharedString& lookupCacheString(ID id);

// Function cache
// function ID -> function data necessary for execution

ID addFunction(FunctionData& function);

// Function output cache
// function ID & input data IDs -> output data IDs

// Queues a function to be executed asynchronously, if it hasn't already
// been run and completed, returning immediately in either case.
//
// This returns true if the function was already executed and is still cached,
// in which case, outputOrTaskID is the ID for the output data.
// This returns false if the function is queued to execute or is currently executing,
// in which case, outputOrTaskID is the ID for the task.
bool runFunction(ID function, const IDArray& inputs, ID& outputOrTaskID);

// Call this and return if runFunction returns false.
// This creates a new task that is unqueued, and returns the task's ID.
// Call waitForFunction with the returned ID and all task IDs to wait for
// before returning.
ID deferFunction(ID function, FunctionState& state);

// Call this after calling deferFunction, in order to ensure that deferredTask
// is resumed when taskToWaitFor completes.
void waitForFunction(ID deferredTask, ID taskToWaitFor);

// If runFunction returns false, this function can be called with the task ID
// to retrieve the output data's ID.  Returns INVALID_ID if the task isn't complete.
//
// NOTE: If this returns a value other than INVALID_ID, *DO* *NOT* call it again!
// This is because this function decrements the count of callers waiting on the task output
// if the task is done.
ID retrieveTaskOutput(ID task);


NEDATA_LIBRARY_NAMESPACE_END
OUTER_NAMESPACE_END
