---
title: WRGamePack Manual
lang: en-US
dir: ltr
---

# Preface

WRGamePack is a commandline tool designed to streamline the creation of new
gamepacks for WifeRadiant and WRMap.

# Usage

The tool can be used from any terminal like so:

```
wrgamepack [options] [@response_file]
```

Where \[options\] can be any of the following:

# General Options

## -h / --help

Displays a listing of options and other useful information.

## -o [filename] / --output [filename]

Output filename to write the gamepack XML into. Use a single dash character as
the argument to print directly into the terminal rather than into a file.

## -v / --verbose

Enable verbose terminal output. This will print more information as the program
processes the options.

## -q / --quiet

Disable all terminal output from the program.

## --nocolor

Disable the use of terminal color codes in the program output.

# Gamepack Options

## --hidden

Set gamepack to be hidden from selection

## --id [id]

Set gamepack ID (required)

## --name [name]

Set gamepack name (required)

## --inherits [name]

Set gamepack to inherit from

## --type [name]

Set gamepack type

## --link [name] [url]

Add gamepack documentation link

## --wads

Gamepack uses .WAD files

## --outputs

Gamepack supports Source Engine I/O

## --engine [os] [path] [exe]

Add gamepack engine searchpath

## --path [path] [priority]

Add gamepack content search path

## --asset [type] [name]

Add gamepack supported asset type

## --entities [filename]

Set gamepack entities filename

## --icon [filename]

Set gamepack icon filename

## --shaders [path] [extension]

Set gamepack shader load behaviour

## --compiler [arg] [arg] [value]

Set map compiler keyvalue

## --compiler flag [name] [value]

Add map compiler surface/content flag

## --compiler type [name]

Set map compiler type

## --compiler surfaceparm [args]

Add map compiler surfaceparm



# Response File

\[@response_file\] is optional and refers to a text file that contains any
combination of the previously listed options. Usually a response file will look
like this:

```
// inherits hl2.xml
--inherits "hl2"

// game id, name and icon
--id "css"
--name "Counter-Strike: Source"
--icon "css.svg"

// engines
--engine windows "C:/Program Files (x86)/Steam/steam/steamapps/Counter-Strike Source/" "cstrike.exe"
--engine linux "~/.steam/steam/steamapps/common/Counter-Strike Source/" "cstrike.sh"
--engine mac "~/Library/Application Support/Steam/steamapps/common/Counter-Strike Source/" "cstrike.sh"

// content paths
--path "cstrike" "2"

// entities
--entities "cstrike.fgd"
```

The response file can then be used like so:

```
wrgamepack -o css.xml @css.txt
```
