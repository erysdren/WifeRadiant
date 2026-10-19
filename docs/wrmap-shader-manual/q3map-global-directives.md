# Q3Map Global Directives

Q3Map2 global directives change the physical nature of the textures and the brushes that are marked with them. Changing any of these values will require the map to be re-compiled. These are global and affect the entire shader.

## light value

Tells compiler to generate flare surface with default game flare shader. Value is not used, but must be present.

## q3map_alphaGen func

Currently takes a single function, const. This directive is used to set a surfaces vertex alpha values. q3map_alphaGen operations are applied to an object's vertexes so the alphaGen vertex directive is required for each affected stage.

### q3map_alphaGen const N.N

Forces a fixed vertex alpha value to the entire shader surface, useful for controlling transparency or blending. Value is a [normalized number](#normalization-a-scale-of-0-to-1).

## q3map_alphaMod func

This is used for special vertex alpha blending effects on surfaces by altering the vertex alpha values depending on specific surface properties, such as the surfaces normal axis or the vertexes contained within its volume. q3map_alphaMod operations are applied to an object's vertexes so the alphaGen vertex directive is required for each affected stage.

### q3map_alphaMod dotproduct ( X Y Z )

Used to blend textures using [alphaFunc](#alphaFunc) or [blendFunc](#blendFunc) in the shader's second pass, with the transparency depending on the surface's normal axis vector. This is achieved by taking the user specified vector ( X Y Z ) and applying a dotproduct calculation with the generated vertex normal vector to yield a normalized vertex alpha value. The dot product operation multiplies each element of one vector against the corresponding elements of a second vector, then adds them:

```
( 0 0 1 ) dp ( 0 0 1 ) = 0 * 0 + 0 * 0 + 1 * 1 = 1
( 0 0 1 ) dp ( 0 0.7 0.7 ) = 0 * 0 + 0 * 0.7 + 1 * 0.7 = 0.7
( 0.5 -0.5 0.7 ) dp ( 0 0.7 0.7 ) = 0.5 * 0 - 0.5 * 0.7 + 0.7 * 0.7 = 0.14
```

In a practical sense, you can think of the dotproduct vector ( 0 0 0.9 ) as meaning that all vertices with normal vector pointing straight up along the z-axis ( 0 0 1 ) will have a vertex alpha value of 90% opacity, entirely horizontal ( X Y 0 ) will get 0%, and everything in between will be mapped linearly.

### q3map_alphaMod dotproduct2 ( X Y Z )

This works in a similar way to dotproduct except it exaggerates the differences in vertex normals by squaring the final dot product value. With the same values as the above example, dotproduct2 would give the following:

```
[ ( 0 0 1 ) dp ( 0 0 1 ) ]2 = ( 0 * 0 + 0 * 0 + 1 * 1 )2 = 1
[ ( 0 0 1 ) dp ( 0 0.7 0.7 ) ]2 = ( 0 * 0 + 0 * 0.7 + 1 * 0.7 )2 = 0.49
[ ( 0.5 -0.5 0.7 ) dp ( 0 0.7 0.7 ) ]2 = ( 0.5 * 0 - 0.5 * 0.7 + 0.7 * 0.7 )2 = 0.0196
```

Example q3map_dotproduct2 shader for terrain:

```
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
```

### q3map_alphaMod dotproductScale ( X Y Z MIN MAX )

Works similar to q3map_alphaMod dotproduct, but in addition values MIN and MAX denote a range of cosine values, in which blending will occur. So less than MIN is mapped to alpha value 0, above MAX is mapped to alpha value 1, and everything in between is mapped linearly.

If you prefer working with angles, you can set MIN to the cosine of the most steep angle of the blending, and MAX to the cosine of the most flat angle of the blending. Example:

```
q3map_alphagen dotProductScale ( 0 0 1 0.500 0.966 )
```

will set alpha value 0 for anything steeper than 60 degrees, and alpha value 1 for anything more flat than 15 degrees.

### q3map_alphaMod dotproduct2scale ( X Y Z MIN MAX )

The same extension to dotProduct2. Note that the MIN and MAX values are squared cosine here. You get a different alpha values curve with the same min/max angles by writing:

```
q3map_alphagen dotProduct2scale ( 0 0 1 0.250 0.933 )
```

> #### Design Notes:
> Typical examples of use include snow covering the top faces of objects, or terrain with grass growing on horizontal planes blending into rocky cliffs on near vertical surfaces. It is an excellent way of automatically creating realistic alpha-blended terrain without the complicated steps of setting up an alpha map.

### q3map_alphaMod scale N.N

Used in conjunction with q3map_alphaMod volume. Scales the vertex alpha by the specified [normalized number](#normalization-a-scale-of-0-to-1) value.

### q3map_alphaMod set N.N

Used in conjunction with q3map_alphaMod volume. Sets the vertex alpha (regardless of any previous alpha values) to the specified [normalized number](#normalization-a-scale-of-0-to-1) value.

### q3map_alphaMod volume

This was created as a way to explicitly set or modify the vertex alpha values of vertex points contained within a controlling brush volume marked with this shader directive. Applies all other q3map_alphaMod directives to each vertex inside a brush textured with this shader, allowing large faded scrolling fire shaders, waterfalls, marquees, explicit dotProduct terrain blending control, etc.

> Design Notes:
> This is usually used in special alphaMod volume "common" shaders for use within the editor only. A brush textured with the alphaMod volume shader is used to overlap the vertexes of another brush or model using an alpha-blended shader, altering the vertex alpha values. Worldspawn alphaMod volume brushes will affect all surfaces it comes in contact with. You can func_group an alphaMod volume brush to its affecting brush to localize the blend to just that entity.

## q3map_backShader shadername

This allows a brush to use a different shader when you are inside it looking out. By way of example, this would allow a water brush (or other) surfaces to have a different [sort](#sort-value) order or appearance when seen from the inside. This is essentially shorthand of [q3map_cloneShader](#q3map_cloneShader) where the target shader contains [q3map_invert](#q3map_invert).

## q3map_backSplash percentage distance

By default [surface light](#q3map_surfaceLight) generates extra 'backsplash' light to light itself and surrounding. That light source is moved away from the surface of the shader, allowing it to create smoother lighting over the face. By default, all shaders are assigned backsplash values, 5 for percentage, 23 units for distance.

percentage
: Specifies the intensity percentage of the light generated by [q3map_surfaceLight](#q3map_surfaceLight) to be redirected back at the surface. Use a value of 0 or a negative value to disable back splash lights.

distance
: Distance of the back splash lights from the surface.

## q3map_baseShader shadername

Allows shaders to be subclassed (Q3Map2 relevant portions only, such as surfaceparms, lighting, texture projection, etc). Subclassed shaders can reference the base shader by referring to the base shader's name. In order for q3map_baseShader to work correctly, the base shader must be specified before any shaders that subclass it. Some EasyGen terrain templates incorrectly specified the base shader after the terrain shaders that depended on it, resulting in some interesting errors.

This is fundamentally the reverse of [q3map_remapShader](#q3map_remapShader). Use q3map_baseShader when a single group of q3map_* directives is required for multiple base shaders. Use [q3map_remapShader](#q3map_remapShader) when similar shaders are used that require different groups of q3map_* directives.

## q3map_bounce N.N

Deprecated! Use q3map_bounceScale instead.

## q3map_bounceScale N.N

Takes positive decimal number values to scale the amount of light reflected in radiosity passes. You can oversaturate it by using a number higher than 1.0, but this can lead to excessive compile times. Using 0.9 would probably make things positively glacial. 1.0 is a default, fudged number that looked OK with the maps that were tested. Tweaking it to 1.5 or 2.0 won't hurt anything per se, but it does give you finer control over how each shader re-emits light. The poorly worded q3map_bounce has been renamed to q3map_bounceScale. While its use has been deprecated, q3map_bounce still works.

## q3map_clipModel

Automatically clips misc_model entities for player and weapon collision. This should only be used on large models such as terrain (not small decorative models - manually clip those). The shader's surfaceparms are inherited by the magic clip brush, so if you have [surfaceparm nonsolid](#surfaceparm-nonsolid) in your model's shader that uses q3map_clipModel, then the brush will also be non-solid. This can also be set on a per model basis with the entity key/value pair, spawnflags 2.

## q3map_cloneShader shadername

Clones surface and applies target shader's properties and appearance to clone. Be careful, this can lead to an infinite loop if a cloning shader references another cloning shader or itself.

## q3map_colorGen func

Currently takes a single function, const. This directive is used to set a surfaces vertex color values. q3map_colorGen operations are applied to an object's vertexes so the [rgbGen vertex](#rgbGen-vertex) directive is required for each affected stage. It's also needed to use [q3map_noVertexLight](q3map_noVertexLight) or -novertex to avoid overwriting RGB vertex values during -light build stage. Same as [q3map_rgbGen](#q3map_rgbGen).

### q3map_colorGen const ( R G B )

Forces a fixed vertex color value to the entire shader surface.

## q3map_colorMod func

Used to alter the vertex color values of vertexes contained within its volume. q3map_colorMod operations are applied to an object's vertexes so the [rgbGen vertex](#rgbGen-vertex) directive is required for each affected stage. It's also needed to use [q3map_noVertexLight](#q3map_noVertexLight) or -novertex to avoid overwriting RGB vertex values during -light build stage. Possible uses include creating a neutral hued texture for a CTF middleground and then creating shader instances of the texture for red and blue bases. Same as [q3map_rgbMod](#q3map_rgbMod).

### q3map_colorMod scale ( R G B )

Used in conjunction with q3map_colorMod volume. Scales the vertex color by the specified color values.

### q3map_colorMod set ( R G B )

Used in conjunction with q3map_colorMod volume. Sets the vertex color to the specified color values.

### q3map_colorMod volume

This was created as a way to explicitly set or modify the vertex color values of vertex points contained within a controlling brush volume marked with this shader directive. Applies all other q3map_colorMod directives to each vertex inside a brush textured with this shader to change the hue of the affected surface.

> #### Design Notes:
> This is usually used in special colorMod volume "common" shaders for use within the editor only. A brush textured with the colorMod volume shader is used to overlap the vertexes of another brush or model, altering the vertex color values. Worldspawn colorMod volume brushes will affect all surfaces it comes in contact with. You can func_group a colorMod volume brush to its affecting brush to localize the hue to just that entity.

### q3map_colorMod dotproduct ( X Y Z )

### q3map_colorMod dotproduct2 ( X Y Z )

### q3map_colorMod dotproductScale ( X Y Z MIN MAX )

### q3map_colorMod dotproduct2scale ( X Y Z MIN MAX )

These ones work just like [q3map_alphaMod dotproduct*](#q3map_alphaMod), but are modifying vertex color values.
