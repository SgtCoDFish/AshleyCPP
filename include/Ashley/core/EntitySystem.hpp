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

#include <cstdint>

#ifndef ENTITYSYSTEM_HPP_
#define ENTITYSYSTEM_HPP_

namespace ashley {
class Engine;

/**
 * <p>Abstract class for processing sets of {@link Entity} objects.</p>
 *
 * <em>Java author: Stefan Bachmann</em>
 * @author Ashley Davis (SgtCoDFish)
 */
class EntitySystem {
public:
	/** The default priority when calling the empty constructor */
	static const constexpr uint64_t DEFAULT_PRIORITY = 0;

	/** Use this to set the priority of the system. Lower means it'll get executed first. */
	uint64_t priority = DEFAULT_PRIORITY;

	/**
	 * Default constructor that will initialise an EntitySystem with priority 0.
	 */
	EntitySystem() :
			EntitySystem(DEFAULT_PRIORITY) {
	}

	/**
	 * Initialises the EntitySystem with the priority specified.
	 * @param priority The priority to execute this system with (lower means higher priority).
	 */

	EntitySystem(uint64_t priority) :
			priority(priority) {
	}

	virtual ~EntitySystem() {
	}

	/**
	 * Called when this EntitySystem is added to an {@link Engine}.
	 * @param engine The {@link Engine} this system was added to.
	 */
	virtual void addedToEngine(ashley::Engine &engine) {
	}

	/**
	 * Called when this EntitySystem is removed from an {@link Engine}.
	 * @param engine The {@link Engine} the system was removed from.
	 */
	virtual void removedFromEngine(ashley::Engine &engine) {
	}

	/**
	 * The update method called every tick.
	 * @param deltaTime The time passed since last frame in seconds.
	 */
	virtual void update(float deltaTime) = 0;

	/**
	 * @return Whether or not the system should be processed.
	 */
	virtual inline bool checkProcessing() {
		return true;
	}

	inline bool operator==(ashley::EntitySystem &other) const {
		return this->priority == other.priority;
	}

	inline bool operator!=(ashley::EntitySystem &other) const {
		return this->priority != other.priority;
	}

	inline bool operator<(ashley::EntitySystem &other) const {
		return this->priority < other.priority;
	}

	inline bool operator<=(ashley::EntitySystem &other) const {
		return this->priority <= other.priority;
	}

	inline bool operator>(ashley::EntitySystem &other) const {
		return this->priority > other.priority;
	}

	inline bool operator>=(ashley::EntitySystem &other) const {
		return this->priority >= other.priority;
	}
};
}

#endif /* ENTITYSYSTEM_HPP_ */
