\pagebreak
# Entity Spawnflags

## misc_model

Spawnflag 1
:   Toggles the model casting shadows on the map surfaces.

Spawnflag 2
:   Sets the autoclipping spawnflag, automatically assigning [q3map_clipModel](#q3map_clipModel) to any shaders used by the model.
    Use of Q3Map2 autoclipping for models is only recommended for large models with relatively few triangles in their mesh (i.e. terrain). The Q3Map2 autoclipping algorithm is a bit of a hack, and can hurt in-game performance (as well as produce erroneous clipping results) when used on small, dense models.

Spawnflag 3
:   Spawnflag 1 plus Spawnflag 2.

Spawnflag 4
:   Sets the forcemeta spawnflag, automatically adding [q3map_forceMeta](#q3map_forceMeta) to any shaders used by the model (which, in turn, allows the model to become lightmapped).
    This, effectively, is the "lightmapped model" spawnflag.

Spawnflag 5
:   Spawnflag 1 plus Spawnflag 4.

Spawnflag 6
:   Spawnflag 2 plus Spawnflag 4.

Spawnflag 7
:   Spawnflag 1 plus Spawnflag 2 plus Spawnflag 4.
