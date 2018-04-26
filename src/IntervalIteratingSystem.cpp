#include "Ashley/core/Family.hpp"
#include "Ashley/core/Engine.hpp"
#include "Ashley/systems/IntervalIteratingSystem.hpp"

ashley::IntervalIteratingSystem::IntervalIteratingSystem(Family *family, float interval, int64_t priority) :
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
	for (auto &entity : *entities) {
		processEntity(entity);
	}
}
