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

#include <filesystem>
#include <iostream>
#include <format>
#include <vector>
#include <cstring>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include "tokenizer.hpp"
#include "pugixml.hpp"

static bool g_writeToStdout = false;
static bool g_terminalColor = true;
static bool g_verbose = false;
static bool g_quiet = false;
static std::filesystem::path g_outputPath{};
static std::vector<std::filesystem::path> g_responsePaths{};

static bool g_gameHidden = false;
static std::string g_gameId{};
static std::string g_gameName{};
static std::string g_gameInherits{};
static std::vector<std::array<std::string, 3>> g_gameEngines{};
static std::vector<std::pair<std::string, std::string>> g_gameLinks{};
static std::vector<std::pair<std::string, std::string>> g_gameContentPaths{};
static std::vector<std::pair<std::string, std::string>> g_gameAssets{};
static std::string g_gameEntities{};
static std::pair<std::string, std::string> g_gameShaders{};
static std::unordered_map<std::string, std::vector<std::pair<std::string, std::string>>> g_compilerOptions{};
static std::vector<std::pair<std::string, std::string>> g_compilerFlags{};
static std::vector<std::array<std::string, 7>> g_compilerSurfaceParms{};

enum {
	LEVEL_INVALID,
	LEVEL_NORMAL,
	LEVEL_WARNING,
	LEVEL_ERROR
};

static void set_terminal_color(int level) {
	if (!g_terminalColor) {
		return;
	}

	static int currentLevel = LEVEL_INVALID;

	if (currentLevel == level) {
		return;
	}

	currentLevel = level;
	switch (level) {
		case LEVEL_NORMAL: {
			std::cout << "\033[00m"; // reset
			break;
		}

		case LEVEL_WARNING: {
			std::cout << "\033[1m"; // bold
			std::cout << "\033[33m"; // yellow
			break;
		}

		case LEVEL_ERROR: {
			std::cout << "\033[1m"; // bold
			std::cout << "\033[31m"; // red
			break;
		}
	}
}

static void _print(int level, std::string msg) {
	set_terminal_color(level);
	std::cout << msg << std::endl;
	set_terminal_color(LEVEL_NORMAL);
}

#define print(...) do { if (!g_quiet) { _print(LEVEL_NORMAL, std::format(__VA_ARGS__)); } } while(0)
#define verbose_print(...) do { if (!g_quiet && g_verbose) { _print(LEVEL_NORMAL, std::format(__VA_ARGS__)); } } while(0)

static void _warning(std::string msg) {
	_print(LEVEL_WARNING, msg);
}

#define warning(...) _warning(std::format(__VA_ARGS__))

[[noreturn]] static void _error(std::string msg) {
	_print(LEVEL_ERROR, msg);
	exit(EXIT_FAILURE);
}

#define error(...) _error(std::format(__VA_ARGS__))

static void print_header() {
	if (g_quiet) {
		return;
	}
	print("WifeRadiant Gamepack Generator");
	print("version : {}", RADIANT_GIT_REVISION);
	print("date    : {}", RADIANT_GIT_DATE);
	print("");
}

struct argHelp {
	const char* args[2];
	const char* help;
};

static argHelp generalArgs[] = {
	{ { "-h", "--help" }, "Display this help text" },
	{ { "-o", "--output" }, "Output file, use - to print to the standard output" },
	{ { "-v", "--verbose" }, "Enable verbose terminal output" },
	{ { "-q", "--quiet" }, "Disable all terminal output" },
	{ { "--nocolor", NULL }, "Disable terminal output color" },
};

static argHelp gamepackArgs[] = {
	{ { "--hidden", NULL }, "Set gamepack to be hidden from selection" },
	{ { "--id [id]", NULL }, "Set gamepack ID (required)" },
	{ { "--name [name]", NULL }, "Set gamepack name (required)" },
	{ { "--inherits [name]", NULL }, "Set gamepack to inherit from" },
	{ { "--link [name] [url]", NULL }, "Add gamepack documentation link" },
	{ { "--engine [os] [path] [exe]", NULL }, "Add gamepack engine searchpath" },
	{ { "--path [path] [priority]", NULL }, "Add gamepack content search path" },
	{ { "--asset [type] [name]", NULL }, "Add gamepack supported asset type" },
	{ { "--entities [filename]", NULL }, "Set gamepack entities filename" },
	{ { "--shaders [path] [extension]", NULL }, "Set gamepack shader load behaviour" },
	{ { "--compiler [arg] [arg] [value]", NULL }, "Set map compiler keyvalue" },
	{ { "--compiler flag [name] [value]", NULL }, "Add map compiler surface/content flag" },
	{ { "--compiler type [name]", NULL }, "Set map compiler type" },
	{ { "--compiler surfaceparm [args]", NULL }, "Add map compiler surfaceparm" },
};

