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

#include <bitset>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>

#include "Ashley/AshleyConstants.hpp"

namespace ashley {
class Component;

/**
 * <p>Uniquely identifies a {@link Component} sub-class, assigning them an index which is used internally for fast comparison and retrieval.</p>
 * <p>Functions which accept a type are overloaded to accept both std::type_index (preferred) and std::type_info. This means that a {@link Component}, c, type can be passed as either std::type_index(typeid(c)) or just as typeid(c).</p>
 * <p>Note that the std::type_info form is silently converted to a std::type_index anyway.</p>
 * See {@link Family} and {@link Entity}.
 *
 * <em>Java author: Stefan Bachmann</em>
 * @author Ashley Davis (SgtCoDFish)
 */
class ComponentType {
private:
	template<unsigned int N> struct get_bits_dummy {
	};

	static get_bits_dummy<0> dummy_base;

public:
	ComponentType();
	~ComponentType() = default;
	ComponentType(const ComponentType &other) = default;
	ComponentType(ComponentType &&other) = default;
	ComponentType& operator=(const ComponentType &other) = default;
	ComponentType& operator=(ComponentType &&other) = default;

	inline uint64_t getIndex() const {
		return index;
	}

	/**
	 * @param componentType The {@link Component} class's type.
	 * @return A ComponentType matching the Component's class.
	 */
	static ComponentType& getFor(std::type_index componentType);

	/**
	 * <p>As with std::type_index version but using a templated type instead.</p>
	 */
	template<typename C> static ComponentType &getFor() {
		return ComponentType::getFor(std::type_index(typeid(C)));
	}

	/**
	 * Quick helper method. The same could be done via {@link ComponentType.getFor()}.
	 * @param componentType The {@link Component} class's type.
	 * @return The index for the specified {@link Component} Class
	 */
	static uint64_t getIndexFor(std::type_index componentType);

	/**
	 * <p>As with the std::type_index version but with a templated type instead of an argument</p>
	 */
	template<typename C> static uint64_t getIndexFor() {
		return ComponentType::getIndexFor(std::type_index(typeid(C)));
	}

	/**
	 * @param index An std::type_index.
	 * @return Bits representing the collection of components for quick comparison and matching. See {@link Family#getFor(BitsType, BitsType, BitsType)}.
	 */
	static ashley::BitsType getBitsFor(std::type_index index);

	/**
	 * Used by getBitsFor<C, CRest...>
	 */
	template<typename C> static ashley::BitsType getBitsFor(get_bits_dummy<0> dummy =
			get_bits_dummy<0>()) {
		return ashley::BitsType().set(ashley::ComponentType::getIndexFor<C>(), true);
	}

	/**
	 * <p>Derives the bits for a list of at least 2 component types passed as template arguments.</p>
	 * @return Bits representing the collection of components for quick comparison and matching. See {@link Family#getFor(BitsType, BitsType, BitsType)}.
	 */
	template<typename C, typename ... CRest> static ashley::BitsType getBitsFor(
			get_bits_dummy<sizeof...(CRest)> = get_bits_dummy<sizeof...(CRest)>()) {
				return ashley::ComponentType::getBitsFor<CRest...>(get_bits_dummy<sizeof...(CRest) -1>()).set(ashley::ComponentType::getIndexFor<C>(), true);
	}
	/**
	 * @param components Initializer list of {@link Component} class std::type_indexes.
	 * @return Bits representing the collection of components for quick comparison and matching. See {@link Family#getFor(BitsType, BitsType, BitsType)}.
	 */
	static ashley::BitsType getBitsFor(std::initializer_list<std::type_index> components);

	bool operator==(const ComponentType &other) const;
	bool operator!=(const ComponentType &other) const;

private:
	static uint64_t typeIndex;
	static std::unordered_map<std::type_index, ComponentType> componentTypes;

	uint64_t index;
};
}

#endif /* COMPONENTTYPE_HPP_ */
