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
#include <typeinfo>
#include <typeindex>

#include "Ashley/core/Engine.hpp"
#include "Ashley/core/Entity.hpp"
#include "Ashley/core/EntityListener.hpp"
#include "Ashley/core/EntitySystem.hpp"
#include "Ashley/core/Family.hpp"
#include "Ashley/internal/ComponentOperations.hpp"

ashley::Engine::Engine() :
		notifying(false), updating(false), operationPool(100) {
	componentAddedListener = std::make_shared<AddedListener>(this);
	componentRemovedListener = std::make_shared<RemovedListener>(this);

	operationHandler = std::unique_ptr<EngineOperationHandler>(new EngineOperationHandler(this));
}

ashley::Engine::~Engine() {
	for (auto &op : operationVector) {
		operationPool.free(op);
	}
	operationVector.clear();
	listeners.clear();

	pendingRemovalEntities.clear();
	removalPendingListeners.clear();
	entities.clear();
	families.clear();

	systems.clear();
	systemsByClass.clear();

	operationHandler = nullptr;
}

void ashley::Engine::addEntity(std::shared_ptr<ashley::Entity> ptr) {
	entities.push_back(ptr);

	updateFamilyMembership(*ptr);
	auto opBase = dynamic_cast<ComponentOperationHandler*>(operationHandler.get());

	ptr->componentAdded.add(componentAddedListener);
	ptr->componentRemoved.add(componentRemovedListener);
	ptr->operationHandler = (opBase);

	notifying = true;

	for (auto &listener : listeners) {
		listener->entityAdded(*ptr);
	}

	notifying = false;
	removePendingListeners();
}

void ashley::Engine::removeEntity(std::shared_ptr<ashley::Entity> &ptr) {
	if (updating) {
		pendingRemovalEntities.push_back(std::shared_ptr<ashley::Entity>(ptr));
	} else {
		removeEntityInternal(std::shared_ptr<ashley::Entity>(ptr));
	}
}

void ashley::Engine::removeAllEntities() {
	while (entities.size() > 0) {
		removeEntity(entities.front());
	}
}

void ashley::Engine::addSystem(std::shared_ptr<ashley::EntitySystem> system) {
	auto systemIndex = std::type_index(typeid(*system));

	auto it = systemsByClass.find(systemIndex);

	if (it == systemsByClass.end()) {
		systems.emplace_back(std::shared_ptr<ashley::EntitySystem>(system));
		systemsByClass[systemIndex] = std::shared_ptr<ashley::EntitySystem>(system);
		system->addedToEngine(*this);

		std::sort(systems.begin(), systems.end(), Engine::systemPriorityComparator);
	}
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
			std::shared_ptr<ashley::EntitySystem>((*ret).second) :
			std::shared_ptr<ashley::EntitySystem>());
}

std::vector<std::shared_ptr<ashley::Entity>> *ashley::Engine::getEntitiesFor(
		ashley::Family &family) {
	auto vecIt = families.find(family);

	if (vecIt == families.end()) {
		std::vector<std::shared_ptr<ashley::Entity>> entVec;

		for (auto &ptr : entities) {
			if (family.matches(*ptr)) {
				entVec.emplace_back(std::shared_ptr<ashley::Entity>(ptr)); // emplace a copy of the shared_ptr
				ptr->getFamilyBits().set(family.getIndex(), true);
			}
		}

		families.insert(std::pair<ashley::Family, std::vector<std::shared_ptr<ashley::Entity>>>(family, entVec));
	}

	return &(families[family]);
}

const std::vector<std::shared_ptr<ashley::EntitySystem>> *ashley::Engine::getSystems() const {
	return &systems;
}

std::vector<std::shared_ptr<ashley::Entity>> *ashley::Engine::getEntitiesFor(
		std::shared_ptr<ashley::Family> family) {
	return getEntitiesFor(*family);
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
	updating = true;

	for (auto &ptr : systems) {
		if (ptr->checkProcessing()) {
			ptr->update(deltaTime);
		}
	}

	processComponentOperations();
	removePendingEntities();
	updating = false;
}

