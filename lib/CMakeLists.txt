add_subdirectory_ifdef(CONFIG_MESHTASTIC_SHELL shell)


zephyr_library()

FILE(GLOB sources
	level0.c
	level1.c
	level2.c
	level3.c
)
zephyr_library_sources(${sources})
