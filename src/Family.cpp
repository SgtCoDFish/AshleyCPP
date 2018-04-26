#include <sstream>

#include "Ashley/core/Family.hpp"
#include "Ashley/core/ComponentType.hpp"
#include "Ashley/core/Entity.hpp"

uint64_t ashley::Family::familyIndex = 0;
std::unordered_map<ashley::Family::FamilyHashType, ashley::Family::internal_family_ptr> ashley::Family::families;
ashley::Family::use_getFor_not_constructor ashley::Family::constructorHider_;

ashley::Family *ashley::Family::getFor(std::initializer_list<std::type_index> list) {
	ashley::BitsType bits = ashley::ComponentType::getBitsFor(list);

	return getFor(bits, ashley::BitsType(), ashley::BitsType());
}

ashley::Family *ashley::Family::getFor(ashley::BitsType all, ashley::BitsType one, ashley::BitsType exclude) {
	const auto hash = ashley::Family::getFamilyHash(all, one, exclude);

	const auto it = families.find(hash);
	if (it == families.end()) {
		families[hash] = std::unique_ptr<ashley::Family>(new Family(constructorHider_, all, one, exclude));
	}

	return families[hash].get();
}

bool ashley::Family::matches(Entity &e) const {
	auto entityComponentBits = e.getComponentBits();

	if (entityComponentBits.none()) {
		return false;
	}

	return !((all & entityComponentBits) != all || (one.any() && (one & entityComponentBits).none())
			 || (exclude.any() && (exclude & entityComponentBits).any()));

}

ashley::Family::FamilyHashType ashley::Family::getFamilyHash(ashley::BitsType all, ashley::BitsType one,
															 ashley::BitsType exclude) {
	std::stringstream ss;

	ss << "all:" << all << ",one:" << one << ",exclude:" << exclude;

	return ss.str();
}
