##########################################
# m4player
##########################################

project(
  m4player
  LANGUAGES C
)

add_executable(
  m4player
  it2drivers/sb16_m.c
  it2drivers/sb16.c
  it2drivers/zerovol.c
  loaders/mmcmp/mmcmp.c
  loaders/it.c
  loaders/s3m.c
  m4p.c
  ft_tables.c
  it_d_rm.c
  it_m_eff.c
  it_music.c
  it_tables.c
  pmp_main.c
  pmp_mix.c
  pmplay.c
  snd_masm.c
  example/m4player-sapp.c
)

add_custom_command(
  TARGET m4player
  POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:m4player>"
          "${CMAKE_CURRENT_LIST_DIR}"
)