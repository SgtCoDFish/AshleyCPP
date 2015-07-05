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

#ifndef INCLUDE_ASHLEY_INTERNAL_HELPER_HPP_
#define INCLUDE_ASHLEY_INTERNAL_HELPER_HPP_

#include <typeinfo>
#include <typeindex>
#include <type_traits>

#include "Ashley/core/Component.hpp"

namespace ashley {
namespace internal {

template<typename C> const char *get_friendly_type_name() {
	return std::type_index(typeid(C)).name();
}

template<typename C> inline void verify_component_type() {
	static_assert(std::is_base_of<ashley::Component, C>(), "Component verification failed.");
}

}
}

#endif /* INCLUDE_ASHLEY_INTERNAL_HELPER_HPP_ */
