Welcome to the OpenGL_Engine!

I've been making this game engine for a (year * 1.2f).

![Screenshot_7](https://user-images.githubusercontent.com/48869588/112764134-ecff5e00-900f-11eb-9f7b-081ee5e2eb25.png)
![image](https://user-images.githubusercontent.com/48869588/112905075-8141f180-90f2-11eb-9d49-136ede113d1e.png)
![Screenshot_14](https://user-images.githubusercontent.com/48869588/123415039-58c87700-d5bd-11eb-8864-4337cd978f28.png)

And now engine has:

Simple sprite animations

Collision2D component SAT algorithm

Component's manager

Poor material component for 3D (Ambient and Specular, Normal maps)

Can draw lines and squares and 3d meshes

Forward, Instancing renderers for 3D and Batch renderer for 2D

Text rendering!

Event system

Simple particle system

Multithreading (simple job system)

Editor with ImGui

2D textures, skyboxes and texture atlases

Ray casting in 2D and 3D

File dialogs

Point, Spot and Directional lights

Directional light shadows

Bloom effect

Audio (OpenAL)

Custom UI(buttons, panels, text, sliders, bars, checkboxes)


Show case: https://www.youtube.com/watch?v=BLJPgcl5DrY (outdated)

No memory leak in runtime
Engine is build as dll and uses a premake to compose it all together automatically. Engine has a class EntryPoint that takes as argument class Application that has OnStart, OnUpdate, OnClose, OnGuiRender methods and here the app must be written in a new project such as SandBox in this repository. There is a lot of junk code.
