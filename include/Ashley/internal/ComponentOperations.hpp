/*
 * ComponentOperations.hpp
 *
 * Created on: 17 Sep 2014
 * Author: Ashley Davis (SgtCoDFish)
 */

#ifndef COMPONENTOPERATIONS_HPP_
#define COMPONENTOPERATIONS_HPP_

#include <memory>

#include "Ashley/core/Component.hpp"
#include "Ashley/util/ObjectPools.hpp"
#include "Ashley/core/Entity.hpp"

namespace ashley {
class Entity;
class Component;

namespace internal {

/**
 * <p>Interface representing possible actions on {@link Component}s with respect to {@link Entity}s.</p>
 *
 * <p>Internal class; you probably don't need this.</p>
 */
class ComponentOperationHandler {
public:
	virtual ~ComponentOperationHandler() {
	}

	virtual void add(::ashley::Entity *entity,
			std::shared_ptr<ashley::Component> &component) = 0;
	virtual void remove(::ashley::Entity *entity,
			std::shared_ptr<ashley::Component> &component) = 0;
};

/**
 * <p>Struct representing all the details required about a component operation.</p>
 *
 * <p>Internal class; you probably don't need this.</p>
 */
struct ComponentOperation : public ashley::Poolable {
	enum class Type {
		ADD, REMOVE, NONE
	};

	Type type;

	ashley::Entity *entity = nullptr;
	std::shared_ptr<ashley::Component> component = nullptr;

	ComponentOperation() :
			type(Type::NONE) {
	}
	virtual ~ComponentOperation() {
	}

	inline void makeAdd(ashley::Entity *entity,
			std::shared_ptr<ashley::Component> &component) {
		this->type = Type::ADD;

		this->entity = entity;
		this->component = component;
	}

	inline void makeRemove(ashley::Entity *entity,
			std::shared_ptr<ashley::Component> &component) {
		this->type = Type::REMOVE;

		this->entity = entity;
		this->component = component;
	}

	void reset() override {
		entity = nullptr;
		component = nullptr;
		this->type = Type::NONE;
	}
};

}
}

#endif /* COMPONENTOPERATIONS_HPP_ */
