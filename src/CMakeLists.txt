# add your own executable
add_executable(main
        ${CMAKE_CURRENT_LIST_DIR}/Main.c)
target_link_libraries(main
        unity
        hardware_spi
        enV5_hw_configuration
        enV5_hw_controller
        stub_v2_lib
        pico_stdlib
        freeRtosUtils
        common_lib
        enV5_hwc
        network_lib
        espBroker_lib
        http_lib
        flash_lib
        fpga_configuration_handler

)
create_enV5_executable(main)
