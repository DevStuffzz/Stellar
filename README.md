# Stellar
A 2D Game Engine written in C++

## Building (Windows)
1. Clone with submodules: `git clone --recursive https://github.com/DevStuffzz/Stellar` (or `git submodule update --init --recursive`).
2. Run `Win-GenProjects.bat` to generate the Visual Studio 2022 solution (uses the bundled `Vendor/bin/premake/premake5.exe`).
3. Open `Stellar.sln` and build/run the `Sandbox` project. Post-build steps copy `Stellar.dll` and `Sandbox/assets` next to `Sandbox.exe`, which is also the debugger working directory, so load assets with paths like `assets/...`.

## Layout
- `Stellar/src/Stellar` - platform-agnostic engine API (pure virtual `Window`, `Input`, `GraphicsContext`, `RendererAPI`, `VertexBuffer`, `IndexBuffer`, `VertexArray`, `Shader`) plus `Mesh`, `OrthographicCamera`, and `Renderer`.
- `Stellar/src/Platform/Windows` - `WindowsWindow` and `WindowsInput` (GLFW).
- `Stellar/src/Platform/OpenGL` - OpenGL implementations of the renderer interfaces.
- `Sandbox` - example client application (WASD/QE moves/rotates the quad, arrow keys pan the camera, hold LMB to tint, Esc quits).
