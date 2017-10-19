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

#ifndef ACPP_SYSTEMS_SORTEDITERATINGSYSTEM_HPP_
#define ACPP_SYSTEMS_SORTEDITERATINGSYSTEM_HPP_

#include <cstdint>

#include <vector>
#include <functional>
#include <algorithm>

#include "Ashley/systems/IteratingSystem.hpp"
#include "Ashley/core/EntityListener.hpp"

namespace ashley {

/**
 * <p>A simple EntitySystem that processes each entity of a given family in the order specified by a comparator and calls
 * processEntity() for each entity every time the EntitySystem is updated. This is really just a convenience class as rendering
 * systems tend to iterate over a list of entities in a sorted manner. Adding entities will cause the entity list to be resorted.
 * Call forceSort() if you changed your sorting criteria.</p>
 *
 * <em>Java author: Santo Pfingsten</em>
 * @author Ashley Davis (SgtCoDFish)
 */
class SortedIteratingSystem : public ashley::IteratingSystem, public ashley::EntityListener {
public:
	using Comparator = std::function<bool(ashley::Entity *, ashley::Entity *)>;

	SortedIteratingSystem(ashley::Family *family, Comparator comparator, int64_t priority) :
		IteratingSystem(family, priority),
		comparator { comparator } {
	}

	virtual ~SortedIteratingSystem() = default;

	SortedIteratingSystem(const SortedIteratingSystem &other) = default;
	SortedIteratingSystem(SortedIteratingSystem &&other) = default;

	SortedIteratingSystem& operator=(const SortedIteratingSystem &other) = default;
	SortedIteratingSystem& operator=(SortedIteratingSystem &&other) = default;

	void forceSort() {
		shouldSort = true;
	}

	virtual void addedToEngine(ashley::Engine &engine) override;
	virtual void removedFromEngine(ashley::Engine &engine) override;

	virtual void entityAdded(ashley::Entity &entity) override;
	virtual void entityRemoved(ashley::Entity &entity) override;

	virtual void update(float deltaTime) override;

protected:
	Comparator comparator;

	std::vector<Entity *> sortedEntities;

	bool shouldSort { false };

private:
	void sort();
};

}

#endif /* ACPP_SYSTEMS_SORTEDITERATINGSYSTEM_HPP_ */
