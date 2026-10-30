\pagebreak
# Foghull

Most recent Quake III engine games are programmed with a feature known
as distance clipping used to clip (cull) map geometry beyond a user set
distance. At the time of Quake III Arena\'s release, distance clipping
was not an available feature. The Q3Map2 foghull feature was designed to
simulate true distance clipping for Quake III Arena and Team Arena
games.

## What is a Foghull?

Farplane distance clipping is a feature used to cull (remove) the
drawing of polygons beyond a certain distance from the player in an
attempt to improve performance. It is typically used on large, open
terrain maps with little vis-blocking structures. Through the use of
distance clipping a maximum vis distance is set, which provides the
culling of rendered polygons. Fog is used to hide the effect of polygons
appearing and disappearing by obscuring the maximum distance that the
player can see.

Since distance clipping is not a feature natively built into Quake III
Arena, using distance culling would result in a hall of mirrors (HOM)
effect where the culled geometry begins, since nothing is being drawn in
the frame buffer. To compensate for this, the foghull feature uses a
series of six skybox images that are drawn in place of the absent culled
geometry, thus preventing the HOM effect.

## Skybox Images

The skybox images used with the foghull feature should never be actually
seen since the idea is to use the fog to obscure the maximum distance
that the player can see. It only exists to prevent the HOM effect. To
pull this off in a convincing manner, the skybox images should be six
identical 8x8 pixel (to save on texture memory) textures each filled
with a flat color matching the exact color of the fog. The six skybox
images must be named in accordance to the
[skyParms](#skyParms) farbox convention, using
the \_ft, \_rt, \_bk, \_lf, \_up, \_dn suffixes.

## Foghull Shaders

Two shaders are required when using the foghull feature, a fog volume
shader and a skybox shader, both of which are simple, standard shaders.

The color of the fog used must match the color used in the skybox
images. Any decent image editing software will give you the three RGB
color values of your skybox images, which must be normalized by dividing
by 255. As with any other fog shader, the
[fogParms](#fogParms) and [surfaceparm
fog](#fog) keywords must be
present.

    textures/env/fog1024
    {
        //Normalized RGB, distance to opaque
        fogParms ( 0.8 0.8 0.8 ) 1024

        surfaceparm fog    //Must be used
        surfaceparm nolightmap
        surfaceparm nonsolid
        surfaceparm trans

        qer_editorImage textures/sfx/fog_grey.tga
        qer_trans 0.4
    }

The [skyParms](#skyParms) and [surfaceparm
sky](#sky) keywords must be used
to create the skybox \"hull\" of the map. The
[skyParms](#skyParms) farbox value must point to
the base name of the skybox images (sans suffix). Optional surface
emitted sun lighting can be added using
[q3map_skylight](#q3map_skylight) with
[q3map_sun](#q3map_sun) or
[q3map_sunExt](#q3map_sunExt) (see Appendix
I: [Light Emitting Shaders](#light-emitting-shaders)).

    textures/skies/foghullsky
    {
        //farbox cloudheight nearbox
        skyParms textures/skies/foghullsky 0 -

        surfaceparm nolightmap
        surfaceparm nonsolid
        surfaceparm sky        //Must be used
        surfaceparm trans
    }

## Entity Key Value Pairs

To activate the foghull feature, the \_foghull and \_farplanedist
worldspawn entity key/value pairs must be set in the Entity Inspector
(in Radiant, select any non-entity brush and press \"N\").

The \_foghull key\'s value works similar to that of terrain entities. It
must point to the name of the skybox shader, minus the standard
\"textures/\" prefix. For the skybox shader example above with the
shader name \"textures/skies/foghullsky\", you would use a \_foghull
value of \"skies/foghullsky\".

The \_farplanedist value is simply the distance at which polygons will
begin to get culled. One important note is that the \_farplanedist value
must be greater than the distance to opaque value set in the fog shader.
In the above fog shader, the distance to opaque value was set to 1024
game units. The \_farplanedist value must therefore be greater than
1024, otherwise the HOM effect will appear.
