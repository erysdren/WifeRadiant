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
#pragma once

#include <filesystem>
#include <vector>
#include <string_view>

// returns the number of gamepacks loaded, or -1 for error
// can be called multiple times to add gamepacks from multiple directories
int GamepackLib_Init(std::filesystem::path& path);

// clean up any memory associated with gamepacklib
void GamepackLib_Quit();

// collect all content paths specified in the given gamepack
int GamepackLib_GetContentPaths(std::vector<std::string>& contentPaths, const char* gameId);

// visitor class
class GamepackLib_Visitor {
public:
	using Args = std::vector<std::pair<std::string_view, std::string_view>>;
	virtual int begin(const char* gameId) { return 0; }
	virtual int visit(const char* gameId, const char* key, const Args& args) { return 0; }
	virtual void end(const char* gameId) { }
};

// iterate over all available games and keyvalue pairs
int GamepackLib_ForEach(GamepackLib_Visitor& visitor);

// iterate over all available keyvalues in the given game
int GamepackLib_ForEach(GamepackLib_Visitor& visitor, const char* gameId);
