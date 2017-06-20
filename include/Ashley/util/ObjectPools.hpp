/*******************************************************************************
 * Copyright 2014, 2015 See AUTHORS file.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/

#ifndef ACPP_UTIL_OBJECTPOOLS_HPP_
#define ACPP_UTIL_OBJECTPOOLS_HPP_

#include <cassert>

#include <stack>
#include <utility>

namespace ashley {

/**
 * <p>Interface implemented by classes wishing to be poolable; i.e. have an ObjectPool manage a pool
 * of instances of the class to avoid dynamic allocation.</p>
 */
class Poolable {
public:
	virtual ~Poolable() {
	}

	/**
	 * <p>Called when an object is "free"ed by the pool, to reset all the arguments to their default state.
	 */
	virtual void reset() = 0;
};

/**
 * <p>Implements an object pool which allocates a number of objects using new and stores them for retrieval using
 * obtain.</p>
 */
template<typename T> class ObjectPool {
private:
	std::stack<T*> pool;

	int64_t peakEntities;

	void createObject() {
		peakEntities++;
		pool.emplace(new T());
	}

public:
	explicit ObjectPool(int64_t startEntities = 100) :
			        peakEntities(0) {
		assert(startEntities >= 1 && "startEntities must be >= 1");

		for (int i = 0; i < startEntities; i++) {
			createObject();
		}
	}

	virtual ~ObjectPool() {
		while (pool.size() > 0) {
			T *obj = pool.top();
			pool.pop();

			if (obj != nullptr) {
				delete obj;
			}
		}
	}

	/**
	 * <p>Returns a pre-allocated object from the stack. If the stack is empty, creates and returns a new object.</p>
	 * <p>Note that the returned object needs to be deleted; either call {@link ObjectPool#free} with the object to
	 * free it for later use, or delete it yourself under the knowledge that the stack will permanently be one object down.
	 * Best practise is to use free().</p>
	 * @return a pointer to an allocated object, will not be nullptr.
	 */
	T *obtain() {
		if (pool.size() == 0) {
			createObject();
		}

		T *obj = pool.top();
		pool.pop();
		return obj;
	}

	/**
	 * <p>Resets the object and returns it to the stack to be used again.</p>
	 * @param object the object, obtained from {@link ObjectPool#obtain} to be freed. Don't use it after passing it to this function.
	 */
	void free(T *object) {
		object->reset();

		pool.push(object);
	}

	/**
	 * @return the highest number of allocations ever required in this {@link ObjectPool}'s lifetime.
	 * Useful for optimising the starting value passed to the constructor.
	 */
	inline int64_t getPeakEntities() {
		return peakEntities;
	}
};

}

#endif /* OBJECTPOOLS_HPP_ */
