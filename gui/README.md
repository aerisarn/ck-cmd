# Skyrim-Behavior-Editor-
![statemachine](https://user-images.githubusercontent.com/8378075/45464759-e8048880-b709-11e8-9448-0abd26b31944.png)

This tool is a GUI viewer/editor for Havok’s Behavior files utilized
by the Creation Engine powering the video game Skyrim. Havok Behavior
is used in this engine to select/manipulate actor animations in
the game.

Internally this tool uses hkxcmd to convert between packed binary
hkx and hkx xml. It parses the hkx xml files associated with a character
project, graphs all the data in each behavior file and exposes all
of the important data to the user via a number of different widgets.

<b>NOTE: As hkxcmd cannot work with SSE animation/behavior files, you will need to use the original Skyrim animation/behavior files for this tool to work!</b>

It also parses the animation text files that contain data representing
animation translation and rotation data along with information used
by the Creation Engine’s animation caching system.

Features of the tool include:

• Open existing projects and create new projects.

• Remove or add new animations to any project.

• Append, inject and remove objects from the behavior graph via
a type-safe interface.

• View and edit the data of over 70 different Havok and Bethesda
behavior classes.

• Extensive error checking and error correction routines.

• View, edit, remove or add translations and rotations to any animation
in the project.

• View and edit the animation cache data.

• Expand or collapse branchs while viewing a behavior graph.

• Pan the view around and zoom in or out while viewing a behavior
graph.

• Locate and focus on objects via a string filterable table of object
names.

• Convert and export project or behavior files to the game directory.

• Merge projects or single behavior files together.
