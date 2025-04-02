##########################################
# m4player
##########################################
cmake_minimum_required(VERSION 3.25)

project(
  m4player
  LANGUAGES C
)

add_executable(
  m4player
  m4player-sapp.c
)

# sokol_app/sokol_audio linked libraries
if (WIN32 OR MINGW)
  target_compile_definitions (m4player PUBLIC SOKOL_D3D11)
  target_link_libraries(m4player d3d11 winmm kernel32)
elseif (CMAKE_HOST_LINUX)
  target_compile_definitions (m4player PUBLIC SOKOL_GLCORE33)
  target_link_libraries(m4player asound X11 Xi Xcursor GL dl pthread m)
endif()