bool ashley::Engine::systemPriorityComparator(std::shared_ptr<ashley::EntitySystem> &one,
		std::shared_ptr<ashley::EntitySystem> &other) {
	return (*one).operator <(*other);
}

void ashley::Engine::updateFamilyMembership(ashley::Entity &entity) {
	// note that this requires that the entity has already been added to the entities vector.
	auto it =
			std::find_if(entities.begin(), entities.end(),
					[&](std::shared_ptr<ashley::Entity> &ptr) {return ptr->getIndex() == entity.getIndex();});

	if (it == entities.end()) {
		// not the end of the world if we had a bad call, probably smells of things getting destroyed incorrectly.
		return;
	}

	auto &entPtr = *it;

	for (auto &pair : families) {
		const auto &family = pair.first;
		auto &vec = pair.second;

		bool belongsToFamily = entPtr->getFamilyBits()[family.getIndex()];
		bool matches = family.matches(entity);

		if (!belongsToFamily && matches) {
			vec.push_back(entPtr);
			entPtr->getFamilyBits()[family.getIndex()] = true;
		} else if (belongsToFamily && !matches) {
			auto familyIt = std::find_if(vec.begin(), vec.end(),
					[&](std::shared_ptr<ashley::Entity> &found) {return *found == entity;});
			entPtr->getFamilyBits()[family.getIndex()] = false;
			vec.erase(familyIt);
		}
	}
}

void ashley::Engine::processComponentOperations() {
	const int numOperations = operationVector.size();

	for (int i = 0; i < numOperations; i++) {
		auto operation = operationVector[i];

		switch (operation->type) {
		case ComponentOperation::Type::ADD: {
			operation->entity->addInternal(operation->component);
			break;
		}

		case ComponentOperation::Type::REMOVE: {
			operation->entity->removeInternal(operation->component);
			break;
		}

		default: {
			throw std::bad_function_call();
			return;
		}
		}

		operationPool.free(operation);
	}

	operationVector.clear();
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

void ashley::Engine::removePendingEntities() {
	const int numPending = pendingRemovalEntities.size();

	for (int i = 0; i < numPending; i++) {
		removeEntityInternal(pendingRemovalEntities[i]);
	}

	pendingRemovalEntities.clear();
}

void ashley::Engine::removeEntityInternal(std::shared_ptr<ashley::Entity> entity) {
	auto it = std::find_if(entities.begin(), entities.end(),
			[&](std::shared_ptr<ashley::Entity> &ptr) {return ptr == entity;});

	if (it == entities.end()) {
		throw std::bad_function_call();
	} else {
		entities.erase(it);
	}

	if (!entity->getFamilyBits().none()) {
		for (auto &entry : families) {
			const auto &family = entry.first;
			std::vector<std::shared_ptr<ashley::Entity>> &vec = entry.second;

			if (family.matches(*entity)) {
				auto familyIt = std::find_if(vec.begin(), vec.end(),
						[&](std::shared_ptr<ashley::Entity> &ptr) {return ptr == entity;});

				if (familyIt != vec.end()) {
					vec.erase(familyIt);
				}

				entity->getFamilyBits()[family.getIndex()] = 0;
			}
		}
	}

	entity->componentAdded.remove(componentAddedListener);
	entity->componentRemoved.remove(componentRemovedListener);
	entity->operationHandler = nullptr;

	notifying = true;

	for (EntityListener *listener : listeners) {
		listener->entityRemoved(*entity);
	}

	notifying = false;

	removePendingListeners();
}

void ashley::Engine::EngineOperationHandler::add(ashley::Entity *entity,
		std::shared_ptr<ashley::Component> component) {
	if (engine->updating) {
		auto operation = engine->operationPool.obtain();
		operation->makeAdd(entity, component);
		engine->operationVector.push_back(operation);
	} else {
		entity->addInternal(component);
	}

}

void ashley::Engine::EngineOperationHandler::remove(ashley::Entity *entity,
		std::shared_ptr<ashley::Component> component) {
	if (engine->updating) {
		auto operation = engine->operationPool.obtain();
		operation->makeRemove(entity, component);
		engine->operationVector.push_back(operation);
	} else {
		entity->removeInternal(component);
	}
}
