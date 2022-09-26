if(CONFIG_COMMAND_LIB)
  zephyr_include_directories(.)

  zephyr_library()
  zephyr_library_sources(
    command.c
    uart_command.c
    gatt_command.c
    )
endif()
