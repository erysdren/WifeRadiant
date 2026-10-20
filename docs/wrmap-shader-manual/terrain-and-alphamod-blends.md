# Terrain

Q3Map2 terrain has changed radically since Quake III: Team Arena first hit the shelves. Alphamapped terrain shaders are difficult to set up, limiting and lacks the ability for the mapper to fine tune the effect. The new terrain improvements in Q3Map2 have removed these difficulties, allowing the mapper to fully exploit terrain shaders for breathtaking outdoor scenes. This section will assume that you are already familiar with the method of setting up a standard alphamapped terrain as described in the Terrain Construction for Quake 3 Engine Games manual by Paul Jaquays, and will focus on the new enhanced features of Q3Map2 terrain.

## General Changes

Q3Map2 terrain has changed dramatically in terms of not only visually, but also in terms of how it is processed by the compiler. To ensure compatibility, there needs to be a few changes. Foremost, make sure that all terrain shader files are listed in the shaderlist.txt file.

Before using Q3Map2 alphamapped terrain, it must be noted that terrain is now handled completely different than before. Q3Map2 no longer looks for the word terrain in the shader name to determine whether or not it is an indexed shader. Instead it looks for [q3map\_indexed](#q3map_indexed) or [q3map\_terrain](#q3map_terrain). For this reason, the alphamap terrain shaders have been slightly modified. If using Q3Map2 alphamaps, make sure that your "textures/common/terrain" and "textures/common/terrain2" shaders have been updated to the following:

```
textures/common/terrain
{
	q3map_terrain
	surfaceparm nodraw
	surfaceparm nolightmap
	surfaceparm nomarks
}

textures/common/terrain2
{
	q3map_terrain
	qer_editorimage textures/common/terrain.tga
	surfaceparm dust
	surfaceparm nodraw
	surfaceparm nomarks
	surfaceparm nolightmap
}
```

### Base Shader

To simplify the terrain shader writing process, a base shader can be used as a template with subclassed shaders referencing it. This makes use of the [q3map\_baseShader](#q3map_baseShader) directive. The base shader can consist of all q3map\_\* directives and must be processed before all other terrain shaders referencing it. The use of a base shader is recommend for all terrain shaders.

### tcMod Functions

[q3map\_tcMod](#q3map_tcMod) can be used on terrain shaders to minimize any obvious texture tiling, particularly q3map\_tcMod rotate. In some situations q3map\_tcMod scale or the stage driven [tcMod scale](#tcModScale) may be used to scale the textures - though typically [tcGen texture projection](#tcGenVector) functions are usually the preferred method.

### tcGen Functions

In most situations, terrain textures will need to be projected onto the terrain mesh. This is achieved by using either [q3map\_tcGen](#q3map_tcGen) function or the stage specific [tcGen](#tcGenVector). Any tcGen functions will overwrite any [tcMod scale](#tcModScale) functions.

## Lightmapped Terrain

The foremost improvement to Q3Map2 terrain is the ability to use lightmaps as a lighting system rather than basic vertex lighting. This allows terrain to not only cast detailed shadows onto itself, but also allows other map geometry to cast shadows onto the terrain as well. The additional rendering pass of the lightmap creates a slight performance hit as a price for the improved visuals. Several new q3map\_\* directives are typically used to tweak the terrain lightmap:

### [q3map\_nonPlanar](#q3map_nonPlanar)

Required on all terrain shaders, q3map\_nonPlanar allows shadows to be cast across non-planar edges. This fixes a problem where lightmapped terrain would not light properly across uneven surfaces.

### [q3map\_lightmapAxis](#q3map_lightmapAxis)

Optionally used, q3map\_lightmapAxis can be used to specify the axis in which the lightmap is projected from. This is typically used on normal triangle-quad souped terrain and is set implicitly to "q3map\_lightmapAxis z" with [q3map\_terrain](#q3map_terrain). It is not recommended for terrain with caves or undersides.

### [q3map\_shadeAngle](#q3map_shadeAngle)

Most terrain artists will choose to enable triangle edge shadow smoothing to reduce the appearance of sharp shadow edges across the terrain mesh. q3map\_shadeAngle specifies the triangle edge angle at which the light will be diffused. For best results, it is recommended that you start with low values and tweak the angle parameter in small increments until a satisfactory result is produced. Overly high values will wash out shadow details.

### [q3map\_lightmapMergable](#q3map_lightmapMergable)

Setting q3map\_lightmapMergable merges all terrain into one seamless lightmap, reducing the appearance of artifacts spanning across separate lightmap images.

### [q3map\_lightmapSampleSize](#q3map_lightmapSampleSize)

Lightmapped terrain can be very memory intensive as Q3Map2 has to typically generate a large amount of lightmap data. q3map\_lightmapSampleSize can be used to reduce the amount of lightmap memory used by limiting the resolution of the lightmap images. Similarly, it can also be used to increase the lightmap resolution at the cost of memory.

### [q3map\_lightmapSampleOffset](#q3map_lightmapSampleOffset)

If experiencing lightmap splotches over the surface of terrain lightmaps, q3map\_lightmapSampleOffset can be used to fix this. For best results, start with small numbers (default 1.0) and slowly increase this value until the splotches disappear. Overly high values will cause unsatisfactory results.

## AlphaMod Dotproduct Terrain

[AlphaMod dotproduct](#q3map_alphaMod) terrain is a revolutionary way in which Q3Map2 terrain is blended. It removes the need for an alphamap and metashader and automatically determines the blending of terrain depending on the vertex normals of the terrain mesh. Equally important, this new system removes the need of a triangle-quad grid, allowing the designer to create a terrain mesh using any triangle shape or size.

## AlphaMod Volume Terrain

Following the automatic generation of dotproduct blending, [alphaMod volume](#q3map_alphaModVolume) terrain blending was developed to give direct control of the alpha blending back to the designer. Using this system the designer can modify the exact location and amount of blending to occur, directly from within the editor.
