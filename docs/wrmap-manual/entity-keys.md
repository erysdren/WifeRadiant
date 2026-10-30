\pagebreak
# Entity Keys

Let it be known that entity keys with a preceeding underscore ("_") character
are keys read by the compiler, and keys with no preceeding underscore are read
by both the compiler and the game.

## _indexmap

- Path to an art file (typically .pcx or .bmp) used to guide the mapping of shaders onto a metashaded surface, such as Quake III: Team Arena-style terrain.
- The path for _indexmap should start in the same directory as is used by your game's fs_basepath switch (i.e. "baseq3"). If your indexmap were found at "C:\\path\\to\\Quake III Arena\\baseq3\\indexmap.pcx", a value of "indexmap.pcx" for the _indexmap key would be sufficient.
- _indexmap used to be "alphamap", though the latter is now deprecated. A key of "alphamap" will still work, but (as it does not follow the underscored compiler key format) its use is not encouraged.
- _indexmap is used on func_group entities.

## _layers

- Number of unique root shaders found in a terrain metashader, such as those used by Quake III: Team Arena-style terrain.
- _layers used to be "layers", though the latter is now deprecated.
- _layers is used on func_group entities.

## _shader

- Path to the metashader used to assign textures to a terrain (func_group) entity. Omit the "textures/" prefix.
- _shader used to be "shader", though the latter is now deprecated.
- _shader is used on func_group entities.

## _lightmapscale

- Floating point value scaling the resolution of lightmaps on brushes/patches in this entity (default 1.0). A _lightmapscale of 0.25 would result in four times as much lightmap detail as 1.0, while 4.0 would only have one quarter of 1.0's detail.
- _lightmapscale is used on func_group entities; though worldspawn supports _lightmapscale, use of lightmap scaling on the world is not recommended.

## _cs or _castshadows

- Allows per-entity control over shadow casting.
- Defaults to 0 on entities, 1 on world. 0 = no shadow casting, 1 = casts shadows on world.
- Can be used in conjunction with _rs (or _receiveshadows) for tighter control of where, exactly, certain shadows fall.
- A _cs value greater than 1 (say, "7") would cast shadows on the world, and on any entity with a corresponding _rs value ("7" again, in our case) as well.
- A _cs value less than 1 (say, "-7") would cast shadows only on entities with a corresponding _rs value ("-7") and not cast shadows on the world.
- _cs is used on any entity.
- misc_model entities, which are static, belongs to world.

## _rs or _receiveshadows

- Allows per-entity control over shadow reception.
- Defaults to 1 on everything. 0 = receives no shadows, 1 = receives world shadows.
- Can be used in conjunction with _cs (or _castshadows) for tighter control of where, exactly, certain shadows fall.
- A _rs value greater than 1 (say, "7") would receive world shadows, as well as shadows from any entity with a corresponding _cs value ("7" again).
- A _rs value less than 1 (say, "-7") would receive shadows only from entities with a corresponding _cs value ("-7") and not receive world shadows.
- _rs is used on any entity.

## _celshader

- Sets the [cel shader](#cel-shading) used for this entity. Omit the "textures/" prefix.
- _celshader is used on any entity.

## _targetname

- Used to attach a misc_model entity to func_whatever entities (i.e. func_bobbing, func_rotating, etc).
- _targetname is generally used on motion entities.

## _keepLights

- By default q3map2 removes light entities from the BSP.
- Set "_keepLights" "1" on the worldspawn to keep light entities in the BSP entity lump.
