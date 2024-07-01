# add your own executable
add_executable(main
        ${CMAKE_CURRENT_LIST_DIR}/Main.c)
target_link_libraries(main
        pico_stdlib
        hardware_spi
        enV5_hw_configuration
        enV5_hw_controller
        stub_v2_lib
        flash_lib
)
create_enV5_executable(main)
