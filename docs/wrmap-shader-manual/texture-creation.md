# Texture Creation

If you are familiar with the required tools, creating new assets for use
in Quake III Arena is not particularly difficult. As a general rule, you
should create new directories for each map with names different from the
names used by id. If you are making a map that will be called
\"H4x0r_D00M\", every directory containing new assets for that map
should be titled H4x0r_D00M. This is to try and avoid asset directories
overwriting each other as the editor and the gameload in assets.

## Tools Needed

Any combination of graphic programs and plug-ins that can output a 24
bit MS windows compatible Targa (.tga) or JPEG (.jpg) graphic file.If
you plan to make textures that will have an alpha channel component (a
4th 8-bit greyscale channel that is used by the shaders to further
manipulate the art), you must have a program that can create 32-bit art
with that fourth channel.

Adobe Photoshop has the ability to easily create alpha channels.
PaintShop Pro from Corel (v5.0+) can also make an alpha channel by
creating a mask and naming it \"alpha\". Free and open source GIMP also
works quite well.

Generally speaking, regardless of the program used, we found it best to
do most of the art manipulation of the alpha channel in a separate layer
or file and then paste it into the alpha channel before saving.

## Setting up Files

The editor and the game program look for assets to be located along the
paths set up in your project file. Start by creating a directory for you
new textures by creating file folders to make a directory path as
follows:

    quake3\baseq3\textures\[mymapname]

The installation of Q3Radiant will create a text document called
\"shaderlist.txt\" in the following path:

    quake3\baseq3\scripts\shaderlist.txt

This is whitelist for Q3Radiant and Q3Map2 to know which shader scripts
to load (the rest will be skipped).

Since you will obviously want to create your own shaders, you need to
put them in separate folders and create a new shader script for them.

If you plan to work on several maps at once and want to distinguish
between textures used in each map, simply add additional map names here.
For map and mod makers, we STRONGLY recommend that any new shader
scripts created use the name of the map or mod in the shader file name.
We know we can\'t avoid every incident of files overwriting each other,
but we certainly can advise you how to try.

Now, in the scripts directory that you just created, create another text
file and call it:

    [mymapname].shader

This file will contain the shader scripts you write to modify a
particular texture.

## Rules and Guidelines

Follow these rules when creating textures for the Quake III Arena
engine:

- Save your textures into your new \[map name\] directories.
- Don\'t use the same names that id used for textures. It will cause
  problems.
- For best quality, save textures without an alpha channel as 24 bit
  TARGA files. Using JPEG files can save memory space, but at the risk
  of losing detail and depth in the texture. JPEG files cannot be used
  for textures requiring an alpha channel.
- Textures containing an alpha channel must be saved as 32 bit TARGA
  files.
- Indexed TARGA is not supported!
- Progressive JPEG is not supported!
- If a new texture requires no further manipulation, it does not need a
  shader script.
- Size textures in powers of 2. Example: 8x8, 16x16,32x32, 64x64 pixels
  and so on.
- Textures don\'t need to be square. A 32x256 pixel texture is perfectly
  acceptable.

The following are some things the id designers learned about textures.

- Create textures in \"suites\" built around one or two large textures
  with a number of much smaller supporting detail or accent textures.
- Textures are grouped alphabetically by name in the texture display
  window, so you may want to give suites of textures similar names.
- Use the shader function
  [qer_editorImage](#qer_editorImage)
  to conserve memory when making multiple versions of a single texture
  (as in the case of a glowing texture with several light values).
- Unless you are creating special effects or textures designed to draw
  the player\'s eye to a specific spot, muted, middle value colors work
  best with the game engine.
- Extremely busy (a lot of fussy detail) textures can break up or form
  visually unpleasant patterns when seen at distances.

## Making the .pk3 File

When you go to distribute your creation to the gaming world, you need to
put your newly created map, textures, bot area files, and shader
documents into an archive format called a \"pk3\" file. You do not need
to include the shaderlist.txt file, since that is only used by the
editor. You will need to keep the paths to the various assets the same.
So your paths should be something like this:

    Textures: baseq3/textures/[mymapnamefolder]
    Bsp & aas: baseq3/maps/mymapname.bsp, mymapname.aas
    Shader scripts: baseq3/scripts/mymapname.shader

You need to use a \"zip\" archiving program
([7-zip](http://www.7-zip.org/){tppabs="http://www.7-zip.org/"} for
example) to make the pk3 file. Make a zip archive called mymapname.zip.
Zip all the required assets into a zip archive file (Quake III Arena
DOES support compressed pk3 files). Rename the zip archive to
mymapname.pk3. Put it where the Quake III Arena community can find it.
