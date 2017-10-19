/*******************************************************************************
 * Copyright 2017 See AUTHORS file.
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

#include <iterator>

#include "Ashley/core/Engine.hpp"
#include "Ashley/systems/SortedIteratingSystem.hpp"

namespace ashley {

void SortedIteratingSystem::addedToEngine(Engine &engine) {
	IteratingSystem::addedToEngine(engine);
	sortedEntities.clear();

	if (entities->size() > 0) {
		std::copy(entities->begin(), entities->end(), sortedEntities.begin());
		forceSort();
		sort();
	}

	engine.addEntityListener(this);
}

void SortedIteratingSystem::removedFromEngine(Engine &engine) {
	engine.removeEntityListener(this);
	sortedEntities.clear();
	shouldSort = false;
}

void SortedIteratingSystem::entityAdded(Entity &entity) {
	sortedEntities.emplace_back(&entity);
	shouldSort = true;
}

void SortedIteratingSystem::entityRemoved(Entity &entity) {
	sortedEntities.erase(std::remove(sortedEntities.begin(), sortedEntities.end(), &entity), sortedEntities.end());
}

void SortedIteratingSystem::update(float deltaTime) {
	sort();
	std::for_each(sortedEntities.begin(), sortedEntities.end(),
	    [&](ashley::Entity *const &found) {processEntity(found, deltaTime);});
}

void SortedIteratingSystem::sort() {
	if (shouldSort) {
		std::sort(sortedEntities.begin(), sortedEntities.end(), comparator);
		shouldSort = false;
	}
}

}