[[noreturn]] static void print_help() {
	print_header();
	std::cout << "usage: wrgamepack [options] [@response_file]" << std::endl;
	std::cout << std::endl << "general arguments:" << std::endl << std::endl;
	auto printArg = [](const argHelp& arg) {
		if (arg.args[1]) {
			std::cout << std::setw(30) << std::left << std::format("{} {}", arg.args[0], arg.args[1]) << " : " << std::right << arg.help << std::endl;
		} else {
			std::cout << std::setw(30) << std::left << arg.args[0] << " : " << std::right << arg.help << std::endl;
		}
	};
	for (const auto& arg : generalArgs) {
		printArg(arg);
	}
	std::cout << std::endl << "gamepack arguments:" << std::endl << std::endl;
	for (const auto& arg : gamepackArgs) {
		printArg(arg);
	}
	exit(EXIT_SUCCESS);
}

static void parse_args(int argc, const char** argv, int start) {
	for (int i = start; i < argc; i++) {
		if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
			g_quiet = false;
			print_help();
		} else if ((!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output")) && i < argc - 1) {
			g_outputPath = argv[i + 1];
			i += 1;
		} else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")) {
			g_verbose = true;
		} else if (!strcmp(argv[i], "-q") || !strcmp(argv[i], "--quiet")) {
			g_quiet = true;
		} else if (!strcmp(argv[i], "--nocolor")) {
			g_terminalColor = false;
		} else if (!strcmp(argv[i], "--hidden")) {
			g_gameHidden = true;
		} else if (!strcmp(argv[i], "--id") && i < argc - 1) {
			g_gameId = argv[i + 1];
			i += 1;
		} else if (!strcmp(argv[i], "--name") && i < argc - 1) {
			g_gameName = argv[i + 1];
			i += 1;
		} else if (!strcmp(argv[i], "--inherits") && i < argc - 1) {
			g_gameInherits = argv[i + 1];
			i += 1;
		} else if (!strcmp(argv[i], "--link") && i < argc - 2) {
			g_gameLinks.push_back({argv[i + 1], argv[i + 2]});
			i += 2;
		} else if (!strcmp(argv[i], "--path") && i < argc - 2) {
			g_gameContentPaths.push_back({argv[i + 1], argv[i + 2]});
			i += 2;
		} else if (!strcmp(argv[i], "--asset") && i < argc - 2) {
			g_gameAssets.push_back({argv[i + 1], argv[i + 2]});
			i += 2;
		} else if (!strcmp(argv[i], "--entities") && i < argc - 1) {
			g_gameEntities = argv[i + 1];
			i += 1;
		} else if (!strcmp(argv[i], "--shaders") && i < argc - 2) {
			g_gameShaders.first = argv[i + 1];
			g_gameShaders.second = argv[i + 2];
			i += 2;
		} else if (!strcmp(argv[i], "--engine") && i < argc - 3) {
			g_gameEngines.push_back({argv[i + 1], argv[i + 2], argv[i + 3]});
			i += 3;
		} else if (!strcmp(argv[i], "--compiler") && i < argc - 8 && !strcmp(argv[i + 1], "surfaceparm")) {
			g_compilerSurfaceParms.push_back({argv[i + 2], argv[i + 3], argv[i + 4], argv[i + 5], argv[i + 6], argv[i + 7], argv[i + 8]});
			i += 8;
		} else if (!strcmp(argv[i], "--compiler") && i < argc - 3) {
			if (!strcmp(argv[i + 1], "flag")) {
				g_compilerFlags.push_back({argv[i + 2], argv[i + 3]});
			} else {
				g_compilerOptions[argv[i + 1]].push_back({argv[i + 2], argv[i + 3]});
			}
			i += 3;
		} else if (argv[i][0] == '@') {
			g_responsePaths.push_back(&argv[i][1]);
		} else {
			warning("unknown argument {}: {}", i, argv[i]);
		}
	}
}

