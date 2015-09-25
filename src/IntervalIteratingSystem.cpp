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

#include "Ashley/core/Family.hpp"
#include "Ashley/core/Engine.hpp"
#include "Ashley/systems/IntervalIteratingSystem.hpp"

ashley::IntervalIteratingSystem::IntervalIteratingSystem(Family *family, float interval) :
		        IntervalIteratingSystem(family, interval, DEFAULT_PRIORITY) {
}

ashley::IntervalIteratingSystem::IntervalIteratingSystem(Family *family, float interval, uint64_t priority) :
		        IntervalSystem(interval, priority),
		        family(family) {
}

void ashley::IntervalIteratingSystem::addedToEngine(ashley::Engine &engine) {
	entities = engine.getEntitiesFor(family);
}

void ashley::IntervalIteratingSystem::removedFromEngine(ashley::Engine &engine) {
	entities = nullptr;
}

void ashley::IntervalIteratingSystem::updateInterval() {
	for (unsigned int i = 0; i < entities->size(); ++i) {
		processEntity(entities->at(i));
	}
}
