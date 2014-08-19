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

#include "Ashley/core/Family.hpp"

#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>

#include "Ashley/core/Engine.hpp"
#include "Ashley/core/Entity.hpp"
#include "Ashley/core/EntityListener.hpp"
#include "Ashley/core/EntitySystem.hpp"

ashley::Engine::Engine() :
		componentAddedListener(), componentRemovedListener(), notifying(false) {

}

void ashley::Engine::addEntity(std::shared_ptr<ashley::Entity> ptr) {
	entities.emplace_back(ptr);

	for (auto &p : families) {
		const Family& family = p.first;
		std::vector<std::shared_ptr<ashley::Entity>>& vec = p.second;

		if (family.matches(*ptr)) {
			vec.emplace_back(std::shared_ptr<ashley::Entity>(ptr));
			ptr->getFamilyBits().set(family.getIndex(), true);
		}
	}

	ptr->componentAdded.add(componentAddedListener);
	ptr->componentRemoved.add(componentRemovedListener);

	notifying = true;

	for (auto &listener : listeners) {
		listener->entityAdded(*ptr);
	}

	notifying = false;
	removePendingListeners();
}

void ashley::Engine::addEntity(ashley::Entity &entity) {
	addEntity(std::make_shared<ashley::Entity>(entity));
}

std::shared_ptr<ashley::Entity> ashley::Engine::addEntityAndGet(ashley::Entity &entity) {
	auto ptr = std::make_shared<ashley::Entity>(entity);
	addEntity(ptr);

	return std::shared_ptr<ashley::Entity>(ptr);
}

void ashley::Engine::removeEntity(ashley::Entity &entity) {
	// saves implementing three times, for now
	removeEntityAndGet(entity);
}

void ashley::Engine::removeEntity(std::shared_ptr<ashley::Entity> ptr) {
	auto entitiesIt = std::find_if(entities.begin(), entities.end(),
			[&](std::shared_ptr<ashley::Entity> found) {return ptr == found;});

	if (entitiesIt == entities.end()) {
		return;
	}

	entities.erase(entitiesIt);

	if (!ptr->getFamilyBits().none()) {
		for (auto &p : families) {
			const Family& family = p.first;
			std::vector<std::shared_ptr<ashley::Entity>>& vec = p.second;

			if (family.matches(*ptr)) {
				auto vecIt = std::find_if(vec.begin(), vec.end(),
						[&](std::shared_ptr<ashley::Entity> found) {return ptr == found;});

				if (vecIt != vec.end()) {
					vec.erase(vecIt);
				}

				ptr->getFamilyBits().set(family.getIndex(), false);
			}
		}
	}

	ptr->componentAdded.remove(componentAddedListener);
	ptr->componentRemoved.remove(componentRemovedListener);

	notifying = true;

	for (auto &listener : listeners) {
		listener->entityRemoved(*ptr);
	}

	notifying = false;
	removePendingListeners();
}

std::shared_ptr<ashley::Entity> ashley::Engine::removeEntityAndGet(ashley::Entity &entity) {
	auto entitiesIt = std::find_if(entities.begin(), entities.end(),
			[&](std::shared_ptr<ashley::Entity> ptr) {return *ptr == entity;});

	if (entitiesIt == entities.end()) {
		return std::shared_ptr<ashley::Entity>();
	}

	auto ret = *entitiesIt;

	entities.erase(entitiesIt);

	if (!ret->getFamilyBits().none()) {
		for (auto &p : families) {
			const Family& family = p.first;
			std::vector<std::shared_ptr<ashley::Entity>>& vec = p.second;

			if (family.matches(entity)) {
				auto vecIt = std::find_if(vec.begin(), vec.end(),
						[&](std::shared_ptr<ashley::Entity> ptr) {return *ptr == entity;});

				if (vecIt != vec.end()) {
					vec.erase(vecIt);
				}

				ret->getFamilyBits().set(family.getIndex(), false);
			}
		}
	}

	ret->componentAdded.remove(componentAddedListener);
	ret->componentRemoved.remove(componentRemovedListener);

	notifying = true;

	for (auto &listener : listeners) {
		listener->entityRemoved(*ret);
	}

	notifying = false;
	removePendingListeners();

	return ret;
}

void ashley::Engine::removeAllEntities() {
	while (entities.size() > 0) {
		removeEntity(*entities.front());
	}
}

void ashley::Engine::addSystem(std::shared_ptr<ashley::EntitySystem> system) {
	auto systemIndex = std::type_index(typeid(*system));

	if (systemsByClass.find(systemIndex) == systemsByClass.end()) {
		systems.emplace_back(std::shared_ptr<ashley::EntitySystem>(system));
		systemsByClass[systemIndex] = std::shared_ptr<ashley::EntitySystem>(system);
		system->addedToEngine(*this);

		std::sort(systems.begin(), systems.end(), Engine::systemPriorityComparator);
	}
}

