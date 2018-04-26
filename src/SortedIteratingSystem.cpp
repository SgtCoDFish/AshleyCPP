#include <iterator>

#include "Ashley/core/Engine.hpp"
#include "Ashley/systems/SortedIteratingSystem.hpp"

namespace ashley {

void SortedIteratingSystem::addedToEngine(Engine &engine) {
	IteratingSystem::addedToEngine(engine);
	sortedEntities.clear();

	if (!entities->empty()) {
		std::copy(entities->begin(), entities->end(), sortedEntities.begin());
		forceSort();
		sort();
	}

	engine.addEntityListener(this);
}

void SortedIteratingSystem::removedFromEngine(Engine &engine) {
	engine.removeEntityListener(this);
	sortedEntities.clear();
	shouldSort = false;
}

void SortedIteratingSystem::entityAdded(Entity &entity) {
	sortedEntities.emplace_back(&entity);
	shouldSort = true;
}

void SortedIteratingSystem::entityRemoved(Entity &entity) {
	sortedEntities.erase(std::remove(sortedEntities.begin(), sortedEntities.end(), &entity), sortedEntities.end());
}

void SortedIteratingSystem::update(float deltaTime) {
	sort();
	std::for_each(sortedEntities.begin(), sortedEntities.end(),
				  [&](ashley::Entity *found) { processEntity(found, deltaTime); });
}

void SortedIteratingSystem::sort() {
	if (shouldSort) {
		std::sort(sortedEntities.begin(), sortedEntities.end(), comparator);
		shouldSort = false;
	}
}

}
