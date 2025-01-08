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

add_library(
  m4p
  ../m4p.c
)

set(M4PLAYER_LINK_LIBRARIES m4p)

# sokol_app/sokol_audio linked libraries
if (CMAKE_HOST_LINUX)
  set(M4PLAYER_LINK_LIBRARIES ${M4PLAYER_LINK_LIBRARIES} asound X11 Xi Xcursor GL dl pthread m)
endif()

target_link_libraries(m4player ${M4PLAYER_LINK_LIBRARIES}) 

add_custom_command(
  TARGET m4player
  POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:m4player>"
          "${CMAKE_CURRENT_LIST_DIR}"
)