int main(int argc, const char** argv) {
	if (argc < 2) {
		print_help();
	}

	// parse arguments
	parse_args(argc, argv, 1);

	// check if we need to read response files
	if (!g_responsePaths.empty()) {
		std::vector<std::string> argStrings{};
		std::vector<const char*> args{};
		for (const auto& path : g_responsePaths) {
			std::string line, word;
			std::ifstream file(path);
			if (!file) {
				error("Failed to open response file \"{}\"", path.string());
			}
			std::stringstream buffer;
			buffer << file.rdbuf();
			Tokenizer tokenizer(buffer.str(), "", true);
			for (auto token = tokenizer.getToken(); token; token = tokenizer.getToken()) {
				argStrings.push_back(token);
			}
		}
		if (!argStrings.empty()) {
			for (auto& arg : argStrings) {
				args.push_back(arg.c_str());
			}
			parse_args(args.size(), args.data(), 0);
		}
	}

	// check if writing to stdout
	if (g_outputPath == "-") {
		g_writeToStdout = true;
		g_quiet = true;
	}

	// print header
	print_header();

	// create gamepack document
	pugi::xml_document doc;
	pugi::xml_node radiantNode = doc.append_child("radiant");
	radiantNode.append_attribute("xmlns") = RADIANT_XMLNS;

	// create game node
	pugi::xml_node gameNode = radiantNode.append_child("game");
	gameNode.append_attribute("id") = g_gameId;
	gameNode.append_attribute("name") = g_gameName;
	if (!g_gameInherits.empty()) {
		gameNode.append_attribute("inherits") = g_gameInherits;
	}

	if (g_gameHidden) {
		gameNode.append_attribute("hidden") = true;
	}

	// create link nodes
	for (const auto& link : g_gameLinks) {
		pugi::xml_node node = gameNode.append_child("link");
		node.append_attribute("name") = link.first;
		node.append_attribute("url") = link.second;
	}

	// create engine nodes
	for (const auto& engine : g_gameEngines) {
		pugi::xml_node node = gameNode.append_child(std::format("engine:{}", engine[0]));
		node.append_attribute("path") = engine[1];
		node.append_attribute("executable") = engine[2];
	}

	// create searchpath nodes
	for (const auto& path : g_gameContentPaths) {
		pugi::xml_node node = gameNode.append_child("content");
		node.append_attribute("path") = path.first;
		node.append_attribute("priority") = path.second;
	}

	// create asset nodes
	for (const auto& asset : g_gameAssets) {
		pugi::xml_node node = gameNode.append_child(std::format("asset:{}", asset.first));
		node.append_attribute("name") = asset.second;
	}

	// create entities node
	if (!g_gameEntities.empty()) {
		pugi::xml_node entitiesNode = gameNode.append_child("entities");
		entitiesNode.append_attribute("name") = g_gameEntities;
	}

	// create shaders node
	if (!g_gameShaders.first.empty()) {
		pugi::xml_node shadersNode = gameNode.append_child("shaders");
		shadersNode.append_attribute("path") = g_gameShaders.first;
		shadersNode.append_attribute("extension") = g_gameShaders.second;
	}

	// create compiler nodes
	for (const auto& [key, value] : g_compilerOptions) {
		pugi::xml_node node = gameNode.append_child(std::format("compiler:{}", key));
		for (const auto& option : value) {
			node.append_attribute(option.first) = option.second;
		}
	}

	// create compiler flag nodes
	for (const auto& flag : g_compilerFlags) {
		pugi::xml_node node = gameNode.append_child("compiler:flag");
		node.append_attribute("name") = flag.first;
		node.append_attribute("value") = flag.second;
	}

	// create compiler surfaceparm nodes
	for (const auto& surfaceparm : g_compilerSurfaceParms) {
		pugi::xml_node node = gameNode.append_child("compiler:surfaceparm");
		node.append_attribute("name") = surfaceparm[0];
		node.append_attribute("contentflags") = surfaceparm[1];
		node.append_attribute("contentflagsclear") = surfaceparm[2];
		node.append_attribute("surfaceflags") = surfaceparm[3];
		node.append_attribute("surfaceflagsclear") = surfaceparm[4];
		node.append_attribute("compileflags") = surfaceparm[5];
		node.append_attribute("compileflagsclear") = surfaceparm[6];
	}

	if (g_writeToStdout) {
		doc.save(std::cout);
	} else {
		if (g_outputPath.empty()) {
			error("No output path specified");
		}
		print("Writing {}", g_outputPath.string());
		doc.save_file(g_outputPath.c_str());
	}

	return 0;
}
