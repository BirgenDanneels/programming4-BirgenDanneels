set(ENGINE_TARGET MiniginEngine)

add_library(${ENGINE_TARGET} STATIC
  Font.cpp
  GameObject.cpp
  InputManager.cpp
  Minigin.cpp
  Renderer.cpp
  ResourceManager.cpp
  Scene.cpp
  SceneManager.cpp
  Texture2D.cpp
  Transform.cpp
  Events/GameEventQueue.cpp
  Loading/SceneLoader.cpp
  Input/InputDevice.cpp
  Input/Keyboard.cpp
  Input/InputMap.cpp
  Input/Gamepad.cpp
  Components/Component.cpp
  Components/TextureComponent.cpp
  Components/TextComponent.cpp
  Components/CharacterController.cpp
)

target_compile_features(${ENGINE_TARGET} PUBLIC cxx_std_20)

if(Steamworks_FOUND)
  target_sources(${ENGINE_TARGET} PRIVATE
    Components/AchievementSystem.cpp
    Steam/SteamAchievements.cpp
  )
endif()

target_include_directories(${ENGINE_TARGET} PUBLIC
  ${CMAKE_SOURCE_DIR}
)

target_include_directories(${ENGINE_TARGET} PRIVATE
  ${CMAKE_SOURCE_DIR}/Minigin
  ${imgui_SOURCE_DIR}
  ${imgui_plot_SOURCE_DIR}
  ${VLD_INCLUDE_DIR}
)

target_link_libraries(${ENGINE_TARGET} PUBLIC
  SDL3::SDL3
  SDL3_ttf::SDL3_ttf
  glm::glm
  imgui
  imgui_plot
  nlohmann_json::nlohmann_json
)

if (WIN32)
  target_link_libraries(${ENGINE_TARGET} PRIVATE xinput)
  if(VLD_FOUND)
    target_link_libraries(${ENGINE_TARGET} PUBLIC ${VLD_LIBRARIES})
  endif()
endif()

if(MSVC)
  target_compile_options(${ENGINE_TARGET} PRIVATE /W4 /WX)
else()
  target_compile_options(${ENGINE_TARGET} PRIVATE -Wall -Wextra -Werror)
endif()
