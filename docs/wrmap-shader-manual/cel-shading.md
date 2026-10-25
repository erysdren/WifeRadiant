# Cel Shading

Cel shading is the technique used to make your maps render like
cartoons, with black (or other color) solid outlines tracing hard edges.

In order for celshading to work properly, you must use -meta when
compiling the BSP. The funny little nonplanar/shadeangle/invert/offset
nonsense in cel.shader is what makes it work properly. You\'ll know when
it\'s working when it starts taking about 10-20x as long to make the
BSP.

    // ink shader for maps
    // to use, add "cel" to shaderlist.txt
    // add a "_celshader" key to worldspawn entity with a value of "cel/ink"

    textures/cel/ink
    {
        qer_editorimage gfx/colors/black.tga

        q3map_notjunc
        q3map_nonplanar
        q3map_bounce 0.0
        q3map_shadeangle 120
        q3map_texturesize 1 1
        q3map_invert                //inverts drawing surface for backfacing hull
        q3map_offset -2.0           //offsets surface by the specified distance

        surfaceparm nolightmap
        surfaceparm trans
        surfaceparm nonsolid
        surfaceparm nomarks

        sort 16

        {
            map gfx/colors/black.tga
            rgbGen identity
        }
    }
