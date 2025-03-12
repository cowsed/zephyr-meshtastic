add_subdirectory(drivers)
if (CONFIG_MESHTASTIC)
add_subdirectory_ifdef(CONFIG_MESHTASTIC lib)


zephyr_library_named(zephyr_meshtastic) # this creates a zephyr-meshtastic library target

set(MESHTASTIC_DIR ${ZEPHYR_CURRENT_MODULE_DIR})

list(APPEND CMAKE_MODULE_PATH ${ZEPHYR_BASE}/modules/nanopb)
include(nanopb)

set(NANOPB_IMPORT_DIRS ${NANOPB_SRC_ROOT_FOLDER}/generator/proto)

set(pb_sources
    ${MESHTASTIC_DIR}/protobufs/meshtastic/admin.proto
    ${MESHTASTIC_DIR}/protobufs/meshtastic/apponly.proto
    ${MESHTASTIC_DIR}/protobufs/meshtastic/atak.proto
    ${MESHTASTIC_DIR}/protobufs/meshtastic/cannedmessages.proto
    ${MESHTASTIC_DIR}/protobufs/meshtastic/channel.proto
    ${MESHTASTIC_DIR}/protobufs/meshtastic/clientonly.proto
    ${MESHTASTIC_DIR}/protobufs/meshtastic/config.proto
    ${MESHTASTIC_DIR}/protobufs/meshtastic/connection_status.proto
    ${MESHTASTIC_DIR}/protobufs/meshtastic/device_ui.proto
    ${MESHTASTIC_DIR}/protobufs/meshtastic/interdevice.proto
    ${MESHTASTIC_DIR}/protobufs/meshtastic/localonly.proto
    ${MESHTASTIC_DIR}/protobufs/meshtastic/mesh.proto
    ${MESHTASTIC_DIR}/protobufs/meshtastic/module_config.proto
    ${MESHTASTIC_DIR}/protobufs/meshtastic/mqtt.proto
    ${MESHTASTIC_DIR}/protobufs/meshtastic/paxcount.proto
    ${MESHTASTIC_DIR}/protobufs/meshtastic/portnums.proto
    ${MESHTASTIC_DIR}/protobufs/meshtastic/powermon.proto
    ${MESHTASTIC_DIR}/protobufs/meshtastic/remote_hardware.proto
    ${MESHTASTIC_DIR}/protobufs/meshtastic/rtttl.proto
    ${MESHTASTIC_DIR}/protobufs/meshtastic/storeforward.proto
    ${MESHTASTIC_DIR}/protobufs/meshtastic/telemetry.proto
    ${MESHTASTIC_DIR}/protobufs/meshtastic/xmodem.proto
    ${NANOPB_SRC_ROOT_FOLDER}/generator/proto/google/protobuf/descriptor.proto
    ${MESHTASTIC_DIR}/protobufs/nanopb.proto
)

zephyr_nanopb_sources(zephyr_meshtastic ${pb_sources})

zephyr_include_directories(${CMAKE_CURRENT_BINARY_DIR}/protobufs)
zephyr_include_directories(${NANOPB_SRC_ROOT_FOLDER}) #pb.h

# Isn't building with correct include in nanopb.pb.h, so do it here manually
target_compile_options(zephyr_meshtastic PUBLIC -include ${CMAKE_CURRENT_BINARY_DIR}/descriptor.pb.h)

# This is needed so that custom driver classes using system calls are taken into
# account
zephyr_syscall_include_directories(include)
zephyr_include_directories(include)


endif()