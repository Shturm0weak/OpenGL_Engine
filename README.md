Welcome to the OpenGL_Engine!

I've been making this game engine for a (year * 1.2f).

And now engine has:

Simple sprite animation component

Collision2D component SAT algorithm

Collision3D component AABB, ray to OBB , ray to triangle algorithms

Component's manager

Poor material component for 3D (Ambient and Specular, Normal maps)

Can draw lines and squares and 3d meshes

Forward,Instancing and Batch renderers

Text rendering

Event system

Simple sprite particle system

Multithreading

Editor with ImGui

2D textures and texture atlases

Ray casting in 2D and 3D

File dialogs

Point and Directional lights

Audio

Custom UI(buttons,panels,text,sliders,bars,checkboxes)


Show case: https://www.youtube.com/watch?v=BLJPgcl5DrY&t=189s

No memory leak in runtime
Engine is build as dll and uses a premake to compose it all together automatically. Engine has a class EntryPoint that takes as argument class Application that has OnStart,OnUpdate,OnClose methods and here the app must be written in a new project such as SandBox in this repository. There is a lot of junk code.
