# Triggerable Shader Entities

The targetShaderName and targetShaderNewName keys can be used with any entity that supports the target key (the entity instance does not actually have to use the target key for these new keys to work). If both are defined, then when the entity decides to activate its targets, all shaders/textures in the map that were originally the same name as the targetShaderName value, will be changed to the targetShaderNewName value. For example this would make it look like the red light shader is "turning on":

```
"targetShaderName" "textures/proto2/redlight_off"
"targetShaderNewName" "textures/proto2/redlight_on"
```

And this would turn it back off:

```
"targetShaderName" "textures/proto2/redlight_off"
"targetShaderNewName" "textures/proto2/redlight_off"
```

Note that the ORIGINAL shader name is used in both instances, not whatever it happens to be currently. Also, of course, this will happen globally. If the mapper wanted to affect only a certain set of red lights, he/she would need to make a unique shader name to be used with that set.

The code that supports these keys is in G\_UseTargets in g\_utils.c

\-TTimo, 31.08.01