void ashley::Engine::addSystem(ashley::EntitySystem *system) {
	addSystemAndGet(system);
}

std::shared_ptr<ashley::EntitySystem> ashley::Engine::addSystemAndGet(
		ashley::EntitySystem *system) {
	auto ptr = std::shared_ptr<ashley::EntitySystem>(system);

	addSystem(ptr);

	return std::shared_ptr<ashley::EntitySystem>(ptr);
}

void ashley::Engine::removeSystem(std::type_index systemType) {
	auto ptr = getSystem(systemType);
	removeSystem(ptr);
}

void ashley::Engine::removeSystem(std::shared_ptr<ashley::EntitySystem> system) {
	auto ptr = std::find_if(systems.begin(), systems.end(),
			[&](std::shared_ptr<ashley::EntitySystem> found) {return found == system;});

	if (ptr != systems.end()) {
		systems.erase(ptr);
		systemsByClass.erase(system->identify());
		system->removedFromEngine(*this);
	}
}

std::shared_ptr<ashley::EntitySystem> ashley::Engine::getSystem(std::type_index systemType) const {
	auto ret = systemsByClass.find(systemType);
	return (ret != systemsByClass.end() ?
			std::shared_ptr<ashley::EntitySystem>((*ret).second) : std::shared_ptr<ashley::EntitySystem>());
}

std::vector<std::shared_ptr<ashley::Entity>> *ashley::Engine::getEntitiesFor(
		ashley::Family &family) {
	auto vecIt = families.find(family);

	if (vecIt == families.end()) {
		std::vector<std::shared_ptr<ashley::Entity>> entVec;

		for (auto ptr : entities) {
			if (family.matches(*ptr)) {
				entVec.emplace_back(std::shared_ptr<ashley::Entity>(ptr)); // emplace a copy of the shared_ptr
				ptr->getFamilyBits().set(family.getIndex(), true);
			}
		}

		families.insert(std::pair<ashley::Family, std::vector<std::shared_ptr<ashley::Entity>>>(family, entVec));
//		immutableFamilies[family] = entVec;
	}

	return &(families[family]);
}

void ashley::Engine::addEntityListener(ashley::EntityListener *listener) {
	listeners.emplace_back(listener);
}

void ashley::Engine::removeEntityListener(ashley::EntityListener *listener) {
	auto it = std::find_if(listeners.begin(), listeners.end(),
			[&](ashley::EntityListener *found) {return found == listener;});

	if (it != listeners.end()) {
		listeners.erase(it);
	}
}

void ashley::Engine::update(float deltaTime) {
	for (auto ptr : systems) {
		if (ptr->checkProcessing()) {
			ptr->update(deltaTime);
		}
	}
}

bool ashley::Engine::systemPriorityComparator(std::shared_ptr<ashley::EntitySystem> &one, std::shared_ptr<ashley::EntitySystem> &other) {
	return (*one).operator <(*other);
}

void ashley::Engine::componentAdded(std::shared_ptr<ashley::Entity> entity) {
	for (auto pair : families) {
		auto &family = pair.first;
		auto &entVec = pair.second;

		if (!(entity->getFamilyBits()[family.getIndex()])) {
			if (family.matches(*entity)) {
				entVec.emplace_back(std::shared_ptr<ashley::Entity>(entity));
				entity->getFamilyBits().set(family.getIndex(), true);
			}
		}
	}
}

void ashley::Engine::componentRemoved(std::shared_ptr<ashley::Entity> entity) {
	for (auto pair : families) {
		auto &family = pair.first;
		auto &entVec = pair.second;

		if (entity->getFamilyBits()[family.getIndex()]) {
			if (!family.matches(*entity)) {
				auto it = std::find(entVec.begin(), entVec.end(), entity);

				if (it != entVec.end()) {
					entVec.erase(it);
					entity->getFamilyBits().set(family.getIndex(), false);
				}
			}
		}
	}
}

void ashley::Engine::removePendingListeners() {
	// read as:
	// for each element in listeners, remove if the same element is found somewhere in removalPendingListeners
	auto it =
			std::remove_if(listeners.begin(), listeners.end(),
					[&](ashley::EntityListener *listenerEntry) {
						return std::find_if(removalPendingListeners.begin(), removalPendingListeners.end(),
								[&](ashley::EntityListener *l) {return listenerEntry == l;}) != removalPendingListeners.end();
					});
	listeners.erase(it, listeners.end());

	removalPendingListeners.clear();
}
