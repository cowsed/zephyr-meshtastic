zephyr_library()

FILE(GLOB sources
	mock_shell.c
    config_shell.c
)
zephyr_library_sources(${sources})
