#pragma once

// Public header for client applications. Include EntryPoint.h in exactly one translation unit.

#include "Core.h"

#include "Stellar/Application.h"
#include "Stellar/Window.h"
#include "Stellar/Timestep.h"

#include "Stellar/Input.h"
#include "Stellar/KeyCodes.h"

#include "Stellar/Renderer/Renderer.h"
#include "Stellar/Renderer/RenderCommand.h"
#include "Stellar/Renderer/RenderTarget.h"
#include "Stellar/Renderer/Mesh.h"
#include "Stellar/Renderer/Shader.h"
#include "Stellar/Renderer/Texture.h"
#include "Stellar/Renderer/TextureAtlas.h"
#include "Stellar/Renderer/SpriteAnimation.h"
#include "Stellar/Renderer/Buffer.h"
#include "Stellar/Renderer/VertexArray.h"
#include "Stellar/Renderer/OrthographicCamera.h"

#include "Stellar/Scene/Scene.h"
#include "Stellar/Scene/Entity.h"
#include "Stellar/Scene/Components.h"
#include "Stellar/Scene/Tilemap.h"

#include "Stellar/Util/Util.h"
#include "Stellar/Util/FileUtil.h"

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <random>