zephyr_library()

FILE(GLOB sources
	config.c
	config_util.c
)

zephyr_library_sources(${sources})
