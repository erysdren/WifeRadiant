\pagebreak
# Entities

## _skybox

The _skybox entity "grabs" all the map geometry it can "see" via the normal entity flooding algorithm and assimilates it into the portal sky heap. This is an important concept to understand; if there is any leak between your "main" map and the separate area designated for the _skybox, your compile will take forever, only to produce a very borked .bsp. With that out of the way...

1. Construct your map as normal, texturing your sky brushes with whatever sky shader you fancy.
2. Seal your map off from the void (of course).
3. Create a small box of sky brushes somewhere slightly removed from your "main" map. You could probably get away with drawing a 128x128x128 cube of your sky shader, then using the CSG "hollow" button.
4. Place more geometry within this tiny skybox, but take care not to make it too complicated. Also: unless you compile with -patchmeta, patch meshes shouldn't go inside the _skybox area... you'll end up with MAX_PATCH_PLANES every time.
5. Place a _skybox entity somewhere inside your tiny sky box. The location of the _skybox entity relative to the rest of the sky box geometry determines where the origin of the portal sky will be relative to the rest of your map... a little experimentation will show you how this works.
6. Compile, and that's that. You've got a portal sky in Quake III: Arena!

Notes:

- The "_scale" key defines the _skybox scaling factor. Depending on what kind of portal sky you are going for, values between 64-256 seem to be pretty good. Picking a power of 2 to keep the _skybox geometry vertexes on the grid might be a good idea.
- The "angles" key can be used to put your _skybox geometry off-kilter and make players sea-sick.
- [deformVertexes](#deformVertexes) shaders can be used on _skybox geometry, but the values you use must be scaled up by the same value you assigned to _scale, or else it won't look right.
- You must also scale down the texture coordinates on _skybox geometry, or else the textures you use will appear extremely stretched out.
