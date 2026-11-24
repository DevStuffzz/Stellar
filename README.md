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
- `Sandbox` - example client application with a circular player-following minimap (WASD moves the player, mouse wheel zooms the main camera, Esc quits).

Tilemap collision is opt-in: mark solid atlas IDs with `SetTileCollision(id, true)` and add a `TilemapColliderComponent` to a moving entity. Collision uses an axis-aligned box and an unrotated, unscaled map centered at `MapCenter`; outside the map is non-solid.

To render a tilemap, add `TilemapRendererComponent` to an entity and set its `Map`, textured `Atlas`, and `TileSize`. The scene culls against the primary camera and renders tilemaps and sprites in entity creation order (a tilemap is drawn before a sprite on the same entity). Visible tile edits, including edits through `GetData()`, automatically refresh the cached mesh. The rendering transform does not change `TilemapColliderComponent`'s independently configured collision center or size.

An `OverlayCameraComponent` captures a scene into an opaque texture and optionally draws it over the main view:

```cpp
auto cameraEntity = scene->CreateEntity("Minimap");
auto& overlay = cameraEntity.AddComponent<Stellar::OverlayCameraComponent>();
overlay.Zoom = 8.0f;
overlay.Resolution = { 256, 256 };
overlay.Position = { 0.75f, 0.05f };
overlay.Size = { 0.2f, 0.2f };
overlay.OverlayShape = Stellar::OverlayCameraComponent::Shape::Circle;
cameraEntity.AddComponent<Stellar::CameraFollowComponent>().Target = player;
```

`Position` is the top-left corner and `Size` the width and height in normalized screen coordinates. A circle is centered within that rectangle and uses its smaller pixel dimension as the diameter. The capture uses the camera entity's transform and resolution, not the screen rectangle; `FixedAspectRatio` makes its projection square. `ShowOverlay = false` keeps capturing without drawing on screen, and `Enabled = false` disables both. `GetTexture()` returns the capture after `Scene::OnRender()` (or null before the first capture); reacquire it after changing `Resolution`.

The capture defaults to the camera's own scene. Call `overlay.SetScene(otherScene)` to capture a different `Ref<Scene>`, or `overlay.SetScene()` to restore the default. The other scene must remain alive and be updated separately. A camera follow target must belong to the camera's own scene. Captures contain world sprites and tilemaps but not screen-space overlays; a world drawable sampling the texture currently being captured is omitted from that capture to avoid framebuffer feedback.

Native scripts derive from `ScriptableEntity` and are attached with `entity.AddComponent<NativeScriptComponent>().Bind<MyScript>()`. The scene calls `OnCreate` once before the first `OnUpdate`, updates scripts before movement, and calls `OnDestroy` when a script is removed, its entity is destroyed, or the scene is destroyed. Use `Entity::RemoveComponent` and `Scene::DestroyEntity` rather than the registry directly so script cleanup runs; removals requested from script callbacks are deferred until the callback returns. `OnFixedUpdate` is not scheduled yet.
