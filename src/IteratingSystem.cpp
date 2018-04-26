#include <cassert>
#include <vector>
#include <algorithm>

#include "Ashley/core/Engine.hpp"
#include "Ashley/systems/IteratingSystem.hpp"

void ashley::IteratingSystem::addedToEngine(ashley::Engine &engine) {
	entities = engine.getEntitiesFor(family);
}

void ashley::IteratingSystem::removedFromEngine(ashley::Engine &engine) {
	entities->clear();
}

void ashley::IteratingSystem::update(float deltaTime) {
	std::for_each(entities->begin(), entities->end(),
				  [&](ashley::Entity * found) { processEntity(found, deltaTime); });
}

bool ashley::IteratingSystem::checkProcessing() {
	return !entities->empty();
}
