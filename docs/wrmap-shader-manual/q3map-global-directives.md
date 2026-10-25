# Q3Map Global Directives

Q3Map2 global directives change the physical nature of the textures and
the brushes that are marked with them. Changing any of these values will
require the map to be re-compiled. These are global and affect the
entire shader.

## light value {#light}

Tells compiler to generate flare surface with default game flare shader.
Value is not used, but must be present.

## q3map_alphaGen func {#q3map_alphaGen}

Currently takes a single function, *const*. This directive is used to
set a surfaces vertex alpha values. q3map_alphaGen operations are
applied to an object\'s vertexes so the [alphaGen
vertex](#alphaGenVertex) directive is required for
each affected stage.

### q3map_alphaGen const N.N

Forces a fixed vertex alpha value to the entire shader surface, useful
for controlling transparency or blending. Value is a [normalized
number](#normalization).

## q3map_alphaMod func {#q3map_alphaMod}

This is used for special vertex alpha blending effects on surfaces by
altering the vertex alpha values depending on specific surface
properties, such as the surfaces normal axis or the vertexes contained
within its volume. q3map_alphaMod operations are applied to an object\'s
vertexes so the [alphaGen vertex](#alphaGenVertex)
directive is required for each affected stage.

### q3map_alphaMod dotproduct ( X Y Z )

Used to blend textures using
[alphaFunc](#alphaFunc) or
[blendFunc](#blendFunc) in the shader\'s second
pass, with the transparency depending on the surface\'s normal axis
vector. This is achieved by taking the user specified vector ( X Y Z )
and applying a dotproduct calculation with the generated vertex normal
vector to yield a normalized vertex alpha value. The dot product
operation multiplies each element of one vector against the
corresponding elements of a second vector, then adds them:

    ( 0 0 1 ) dp ( 0 0 1 ) = 0 * 0 + 0 * 0 + 1 * 1 = 1
    ( 0 0 1 ) dp ( 0 0.7 0.7 ) = 0 * 0 + 0 * 0.7 + 1 * 0.7 = 0.7
    ( 0.5 -0.5 0.7 ) dp ( 0 0.7 0.7 ) = 0.5 * 0 - 0.5 * 0.7 + 0.7 * 0.7 = 0.14

In a practical sense, you can think of the dotproduct vector ( 0 0 0.9 )
as meaning that all vertices with normal vector pointing straight up
along the z-axis ( 0 0 1 ) will have a vertex alpha value of 90%
opacity, entirely horizontal ( X Y 0 ) will get 0%, and everything in
between will be mapped linearly.

### q3map_alphaMod dotproduct2 ( X Y Z )

This works in a similar way to dotproduct except it exaggerates the
differences in vertex normals by squaring the final dot product value.
With the same values as the above example, dotproduct2 would give the
following:

    [ ( 0 0 1 ) dp ( 0 0 1 ) ]2 = ( 0 * 0 + 0 * 0 + 1 * 1 )2 = 1
    [ ( 0 0 1 ) dp ( 0 0.7 0.7 ) ]2 = ( 0 * 0 + 0 * 0.7 + 1 * 0.7 )2 = 0.49
    [ ( 0.5 -0.5 0.7 ) dp ( 0 0.7 0.7 ) ]2 = ( 0.5 * 0 - 0.5 * 0.7 + 0.7 * 0.7 )2 = 0.0196

Example q3map_dotproduct2 shader for terrain:

    textures/shaderlab_terrain/dirt_rock_grass
    {
        //Used for radiosity lighting
        q3map_lightImage textures/shaderlab_terrain/snow_1.tga

        q3map_nonplanar
        q3map_shadeAngle 179
        q3map_lightmapAxis z

        q3map_tcGen ivector ( 2048 0 0 ) ( 0 2048 0 )
        q3map_tcMod rotate 33

        // this means dot product squared, for faster falloff between vertical and horizontal planes
        q3map_alphaMod dotproduct2 ( 0 0 0.825 )

        {
            map textures/shaderlab_terrain/rock_1.tga
            rgbGen identity
        }
        {
            map textures/shaderlab_terrain/snow_1.tga
            blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
            alphaFunc GE128
            rgbGen identity
            alphaGen vertex
            tcMod scale 7 7
        }
        {
            map $lightmap
            blendFunc GL_DST_COLOR GL_ZERO
            rgbGen identity
        }
    }

### q3map_alphaMod dotproductScale ( X Y Z MIN MAX )

Works similar to q3map_alphaMod dotproduct, but in addition values MIN
and MAX denote a range of cosine values, in which blending will occur.
So less than MIN is mapped to alpha value 0, above MAX is mapped to
alpha value 1, and everything in between is mapped linearly.

If you prefer working with angles, you can set MIN to the cosine of the
most steep angle of the blending, and MAX to the cosine of the most flat
angle of the blending. Example:

    q3map_alphagen dotProductScale ( 0 0 1 0.500 0.966 )

will set alpha value 0 for anything steeper than 60 degrees, and alpha
value 1 for anything more flat than 15 degrees.

### q3map_alphaMod dotproduct2scale ( X Y Z MIN MAX )

The same extension to dotProduct2. Note that the MIN and MAX values are
squared cosine here. You get a different alpha values curve with the
same min/max angles by writing:

    q3map_alphagen dotProduct2scale ( 0 0 1 0.250 0.933 )

> #### Design Notes:
>
> Typical examples of use include snow covering the top faces of
> objects, or terrain with grass growing on horizontal planes blending
> into rocky cliffs on near vertical surfaces. It is an excellent way of
> automatically creating realistic alpha-blended terrain without the
> complicated steps of setting up an alpha map.

### q3map_alphaMod scale N.N

Used in conjunction with q3map_alphaMod volume. Scales the vertex alpha
by the specified [normalized number](#normalization)
value.

### q3map_alphaMod set N.N

Used in conjunction with q3map_alphaMod volume. Sets the vertex alpha
(regardless of any previous alpha values) to the specified [normalized
number](#normalization) value.

### q3map_alphaMod volume {#q3map_alphaModVolume}

This was created as a way to explicitly set or modify the vertex alpha
values of vertex points contained within a controlling brush volume
marked with this shader directive. Applies all other q3map_alphaMod
directives to each vertex inside a brush textured with this shader,
allowing large faded scrolling fire shaders, waterfalls, marquees,
explicit dotProduct terrain blending control, etc.

> #### Design Notes:
>
> This is usually used in special alphaMod volume \"common\" shaders for
> use within the editor only. A brush textured with the alphaMod volume
> shader is used to overlap the vertexes of another brush or model using
> an alpha-blended shader, altering the vertex alpha values. Worldspawn
> alphaMod volume brushes will affect all surfaces it comes in contact
> with. You can func_group an alphaMod volume brush to its affecting
> brush to localize the blend to just that entity.

## q3map_backShader shadername {#q3map_backShader}

This allows a brush to use a different shader when you are inside it
looking out. By way of example, this would allow a water brush (or
other) surfaces to have a different [sort](#sort)
order or appearance when seen from the inside. This is essentially
shorthand of
[q3map_cloneShader](#q3map_cloneShader)
where the target shader contains
[q3map_invert](#q3map_invert).

## q3map_backSplash percentage distance {#q3map_backSplash}

By default [surface
light](#q3map_surfaceLight) generates extra
\'backsplash\' light to light itself and surrounding. That light source
is moved away from the surface of the shader, allowing it to create
smoother lighting over the face. By default, all shaders are assigned
backsplash values, 5 for percentage, 23 units for distance.

percentage
:   Specifies the intensity percentage of the light generated by
    [q3map_surfaceLight](#q3map_surfaceLight)
    to be redirected back at the surface. Use a value of 0 or a negative
    value to disable back splash lights.

distance
:   Distance of the back splash lights from the surface.

## q3map_baseShader shadername {#q3map_baseShader}

Allows shaders to be subclassed (Q3Map2 relevant portions only, such as
surfaceparms, lighting, texture projection, etc). Subclassed shaders can
reference the base shader by referring to the base shader\'s name. In
order for q3map_baseShader to work correctly, the base shader must be
specified before any shaders that subclass it. Some EasyGen terrain
templates incorrectly specified the base shader after the terrain
shaders that depended on it, resulting in some interesting errors.

This is fundamentally the reverse of
[q3map_remapShader](#q3map_remapShader). Use
q3map_baseShader when a single group of q3map\_\* directives is required
for multiple base shaders. Use
[q3map_remapShader](#q3map_remapShader) when
similar shaders are used that require different groups of q3map\_\*
directives.

## q3map_bounce N.N {#q3map_bounce}

Deprecated! Use q3map_bounceScale instead.

## q3map_bounceScale N.N {#q3map_bounceScale}

Takes positive decimal number values to scale the amount of light
reflected in radiosity passes. You can oversaturate it by using a number
higher than 1.0, but this can lead to excessive compile times. Using 0.9
would probably make things positively glacial. 1.0 is a default, fudged
number that looked OK with the maps that were tested. Tweaking it to 1.5
or 2.0 won\'t hurt anything per se, but it does give you finer control
over how each shader re-emits light. The poorly worded q3map_bounce has
been renamed to q3map_bounceScale. While its use has been deprecated,
q3map_bounce still works.

## q3map_clipModel {#q3map_clipModel}

Automatically clips misc_model entities for player and weapon collision.
This should only be used on large models such as terrain (not small
decorative models - manually clip those). The shader\'s surfaceparms are
inherited by the magic clip brush, so if you have [surfaceparm
nonsolid](#nonsolid) in your
model\'s shader that uses q3map_clipModel, then the brush will also be
non-solid. This can also be set on a per model basis with the entity
key/value pair, spawnflags 2.

## q3map_cloneShader shadername {#q3map_cloneShader}

Clones surface and applies target shader\'s properties and appearance to
clone. Be careful, this can lead to an infinite loop if a cloning shader
references another cloning shader or itself.

## q3map_colorGen func {#q3map_colorGen}

Currently takes a single function, *const*. This directive is used to
set a surfaces vertex color values. q3map_colorGen operations are
applied to an object\'s vertexes so the [rgbGen
vertex](#rgbGenVertex) directive is required for
each affected stage. It\'s also needed to use
[q3map_noVertexLight](#q3map_noVertexLight)
or -novertex to avoid overwriting RGB vertex values during -light build
stage. Same as
[q3map_rgbGen](#q3map_rgbGen).

### q3map_colorGen const ( R G B )

Forces a fixed vertex color value to the entire shader surface.

## q3map_colorMod func {#q3map_colorMod}

Used to alter the vertex color values of vertexes contained within its
volume. q3map_colorMod operations are applied to an object\'s vertexes
so the [rgbGen vertex](#rgbGenVertex) directive is
required for each affected stage. It\'s also needed to use
[q3map_noVertexLight](#q3map_noVertexLight)
or -novertex to avoid overwriting RGB vertex values during -light build
stage. Possible uses include creating a neutral hued texture for a CTF
middleground and then creating shader instances of the texture for red
and blue bases. Same as
[q3map_rgbMod](#q3map_rgbMod).

### q3map_colorMod scale ( R G B )

Used in conjunction with q3map_colorMod volume. Scales the vertex color
by the specified color values.

### q3map_colorMod set ( R G B )

Used in conjunction with q3map_colorMod volume. Sets the vertex color to
the specified color values.

### q3map_colorMod volume

This was created as a way to explicitly set or modify the vertex color
values of vertex points contained within a controlling brush volume
marked with this shader directive. Applies all other q3map_colorMod
directives to each vertex inside a brush textured with this shader to
change the hue of the affected surface.

> #### Design Notes:
>
> This is usually used in special colorMod volume \"common\" shaders for
> use within the editor only. A brush textured with the colorMod volume
> shader is used to overlap the vertexes of another brush or model,
> altering the vertex color values. Worldspawn colorMod volume brushes
> will affect all surfaces it comes in contact with. You can func_group
> a colorMod volume brush to its affecting brush to localize the hue to
> just that entity.

### q3map_colorMod dotproduct ( X Y Z )

### q3map_colorMod dotproduct2 ( X Y Z )

### q3map_colorMod dotproductScale ( X Y Z MIN MAX )

### q3map_colorMod dotproduct2scale ( X Y Z MIN MAX )

These ones work just like [q3map_alphaMod
dotproduct\*](#q3map_alphaMod), but are
modifying vertex color values.

## q3map_deprecateShader newShaderName {#q3map_deprecateShader}

Rename current shader to newShaderName, so it will be shown in a map as
newShaderName. Similar to
[q3map_remapShader](#q3map_remapShader),
but, unlike it, performs renaming in early loading stage, so compile
parameters of newShaderName are used.

## q3map_flare shadername {#q3map_flare}

Tells compiler to generate flare surface with specified shader.

## q3map_flareShader shadername {#q3map_flareShader}

q3map_flare alias.

## q3map_floodLight r g b dist intensity power {#q3map_floodLight}

Overrides the global floodlight (sort of ambient occlusion) parameters.

r g b
:   Light color, e.g. 240 240 255.

dist
:   The distance (in game units) at which occlusion test starts making
    an effect, e.g. 1024.

intensity
:   Light intensity, e.g. 128.

power
:   Light direction power, e.g. 1.

## q3map_fogDir ( X Y Z ) {#q3map_fogDir}

Specifies the direction a fog shader fades from transparent to opaque.
Values indicate a directional vector.

## q3map_foliage model scale density odds invert_alpha {#q3map_foliage}

Wolf ET randomized instanced foliage.

model
:   Model path.

scale
:   Model scale.

density
:   Distribution density.

odds
:   Controls placement probability: placed if (alpha \* odds) \>
    random(0..1).

invert_alpha
:   0: use vertex alpha, 1: use inverted vertex alpha, 2: use alpha =
    1.0.

## q3map_forceMeta {#q3map_forceMeta}

Forces model (MD3, ASE, etc.) surfaces to be broken down into their
component triangles like brush faces and passed through the meta code on
a per shader basis. This is required for lightmapped models. Setting
spawnflags 4 on a misc_model will set q3map_forceMeta on all its
surfaces.

## q3map_forceSunlight {#q3map_forceSunlight}

Obsolete! By default, no sunlight is cast on vertex-lit .md3 models or
vertex-lit terrain. Using this option, sunlight (overbright bits created
by the [q3map_sun](#q3map_sun) option) will
be cast on these surfaces. q3map_forceSunlight is now obsolete since
suns are now first class light sources.

## q3map_fur layers offset fade {#q3map_fur}

This is used for generating fur over a surface. This is typically used
in conjunction with
[q3map_cloneShader](#q3map_cloneShader) in
the surface (parent) shader and references the fur shader as the clone.
A possible application of this is to create grass on alphablended
terrain. Keep in mind that the use of a fur shader may cause a large hit
to performance but when used sparingly, it can produce some interesting
effects. (See [Appendix: Fur](#fur))

layers
:   This specifies the number of desired replicated fur layers
    generated.

offset
:   The distance (in game units) between subsequent layers. It is
    additionally scaled by original vertex alpha color component.

fade
:   A [normalized value](#normalization) indicating
    the fade falloff between subsequent layers.

## q3map_globalTexture {#q3map_globalTexture}

Use this shader in the global directive commands whenever the [tcMod
scale](#tcModScale) function is used in one of the
later render stages. Many problems with getting shader effects to work
across multiple adjacent brushes are a result of the way Q3Map2
optimizes texture precision. This option resolves that, but at the
expense of some precision of the textures when they are far away from
the origin of the map.

## q3map_indexed

This is used for explicit terrain-style indexed mapping. It instructs
Q3Map2 to look at the func_group terrain entity\'s \_indexmap key for an
image to pull index values from, and then to construct a shader name
with the root based on the \_shader key\'s value.

## q3map_invert

Inverts a surface normal. Works on brush faces, models and patches. Used
in [celshading](#cel-shading) to achieve the inverted backfacing
hull. Can be used with a shader that is targeted by
[q3map_cloneShader](#q3map_cloneShader) for
something similar to
[q3map_backShader](#q3map_backShader).

## q3map_lightImage texturename {#q3map_lightImage}

By default, surface lights use the average color of the source image to
generate the color of the light. q3map_lightImage specifies an alternate
image to be used for light color emission, radiosity color emission,
light filtering and alpha shadows. You can even use a light image with a
different alpha channel for blurrier alpha shadows. The light color is
averaged from the referenced texture. The texture must be the same size
as the base image map. q3map_lightImage should appear before
[qer_editorImage](#qer_editorImage).

The reason q3map_lightImage is specified for the light in the example
below, is because the blend map is predominantly yellow, but the base
image is not. The designer wanted the color of the light to be sampled
from the blend map instead of the base image.

    textures/eerie/ironcrosslt2_10000
    {
        // this TGA is the source for the color of the blended light
        q3map_lightImage textures/gothic_light/ironcrosslt2.blend.tga

        //editor TGA (used because the shader is used with several different light values)
        qer_editorImage textures/gothic_light/ironcrosslt2.tga

        //emitted light value of 10,000
        q3map_surfacelight 10000

        {
            //source texture is affected by the lightmap
            map $lightmap

            // this command handles the overbright bits created by "sunlight" in the game
            rgbGen identity
        }
        {
            map textures/gothic_light/ironcrosslt2.tga
            blendFunc filter
            rgbGen identity
        }
        {
            maptextures/gothic_light/ironcrosslt2.blend.tga
            blendFunc add
        }
    }

## q3map_lightmapAxis axis {#q3map_lightmapAxis}

Takes a single argument: either X, Y or Z as the direction from which
the lightmap is projected from. This directive is not recommended for
things like caves or arches which have undersides.

## q3map_lightmapBrightness N.N {#q3map_lightmapBrightness}

Lightmap brightness scaling. A value of 2.0 will be twice as bright
(linearly) and a value of 0.5 will be half as bright.

## q3map_lightmapFilterRadius self other {#q3map_lightmapFilterRadius}

This is usually used on light emitting shaders to approximate finer
subdivided lighting. It adds a gaussian blur effect to the lightmaps of
either the shader itself, or the surfaces affected by the shader, or
both. The values for self and other are measured in world units of
filtering (blurring) of lightmap data cast by any light sources.

self
:   Amount of blur set for the shader itself to approximate for the
    surfacelight\'s finer subdivided lighting. It should be set to 0 for
    sky shaders since they don\'t have lightmaps.

other
:   Amount of blur set for other surfaces affected by this shader\'s
    emitted light. It should be set just high enough to eliminate the
    \"stadium shadow\" effect sometimes produced by
    [q3map_skylight](#q3map_skylight) or to
    smooth out the lighting on surfacelights.

If using a value higher than 4 for the iterations parameter on
[q3map_skylight](#q3map_skylight), you
don\'t need q3map_lightmapFilterRadius as much, but at the expense of
higher compile times. q3map_lightmapFilterRadius should be placed before
any light related shader directives that you want it to affect. (see
Appendix: [Light Emitting Shaders](#light-emitting-shaders))

## q3map_lightmapGamma N.N {#q3map_lightmapGamma}

Deprecated! Use
[q3map_lightmapBrightness](#q3map_lightmapBrightness)
instead.

## q3map_lightmapMergable {#q3map_lightmapMergable}

Allows surfaces like terrain to be mapped onto a single lightmap page
for seamless terrain shadows. It will specify that the shaders using it
can merge nonplanars together onto a single lightmap, so you can have a
single 512x512 lightmap across a terrain entity. Note that most id Tech
3 engine games are limited to lightmap pages of 128x128. You may need to
experiment with
[q3map_lightmapSize](#q3map_lightmapSize)
and the Q3Map2 -export general switch.

## q3map_lightmapSampleOffset distance {#q3map_lightmapSampleOffset}

Takes a single parameter, defaulting to 1.0, which specifies how many
units off a surface should Q3Map2 sample lighting from. Use larger
values (2.0-8.0) if you\'re getting ugly splotches on lightmapped
terrain. Try to use filtering to solve splotches if possible, leaving
q3map_lightmapSampleOffset as a last resort.

## q3map_lightmapSampleSize N {#q3map_lightmapSampleSize}

Surfaces using a shader with this option will have the pixel size of the
lightmaps set to N world grid units. This option can be used to produce
high-resolution shadows on certain surfaces. In addition, it can be used
to reduce the size of lightmap data, where high-resolution shadows are
not required, gaining memory and performance benefits. The default Quake
III lightmap sample size is 16, so square of 16x16 units will be \'lit\'
by single lightmap pixel. Smaller numbers increases lighting resolution.
In general, you should stick with power of 2 values.

## q3map_lightmapSize width height {#q3map_lightmapSize}

Specifies the size of the lightmap page that surface lightmaps get
packed on to. Used mainly in Enemy Territory for terrain lightmaps
(512x512) in concert with the Q3Map2 switch -lightmapsize. Most idTech 3
games will only support internal lightmap pages of 128x128. Larger pages
will result in lightmaps exported externally from the .bsp file.

## q3map_lightRGB red green blue {#q3map_lightRGB}

This forces a specified color of light to be emitted from a surface or
sky light, rather than sampling colors from a
[lightimage](#q3map_lightImage), [editor
image](#qer_editorImage) or the
[texture map](#map). Three normalized color values
of light are required for the red green blue parameters. This does not
affect bounced light in radiosity or
[lightfilter](#lightfilter).

## q3map_lightStyle N {#q3map_lightStyle}

Used to set light styles on surface lights for lightmap
flickering/waveform effects. N takes the form of a number between 1 and
31, that references the corresponding style number set in the worldspawn
entity. Equivalent to setting \"style\" \"N\" on a light entity (see
Appendix: [Lightstyles](#lightstyles)).

## q3map_lightSubdivide N {#q3map_lightSubdivide}

Used on surface lights (see
[q3map_surfaceLight](#q3map_surfaceLight)).
Controls maximum size, above which surface light source is split for
uniform lighting. It defaults to 999 game units, but can be made larger
or smaller as needed (for light surfaces at the bottom of cracks, for
example). This can be a dominant factor in processing time for Q3Map2
lighting. Can have an increasingly \"darker\" effect when used with
-fast. Compensate by raising the surface light value. For sky shaders,
use [q3map_skylight](#q3map_skylight)
instead (see Appendix: [Light Emitting
Shaders](#light-emitting-shaders)).

Surface lights are also used for -bounce lighting with default starting
subdivision value of 256. q3map_lightSubdivide controls this value too.

## q3map_noClip {#q3map_noClip}

You might have noticed that terrain has been made to not clip or
t-junction anymore. It was causing too many issues, so a new parameter
was added: q3map_noClip. Normally, Q3Map2 clips all faces to the BSP,
and then takes the minimum polygon that encompasses all visible
fragments. q3map_noClip forces Q3Map2 to uses the original brush faces
(This is implicit for
[autosprite(2)](#deformVertexesAutosprite)
surfaces). Therefore, if you map tidy, you could theoretically use
q3map_noClip on all your shaders. q3map_noClip and
[q3map_noTJunc](#q3map_noTJunc), when used
in combination, will preserve mesh geometry exactly as you make it.

## q3map_noDirty {#q3map_noDirty}

Don\'t apply -dirty on this surface.

## q3map_noFast {#q3map_noFast}

When used on surfaces that emit light, this will disable -fast
optimizations. This is useful for large areas of dim sky, where you want
the dim light to reach all surfaces. This shader directive prevents
-fast from affecting dim sky surfaces. It is necessary, if you can\'t do
a workaround with brighter skies or by using a larger
[q3map_lightSubdivide](#q3map_lightSubdivide)
value.

## q3map_noFog {#q3map_noFog}

Volumes marked with a shader containing this directive will not be
affected by fog. For example, it can be used on sky shaders to prevent
the fog from affecting it.

## q3map_nonPlanar {#q3map_nonPlanar}

Instructs Q3Map2 to merge any adjacent triangles that don\'t lie in the
same plane into a non-planar triangle soup. This allows shadows to be
cast correctly across non-planar edges. It is typically used on
lightmapped terrain shaders.

## q3map_normalImage texturename {#q3map_normalImage}

Allow the use of a normal (height) map to simulate textured bumpmapping.
This isn\'t real bumpmapping per se, but generates a static lightmap
image that reflects the normal map and light source.

## q3map_noTJunc {#q3map_noTJunc}

Read as \"no T-Junc\". With this option, surfaces modified by a shader
are not used for T-junction fixing.
[q3map_noClip](#q3map_noClip) and
q3map_noTJunc, used in combination will preserve mesh geometry exactly
as you make it.

## q3map_noVertexLight {#q3map_noVertexLight}

Suppresses Q3Map2 from overwriting the BSP-phase set vertex RGB values
by -light phase.

## q3map_noVertexShadows {#q3map_noVertexShadows}

Obsolete! Vertex lighting code was rewritten a couple of times,
rendering this directive irrelevant. Shaders that are used on
misc_models or terrain were to use q3map_noVertexShadows to disable
shadows being cast on the vertex lit surfaces. Casting shadows at small,
misc_model objects often makes sense. However, having shadows on large,
vertex lit terrain surfaces often looks bad. Shadows are not cast on
forced_vertex_lit surfaces by default (shaders with
[pointlight](#pointlight)).

## q3map_offset N.N {#q3map_offset}

Offsets a surface along the vertex normals by N.N units. Used in
[celshading](#cel-shading) for those black edges.

## q3map_patchShadows {#q3map_patchShadows}

Obsolete! When this option is used in conjunction with the original
lighting algorithm (-light), surfaces with textures modified with this
option will show shadows that are cast by curve patches. Curve patches
do not cast shadows by default. You would usually force patches to cast
shadows by using the -patchshadows Q3Map2 switch.

## q3map_remapShader shadername {#q3map_remapShader}

Allows shaders to be subclassed. Subclassed shaders can reference the
remap shader by referencing the remap shader\'s name. In order for this
to work, the remap shader must be specified before any shaders that
references it. Subclassed shaders must contain Q3Map2 and editor
relevant portions only (q3map\_\*, surfaceparm and qer\_\* directives).

This is fundamentally the reverse of
[q3map_baseShader](#q3map_baseShader). Use
[q3map_baseShader](#q3map_baseShader) when a
single group of q3map\_\* directives is required for multiple base
shaders. Use q3map_remapShader when similar shaders are used that
require different groups of q3map\_\* directives.

> #### Design Notes:
>
> Since the subclassed shaders that are applied to geometry does not
> contain any texture stages,
> [qer_editorImage](#qer_editorImage)
> is usually required in conjunction with q3map_remapShader.

## q3map_rgbGen func {#q3map_rgbGen}

Same as [q3map_colorGen](#q3map_colorGen)

## q3map_rgbMod func {#q3map_rgbMod}

Same as [q3map_colorMod](#q3map_colorMod)

## q3map_shadeAngle angle {#q3map_shadeAngle}

Specifies the breaking angle for phong shading. This allows for smooth
shadows between brush faces like on patches. The angle parameter is the
angle between adjacent faces at which smoothing will start to occur.
Typical values are usually in the 120-179 range.

## q3map_skylight *amount iterations* [optional]{.small}\[*horizon_min horizon_max sample_color*\] {#q3map_skylight}

This replaces
[q3map_surfaceLight](#q3map_surfaceLight)
and
[q3map_lightSubdivide](#q3map_lightSubdivide)
on sky surfaces for much faster and more uniform sky illumination. May
be used multiple times in a single shader. See Appendix: [Light Emitting
Shaders](#light-emitting-shaders).

amount
:   Brightness value, similar to what you would use in
    [q3map_sun](#q3map_sun). Good values are
    between 50 and 200.

iterations
:   An exponential factor. 3 is the best value that balances speed and
    quality. Values of 4 and 5 are higher quality at the expense of
    higher compile time. Values below 3 are not too useful.

horizon_min horizon_max
:   Two spherical angles, defining portion of sphere to emit light from.
    Default is *0 90*, which is upper hemisphere. *-90 90* will be whole
    sphere.

sample_color
:   Default = 1: sample color of each individual light from skybox
    images, if they are present. 0: use shader color, set by
    [q3map_lightRGB](#q3map_lightRGB)/[q3map_lightImage](#q3map_lightImage)/[\_up
    skybox
    image](#skyParms)/[qer_editorImage](#qer_editorImage).

## q3map_splotchFix {#q3map_splotchFix}

This is used on lightmapped model shaders if splotched lighting
artifacts appear. Any shadows at the ambient/dark level will be flooded
from neighbouring luxels. This gets rid of shadow acne, but a surface
must be more or less uniformly lit or this looks ugly. Try using
[q3map_lightmapSampleOffset](#q3map_lightmapSampleOffset)
first before using this as a last resort.

## q3map_styleMarker {#q3map_styleMarker}

For use on shaders that accompany style lights. For any shaders that may
be hit by a styled light, add q3map_styleMarker after the lightmap stage
and before the texture stages so Q3Map2 can properly create the fake
lightmap stages (see Appendix: [Lightstyles](#lightstyles)).

## q3map_styleMarker2 {#q3map_styleMarker2}

Similar to q3map_styleMarker except it is used on masked textures where
a [depthFunc equal](#depthFunc) is required.

## q3map_sun red green blue intensity degrees elevation {#q3map_sun}

This directive in a sky shader will create the illusion of light cast
into a map by a single, infinitely distance parallel light source (sun,
moon, hellish fire, etc.). This is only processed during the lighting
phase of Q3Map2. While still perfectly usable, q3map_sun is now
deprecated in favour of
[q3map_sunExt](#q3map_sunExt) (see below).

## q3map_sunExt red green blue intensity degrees elevation deviance samples {#q3map_sunExt}

Works like [q3map_sun](#q3map_sun) with the
addition of two new parameters to create \"light jittering\" for
penumbra (half-shadow) effects. This gives you much more realistic
shadows from the sun, especially when trying to simulate a cloudy day or
a wide sun. The penumbra effect can also be applied to entity lights
(point, spot or sun) with the \_deviance N (distance in world units for
point/spot lights and degrees for suns) and \_samples N (number of
samples) key/value pairs.

red green blue
:   Color is described by three RGB values. Color will be normalized to
    a 0.0 to 1.0 range, so it doesn\'t matter what range you use.

intensity
:   The brightness of the generated light. A value of 100 is a fairly
    bright sun. The intensity of the light falls off with angle but not
    distance.

degrees
:   The angle relative to the directions of the map file. A setting of 0
    degrees equals east. 90 is north, 180 is west and 270 is south. In
    the original version of Q3Map, non-axial values had a tendency to
    produce jagged shadows. With Q3Map2, this problem is avoided with
    new options like lightmap filtering, raytracing and penumbra
    effects.

elevation
:   The angle, measured in degrees from the horizon. An elevation of 0
    is sunrise/sunset. An elevation of 90 is noon.

deviance
:   The number of degrees for the half-shadow. General values up to 2 or
    3 are acceptable. The real sun has a solid angle of about half a
    degree.

samples
:   The number of random jitters distributed over the solid arc (\~16).

> #### Design Notes:
>
> Sky shaders should probably still have a
> [q3map_surfaceLight](#q3map_surfaceLight)
> or preferred
> [q3map_skylight](#q3map_skylight) value.
> The \"sun\" gives a strong directional light, but doesn\'t necessarily
> give the fill light needed to soften and illuminate shadows. Skies
> with clouds should probably have a weaker
> [q3map_sun](#q3map_sun) value and a higher
> [q3map_surfaceLight](#q3map_surfaceLight)
> or [q3map_skylight](#q3map_skylight)
> value. Heavy clouds diffuse light and weaken shadows. The opposite is
> true of a cloudless or nearly cloudless sky. In such cases, the
> \"sun\" or \"moon\" will cast stronger shadows that have a greater
> degree of contrast. This is also why q3map_sunExt is preferred. It
> gives the designer greater control over shadow contrast with a
> penumbra effect.
>
> #### Design Notes:
>
> Not certain what color formula you want to use for the sun\'s light?
> Try this. Create a light entity. Use the Radiant editor\'s color
> selection tools to pick a color. The light\'s \_color key\'s value
> will be the normalized RGB formula. Copy it from the value line in the
> editor (CTRL+c) and paste it into your shader.

## q3map_surfaceLight value {#q3map_surfaceLight}

The surface gives off light equal to the value set for it. The relative
surface area in the world affects the actual amount of light that
appears to be radiated. To give off what appears to be the same amount
of light, a smaller surface must be significantly brighter than a larger
one. Unless the
[q3map_lightImage](#q3map_lightImage) or
[q3map_lightRGB](#q3map_lightRGB) directive
is used to select a different source for the shader\'s light color
information, the color of the light will be the averaged color of the
texture. For sky shaders, use
[q3map_skylight](#q3map_skylight) instead
for faster and more uniform sky illumination.

## q3map_surfaceModel modelpath density odds minscale maxscale minangle maxangle oriented {#q3map_surfaceModel}

A surface with q3map_surfaceModel in its shader will randomly place a
specified model across it\'s face. This is designed to place grass or
tree models over terrain.

modelpath
:   The path to the model file (any supported format).

density
:   The density of the models, in game units.

odds
:   The odds of the model appearing (normalized?).

minscale
:   The minimum scale of the model from its original size of 1.0.

maxscale
:   The maximum scale of the model from its original size of 1.0.

minangle
:   The model\'s minimum angle of rotation.

maxangle
:   The model\'s maximum angle of rotation.

oriented
:   This is a flag, either 0 or 1, and sets whether the model gets
    fitted to the orientation of the surface.

## q3map_tcGen func {#q3map_tcGen}

This currently supports two functions, vector and ivector. Both
functions are used for texture projection and do the exact same thing.
The only difference is in the math, ivector was designed to be more
intuitive.

### q3map_tcGen vector sVector tVector

Projects a texture Ns units by Nt units along a chosen axis. q3map_tcGen
vector ( 1/256 0 0 ) ( 0 1/256 0 ) will project a texture every 256
units in x, and every 256 units in y, along the z-axis.

### q3map_tcGen ivector 1.0/sVector 1.0/tVector

Projects a texture Ns units by Nt units along a chosen axis. q3map_tcGen
ivector ( 256 0 0 ) ( 0 256 0 ) will project a texture every 256 units
in x, and every 256 units in y, along the z-axis. Ivector means inverse
vector, and this means you won\'t have to do the divide with a
calculator. Inverse = 1.0/n, unless the value is 0, then the matrix
value is set to 0. A bit of dodgy math, but it works.

## q3map_tcMod func {#q3map_tcMod}

This works in a similar manner to the stage specific tcMod directive
(see Stage Directives: [tcMod](#tcMod)), except in
the compiler, so that modified texture coordinates are \"baked\" into
the surface. This lets you set up less obvious texture tiling on natural
and organic surfaces (especially terrain).

### q3map_tcMod rotate degrees

Rotates the texture (around origin, not center) a specified number of
degrees.

### q3map_tcMod scale s-scale t-scale

Scales S (x) and T (y) texture co-ordinates. scale 2 2 would halve the
size of the texture (doubling the texture co-ordinates).

### q3map_tcMod translate [or]{.small} offset [or]{.small} shift s-offset t-offset

Shifts texture co-ordinates by S, T amount. translate 0.5 0 would shift
it one-half in S, and none in T.

## q3map_terrain

Alphamap terrain shaders (typically textures/common/terrain and
terrain2) must have the q3map_terrain directive. Terrain is handled
completely differently from previous versions. Q3Map2 no longer looks
for the word terrain in the shader name to determine whether or not it
is an indexed shader. It looks for
[q3map_indexed](#q3map_indexed), or
q3map_terrain, which then sets off a bunch of stuff shoehorned into it,
like: the lightmap axis, texture projection, etc.

By default, q3map_terrain sets the following:

    // texture width/height are grabbed from "q3map_textureSize" if it's set,
    // otherwise the actual texture dimensions are used
    q3map_tcGen ivector ( [ textureWidth / 2 ] 0 0 ) ( 0 [ textureHeight / 2 ] 0 )
    q3map_nonplanar
    q3map_shadeAngle 179
    q3map_noTJunc
    q3map_noClip
    q3map_forceMeta
    q3map_indexed

## q3map_tessSize amount {#q3map_tessSize}

This enables surface tessellation and controls its size (how finely a
surface is chopped up in to triangles), in game units. This is only
applicable to brushes, not curves, and is generally only used on
surfaces that are flagged with the
[deformVertexes](#deformVertexes) directive.
Abuse of this can create a huge number of triangles. The poorly named
*tessSize* directive still works but has been deprecated in favour of
q3map_tessSize for consistency.

## q3map_textureSize X Y {#q3map_textureSize}

Deprecated. Useful when you don\'t specify an editor or light image for
a shader. Recent builds of Q3Map2 will find some referenced image in a
shader and use that as a fallback. Historically, this was used for
surface splitting for RTCW for PS2, to accommodate the hardware\'s
limited texture range precision.

## q3map_vertexScale N.N {#q3map_vertexScale}

The light value, at the vertices of a surface using a shader with this
option, is multiplied by the scale value. This is a way to lighten, or
darken, a vertex_lit surface, in comparison to other lightmap_lit
surfaces around it.

## q3map_vertexShadows {#q3map_vertexShadows}

Obsolete! (See
[q3map_noVertexShadows](#q3map_noVertexShadows))
By default, no shadows are cast on vertex_lit_surfaces (see surfaceparm
pointlight). In addition, when running Quake III Arena in vertex light,
no shadows are cast upon any surface at all, since shadows are part of
the lightmap. When using this shader directive, shadows will be cast on
surfaces that are vertex lit. However, sharp shadow edges won\'t be seen
on the surfaces, since light values are only calculated at the vertices.
