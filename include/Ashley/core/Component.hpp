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

#ifndef ACPP_CORE_COMPONENT_HPP_
#define ACPP_CORE_COMPONENT_HPP_

#include <typeinfo>
#include <typeindex>

#include "Ashley/AshleyConstants.hpp"

namespace ashley {

/**
 * Base class for all Components. A Component is intended as a data holder and provides data to be processed
 * in an {@link EntitySystem}.
 *
 * <em>Java author: Stefan Bachmann</em>
 * @author Ashley Davis (SgtCoDFish)
 */
class Component {
public:
	explicit Component() {
	}

	~Component() = default;
	Component(const Component &other) = default;
	Component(Component &&other) = default;
	Component& operator=(const Component &other) = default;
	Component& operator=(Component &&other) = default;
};
}

#endif /* COMPONENT_HPP_ */
