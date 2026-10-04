/*
    Copyright (C) 2025-2026 erysdren (it/its)

    This file is part of WifeRadiant.

    WifeRadiant is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    WifeRadiant is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with WifeRadiant.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <format>
#include <map>
#include <algorithm>
#include <vector>

#include "pugixml.hpp"
#include "gamepacklib.hpp"

pugi::xml_document gamepacksDoc{};

int GamepackLib_Init(std::filesystem::path& path) {
	std::error_code error{};
	std::filesystem::directory_iterator dir{path, error};

	if (error) {
		std::cout << __func__ << ": error \"" << error << "\"" << std::endl;
		return -1;
	}

	// add radiant mode if needed
	pugi::xml_node radiantNode = gamepacksDoc.child("radiant");
	if (!radiantNode) {
		radiantNode = gamepacksDoc.append_child("radiant");
		radiantNode.append_attribute("xmlns") = RADIANT_XMLNS;
	}

	// grab all gamepacks
	for (const auto& entry : dir) {
		if (entry.path().extension() != ".xml") {
			continue;
		}
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file(entry.path().c_str());
		if (!result) {
			std::cout << __func__ << ": error \"" << result.description() << "\"" << std::endl;
			continue;
		}

		for (const auto& child : doc.select_nodes("/radiant/game")) {
			radiantNode.append_copy(child.node());
		}
	}

	// count up gamepacks
	int numGamepacks = std::distance(radiantNode.children("game").begin(), radiantNode.children("game").end());

	// check if we got any
	if (numGamepacks == 0) {
		std::cout << __func__ << ": no gamepacks found" << std::endl;
		return -1;
	}

	// resolve inheritence
	for (auto& child : gamepacksDoc.select_nodes("/radiant/game[@id and @inherits]")) {
		std::string id = child.node().attribute("id").value();
		std::string inherits = child.node().attribute("inherits").value();

		// recursive lambdas, yay
		std::vector<std::string> inherited{};
		inherited.push_back(id);
		auto checkInheritence = [&inherited, &child](std::string inherits, auto&& checkInheritence){
			// already inherited from this one
			if (std::find(inherited.begin(), inherited.end(), inherits.c_str()) != inherited.end()) {
				return true;
			}

			// remember that we've inherited from this one
			inherited.push_back(inherits);

			// select nodes with this id
			pugi::xpath_node_set nodes{};
			std::string expression = std::format("/radiant/game[@id='{}']", inherits);
			try {
				nodes = gamepacksDoc.select_nodes(expression.c_str());
			} catch(pugi::xpath_exception& e) {
				std::cout << __func__ << ": error \"" << e.what() << "\"" << std::endl;
				return false;
			}

			// process nodes
			std::vector<std::string> nextInherits{};
			for (const auto& node : nodes) {
				// get next inheritence
				pugi::xml_attribute attr = node.node().attribute("inherits");
				if (attr && std::find(inherited.begin(), inherited.end(), attr.value()) == inherited.end()) {
					nextInherits.push_back(attr.value());
				}

				// copy stuff in
				for (const auto& childNode : node.node()) {
					child.node().append_copy(childNode);
				}
			}

			// process next inheritence
			for (const auto& nextInherit : nextInherits) {
				if (!checkInheritence(nextInherit, checkInheritence)) {
					return false;
				}
			}

			return true;
		};

		if (!checkInheritence(inherits, checkInheritence)) {
			return -1;
		}
	}

	return numGamepacks;
}

void GamepackLib_Quit() {
	gamepacksDoc.reset();
}

int GamepackLib_ForEach(GamepackLib_Visitor& visitor, const char* gameId) {
	// select nodes with this id
	pugi::xpath_node_set nodes{};
	std::string expression = std::format("/radiant/game[@id='{}']", gameId);
	try {
		nodes = gamepacksDoc.select_nodes(expression.c_str());
	} catch(pugi::xpath_exception& e) {
		std::cout << __func__ << ": error \"" << e.what() << "\"" << std::endl;
		return -1;
	}

	// no nodes found
	if (nodes.empty()) {
		return -1;
	}

	// start game
	visitor.begin(gameId);

	// process each node
	int r = 0;
	GamepackLib_Visitor::Args args;
	for (const auto& node : nodes) {
		for (const auto& child : node.node().children()) {
			// collect args
			args.clear();
			for (const auto& attr : child.attributes()) {
				args.push_back({attr.name(), attr.value()});
			}

			// call visitor func
			if ((r = visitor.visit(gameId, child.name(), args)) != 0) {
				visitor.end(gameId);
				return r;
			}
		}
	}

	// end and return
	visitor.end(gameId);
	return r;
}


int GamepackLib_ForEach(GamepackLib_Visitor& visitor) {
	int r = 0;
	std::vector<std::string> names;
	for (const auto& child : gamepacksDoc.select_nodes("/radiant/game[@id]")) {
		if (std::find(names.begin(), names.end(), child.node().attribute("id").value()) == names.end()) {
			names.push_back(child.node().attribute("id").value());
		}
	}
	for (const auto& name : names) {
		if ((r = GamepackLib_ForEach(visitor, name.c_str())) != 0) {
			return r;
		}
	}
	return r;
}

int GamepackLib_GetContentPaths(std::map<int, std::string>& contentPaths, const char* gameId) {
	if (gameId == nullptr || gameId[0] == '\0') {
		return -1;
	}
	int numPaths = 0;
	std::string expression = std::format("/radiant/game[@id='{}']/content[@path and @priority]", gameId);
	for (const auto& node : gamepacksDoc.select_nodes(expression.c_str())) {
		contentPaths[node.node().attribute("priority").as_int()] = node.node().attribute("path").value();
		numPaths++;
	}
	return numPaths;
}

int GamepackLib_GetAssets(std::vector<std::string>& assetTypes, const char* gameId, const char* assetType) {
	if (gameId == nullptr || gameId[0] == '\0') {
		return -1;
	}
	if (assetType == nullptr || assetType[0] == '\0') {
		return -1;
	}
	int numAssets = 0;
	std::string expression = std::format("/radiant/game[@id='{}']/asset:{}[@name]", gameId, assetType);
	for (const auto& node : gamepacksDoc.select_nodes(expression.c_str())) {
		assetTypes.push_back(node.node().attribute("name").value());
		numAssets++;
	}
	return numAssets;
}
