#include <cstddef>
#include <stdbool.h>
#include <stdint.h>

#include "meshtastic/mesh.pb.h"

typedef uint32_t NodeID;
typedef uint32_t PacketID;
typedef uint8_t PacketHeaderFlags;
typedef uint8_t PacketChannelHash;

// https://meshtastic.org/docs/overview/mesh-algo/
#define MESHTASTIC_SYNC_WORD 0x2B

void meshtastic_send_level0(meshtastic_MeshPacket_Priority tx_priority,
                            bool do_csma, uint8_t *data, size_t len);

void meshtastic_send_level1(size_t tx_priority, NodeID destination,
                            NodeID sender, PacketID packetId,
                            PacketHeaderFlags flags, PacketChannelHash hash,
                            uint8_t *data, size_t length);

// Mocking stuff
void meshtastic_mock_level0_recv(int i);
void meshtastic_mock_level1_recv(int i);
void meshtastic_mock_level2_recv(int i);
void meshtastic_mock_level3_recv(int i);
