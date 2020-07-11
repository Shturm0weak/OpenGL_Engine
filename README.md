Welcome to the OpenGL_Engine!

I've been making this game engine for a (year * 0.8f).

And now engine has:

Simple sprite animation component

Collision component SAT algorithm

Transform component

Component's manager

Pour material component for 3D

Can draw lines and squares and 3d meshes

Batch and Forward renderer

Text rendering

Event system

Simple particle system

Multithreading

Editor with ImGui

2D textures and texture atlases

Ray casting in 2D and 3D

Audio

Custom UI(buttons,panels,text,sliders,bars,checkboxes)


No memory leak in runtime
Engine is build as dll and uses a premake to compose it all together automatically. Engine has a class EntryPoint that takes as argument class Application that has OnStart,OnUpdate,OnClose methods and here the app must be written in a new project such as SandBox in this repository. There is a lot of junk code.
