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
  m4player.c
)

# sokol_audio linked libraries
if (CMAKE_HOST_LINUX AND NOT MINGW)
  target_link_libraries(m4player asound m)
elseif (MINGW)
  set(CMAKE_EXE_LINKER_FLAGS "-static -mconsole")
  target_link_libraries(m4player ole32)
endif()
