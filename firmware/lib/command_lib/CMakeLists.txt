zephyr_library()
zephyr_library_sources(command.c)

if(CONFIG_COMMAND_UART)
  zephyr_library_sources(uart_command.c)
endif()

if(CONFIG_COMMAND_GATT)
  zephyr_library_sources(gatt_command.c)
endif()
