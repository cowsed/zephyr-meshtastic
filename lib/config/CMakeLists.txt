zephyr_library()

FILE(GLOB sources
	config.c
)

zephyr_library_sources(${sources})
