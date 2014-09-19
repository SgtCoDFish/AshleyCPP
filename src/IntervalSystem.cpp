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

#include "Ashley/systems/IntervalSystem.hpp"

ashley::IntervalSystem::IntervalSystem(float interval) :
		IntervalSystem(interval, ashley::EntitySystem::DEFAULT_PRIORITY) {
}

ashley::IntervalSystem::IntervalSystem(float interval, uint64_t priority) :
		ashley::EntitySystem(priority), interval(interval), accumulator(0) {
}

void ashley::IntervalSystem::update(float deltaTime) {
	accumulator += deltaTime;

	if (accumulator >= interval) {
		accumulator -= interval;
		updateInterval();
	}
}
