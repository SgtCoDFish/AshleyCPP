/*******************************************************************************
 * Copyright 2014 See AUTHORS file.
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

#ifndef COMPONENTTYPE_HPP_
#define COMPONENTTYPE_HPP_

#include <cstdint>

#include <typeindex>
#include <unordered_map>

namespace ashley {

/**
 * <strong>Java Description</strong>
 * Uniquely identifies a {@link Component} sub-class.
 * It assigns them an index which is used internally for fast comparison and retrieval.
 * See {@link Family} and {@link Entity}.
 *
 * <em>Java author: Stefan Bachmann</em>
 * @author Ashley Davis (SgtCoDFish)
 */
class ComponentType {
public:
	ComponentType();
	~ComponentType();

	inline uint64_t getIndex() const {
		return index;
	}

	static ComponentType& getFor(std::type_index index);
private:
	static uint64_t typeIndex;
	static std::unordered_map<std::type_index, ComponentType> componentTypes;

	uint64_t index;
};
}

#endif /* COMPONENTTYPE_HPP_ */
