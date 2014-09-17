/*
 * ComponentOperations.hpp
 *
 * Created on: 17 Sep 2014
 * Author: Ashley Davis (SgtCoDFish)
 */

#ifndef COMPONENTOPERATIONS_HPP_
#define COMPONENTOPERATIONS_HPP_

namespace ashley {
namespace internal {

/**
 * <p>Interface representing possible actions on {@link Component}s with respect to {@link Entity}s.</p>
 *
 * <p>Internal class; you probably don't need this.</p>
 */
class ComponentOperationHandler {
public:
	virtual ~ComponentOperationHandler() {}

	virtual void add(std::shared_ptr<ashley::Entity> &entity,
			std::shared_ptr<ashley::Component> &component) = 0;
	virtual void remove(std::shared_ptr<ashley::Entity> &entity,
			std::shared_ptr<ashley::Component> &component) = 0;
};

/**
 * <p>Struct representing all the details required about a component operation.</p>
 */
struct ComponentOperation {
	enum class Type {
		ADD, REMOVE
	};

	Type type;

	std::shared_ptr<ashley::Entity> entity = nullptr;
	std::shared_ptr<ashley::Component> component = nullptr;

	std::type_index componentType;

	inline void makeAdd(std::shared_ptr<ashley::Entity> &entity, std::shared_ptr<ashley::Component> &component) {
		this->type = Type::ADD;

		this->entity = entity;
		this->component = component;

		this->componentType = component->identify();
	}

	inline void makeRemove(std::shared_ptr<ashley::Entity> &entity, std::shared_ptr<ashley::Component> &component) {
		this->type = Type::REMOVE;

		this->entity = entity;
		this->component = component;

		this->componentType = component->identify();
	}
};

}
}

#endif /* COMPONENTOPERATIONS_HPP_ */
