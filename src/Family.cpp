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

#include <cstdint>

#include <string>
#include <sstream>
#include <unordered_map>
#include <memory>
#include <bitset>
#include <utility>

#include "Ashley/core/Family.hpp"

uint64_t ashley::Family::familyIndex = 0;
std::unordered_map<ashley::Family::FamilyHashType, std::shared_ptr<ashley::Family>> ashley::Family::families;
ashley::Family::use_getFor_not_constructor ashley::Family::__constructorHider;

std::shared_ptr<ashley::Family> ashley::Family::getFor(ashley::BitsType all, ashley::BitsType one,
		ashley::BitsType exclude) {
	auto hash = ashley::Family::getFamilyHash(all, one, exclude);
	auto family = families.at(hash);

	if (family == nullptr) {
		family = std::make_shared<ashley::Family>(__constructorHider, all, one, exclude);
		families[hash] = family;
	}

	return std::shared_ptr<ashley::Family>(families[hash]);
}

bool ashley::Family::matches(Entity &e) const {
	auto entityComponentBits = e.getComponentBits();

	if (entityComponentBits.none()) {
		return false;
	}

	if ((all & entityComponentBits) != all || (one & entityComponentBits).none()
			|| (exclude & entityComponentBits).any()) {
		return false;
	}

	return true;
}

ashley::Family::FamilyHashType ashley::Family::getFamilyHash(ashley::BitsType all,
		ashley::BitsType one, ashley::BitsType exclude) {
	std::stringstream ss;

	ss << "all:" << all << ",one:" << one << ",exclude:" << exclude;

	return ss.str();
}
