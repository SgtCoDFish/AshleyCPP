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

#include <cassert>
#include <vector>
#include <algorithm>

#include "Ashley/core/Engine.hpp"
#include "Ashley/core/Entity.hpp"
#include "Ashley/core/EntitySystem.hpp"
#include "Ashley/core/Family.hpp"
#include "Ashley/systems/IteratingSystem.hpp"

void ashley::IteratingSystem::addedToEngine(ashley::Engine &engine) {
	entities = engine.getEntitiesFor(family);
}

void ashley::IteratingSystem::removedFromEngine(ashley::Engine &engine) {
	entities.clear();
}

void ashley::IteratingSystem::update(float deltaTime) {
	std::for_each(entities.begin(), entities.end(),
			[&](std::shared_ptr<ashley::Entity> found) {processEntity(*found, deltaTime);});
}
