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
	componentAddedListener = std::unique_ptr<AddedListener>(new AddedListener(this));
	componentRemovedListener = std::unique_ptr<RemovedListener>(new RemovedListener(this));

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
}

ashley::Entity *ashley::Engine::addEntity(std::unique_ptr<Entity> &&ptr) {
	entities.emplace_back(std::move(ptr));

	auto &added = entities.back();

	updateFamilyMembership(*added);

	added->componentAdded.add((Listener<Entity>*) componentAddedListener.get());
	added->componentRemoved.add((Listener<Entity>*) componentRemovedListener.get());
	added->operationHandler = operationHandler.get();

	notifying = true;

	for (auto &listener : listeners) {
		listener->entityAdded(*added);
	}

	notifying = false;
	removePendingListeners();

	return added.get();
}

ashley::Entity *ashley::Engine::addEntity() {
	return addEntity(std::unique_ptr<Entity>(new Entity()));
}

void ashley::Engine::removeEntity(Entity * const ptr) {
	if (updating) {
		pendingRemovalEntities.push_back(ptr);
	} else {
		removeEntityInternal(ptr);
	}
}

void ashley::Engine::removeAllEntities() {
	while (entities.size() > 0) {
		removeEntity(entities.front().get());
	}
}

ashley::EntitySystem *ashley::Engine::addSystem(std::unique_ptr<EntitySystem> &&system) {
	const auto systemIndex = std::type_index(typeid(*system));

	auto it = systemsByClass.find(systemIndex);

	if (it == systemsByClass.end()) {
		systems.emplace_back(std::move(system));

		systemsByClass.emplace(systemIndex, systems.back().get());
		systems.back()->addedToEngine(*this);

		std::sort(systems.begin(), systems.end(), Engine::systemPriorityComparator);
	}

	return getSystem(systemIndex);
}

void ashley::Engine::removeSystem(std::type_index systemType) {
	auto ptr = getSystem(systemType);
	removeSystem(ptr);
}

void ashley::Engine::removeSystem(EntitySystem * const system) {
	auto ptr = std::find_if(systems.begin(), systems.end(),
			[&](std::unique_ptr<ashley::EntitySystem> &found) {return found.get() == system;});

	if (ptr != systems.end()) {
		systemsByClass.erase(system->identify());
		system->removedFromEngine(*this);
		systems.erase(ptr);
	}
}

ashley::EntitySystem * ashley::Engine::getSystem(std::type_index systemType) const {
	auto ret = systemsByClass.find(systemType);
	return (ret != systemsByClass.end() ? (*ret).second : nullptr);
}

std::vector<ashley::Entity *> *ashley::Engine::getEntitiesFor(Family * const family) {
	auto vecIt = families.find(*family);

	if (vecIt == families.end()) {
		std::vector<ashley::Entity *> entVec;

		for (auto &ptr : entities) {
			if (family->matches(*ptr)) {
				entVec.emplace_back(ptr.get());
				ptr->getFamilyBits().set(family->getIndex(), true);
			}
		}

		families.insert(std::pair<ashley::Family, std::vector<ashley::Entity *>>(*family, entVec));
	}

	return &(families[*family]);
}

const std::vector<ashley::EntitySystem *> ashley::Engine::getSystems() const {
	std::vector<ashley::EntitySystem *> ret;

	for (auto &unique : systems) {
		ret.emplace_back((EntitySystem * const ) unique.get());
	}

	return ret;
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

bool ashley::Engine::systemPriorityComparator(std::unique_ptr<EntitySystem> &one,
		std::unique_ptr<EntitySystem> &other) {
	return (*one) < (*other);
}

void ashley::Engine::updateFamilyMembership(ashley::Entity &entity) {
	// note that this requires that the entity has already been added to the entities vector.
	auto it =
			std::find_if(entities.begin(), entities.end(),
					[&](std::unique_ptr<ashley::Entity> &ptr) {return ptr->getIndex() == entity.getIndex();});

	if (it == entities.end()) {
		// not the end of the world if we had a bad call, probably smells of things getting destroyed incorrectly.
		return;
	}

	auto &entPtr = *it;

	for (auto &pair : families) {
		const auto &family = pair.first;
		auto &vec = pair.second;

		const bool belongsToFamily = entPtr->getFamilyBits()[family.getIndex()];
		const bool matches = family.matches(entity);

		if (!belongsToFamily && matches) {
			vec.push_back(entPtr.get());
			entPtr->getFamilyBits()[family.getIndex()] = true;
		} else if (belongsToFamily && !matches) {
			auto familyIt = std::find_if(vec.begin(), vec.end(),
					[&](Entity * const &found) {return *found == entity;});
			entPtr->getFamilyBits()[family.getIndex()] = false;

			if (familyIt != vec.end()) {
				vec.erase(familyIt);
			}
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
			operation->entity->removeInternal(*(operation->typeIndex));
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

void ashley::Engine::removeEntityInternal(Entity * const entity) {
	auto it = std::find_if(entities.begin(), entities.end(),
			[&](std::unique_ptr<Entity> &ptr) {return ptr.get() == entity;});

	if (it == entities.end()) {
		throw std::bad_function_call();
	} else {
		entities.erase(it);
	}

	if (!entity->getFamilyBits().none()) {
		for (auto &entry : families) {
			const auto &family = entry.first;
			auto &vec = entry.second;

			if (family.matches(*entity)) {
				auto familyIt = std::find_if(vec.begin(), vec.end(),
						[&](Entity *const &ptr) {return ptr == entity;});

				if (familyIt != vec.end()) {
					vec.erase(familyIt);
				}

				entity->getFamilyBits()[family.getIndex()] = 0;
			}
		}
	}

	entity->componentAdded.remove(componentAddedListener.get());
	entity->componentRemoved.remove(componentRemovedListener.get());
	entity->operationHandler = nullptr;

	notifying = true;

	for (EntityListener *listener : listeners) {
		listener->entityRemoved(*entity);
	}

	notifying = false;

	removePendingListeners();
}

void ashley::Engine::EngineOperationHandler::add(ashley::Entity * const entity,
		std::unique_ptr<Component> &component) {
	if (engine->updating) {
		auto operation = engine->operationPool.obtain();
		operation->makeAdd(entity, component);
		engine->operationVector.push_back(operation);
	} else {
		entity->addInternal(component);
	}

}

void ashley::Engine::EngineOperationHandler::remove(ashley::Entity * const entity,
		std::type_index typeIndex) {
	if (engine->updating) {
		auto operation = engine->operationPool.obtain();
		operation->makeRemove(entity, typeIndex);
		engine->operationVector.push_back(operation);
	} else {
		entity->removeInternal(typeIndex);
	}
}
