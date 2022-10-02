if(CONFIG_COMMAND_LIB)
  zephyr_include_directories(.)

  zephyr_library()
  zephyr_library_sources(
    command.c
    )
endif()

if(CONFIG_COMMAND_UART)
  zephyr_library_sources(
    command.c
    uart_command.c
    )
endif()

if(CONFIG_COMMAND_GATT)
  zephyr_library_sources(
    command.c
    gatt_command.c
    )
endif()
