if(CONFIG_CUSTOM_DRIVERS)
  zephyr_include_directories(.)

  zephyr_library()
  zephyr_library_sources(
    interfaces.c
    )
endif()
