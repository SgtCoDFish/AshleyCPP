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
#include <unordered_map>
#include <memory>
#include <algorithm>

#include "Ashley/core/Family.hpp"
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
	// saves implementing twice
	removeEntityAndGet(entity);
}

std::shared_ptr<ashley::Entity> ashley::Engine::removeEntityAndGet(ashley::Entity &entity) {
	auto entitiesIt = std::find_if(entities.begin(), entities.end(),
			[](std::shared_ptr<ashley::Entity> ptr) {return *ptr == entity;});

	if(entitiesIt == entities.end()) {
		return std::shared_ptr(nullptr);
	}

	auto ret = *entitiesIt;

	entities.erase(entitiesIt);

	if(!ret->getFamilyBits().none()) {
		for (auto &p : families) {
			const Family& family = p.first;
			std::vector<std::shared_ptr<ashley::Entity>>& vec = p.second;

			if(family.matches(entity)) {
				auto vecIt = std::find_if(vec.begin(), vec.end(), [](std::shared_ptr<ashley::Entity> ptr){return *ptr == entity;});

				if(vecIt != vec.end()) {
					vec.erase(vecIt);
				}

				ret->getFamilyBits().set(family.getIndex(), false);
			}
		}
	}

	ret->componentAdded.remove(componentAddedListener);
	ret->componentRemoved.remove(componentRemovedListener);

	notifying = true;

	for(auto &listener : listeners) {
		listener->entityRemoved(*ret);
	}

	notifying = false;
	removePendingListeners();
}

void ashley::Engine::removeAllEntities() {
	while (entities.size() > 0) {
		removeEntity(*entities.front());
	}
}

void ashley::Engine::addSystem(std::shared_ptr<ashley::EntitySystem> system) {

}

void ashley::Engine::addSystem(ashley::EntitySystem &system) {

}

std::shared_ptr<ashley::EntitySystem> ashley::Engine::addSystemAndGet(
		ashley::EntitySystem &system) {

}

void ashley::Engine::removeSystem(ashley::EntitySystem &system) {

}

std::shared_ptr<ashley::EntitySystem> ashley::Engine::removeSystemAndGet(
		ashley::EntitySystem &system) {

}

std::weak_ptr<ashley::EntitySystem> getSystem(std::type_index &systemType) const {

}

const std::vector<std::shared_ptr<ashley::Entity>> ashley::Engine::getEntitiesFor(
		ashley::Family &family) const {
	return std::vector<ashley::Entity *>();
}

void ashley::Engine::addEntityListener(ashley::EntityListener &listener) {

}

void ashley::Engine::removeEntityListener(ashley::EntityListener &listener) {

}

void ashley::Engine::update(float deltaTime) {

}

void ashley::Engine::componentAdded(const ashley::Entity &entity) {

}

void ashley::Engine::componentRemoved(const ashley::Entity &entity) {

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
