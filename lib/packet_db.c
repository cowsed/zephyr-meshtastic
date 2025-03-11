#include <stddef.h>
#include <stdint.h>

typedef uint64_t PacketDBId; 

PacketDBId NULLPACKETDBID = 0;



enum EntryType{
    EntryType_Packet = 0,
    EntryType_Ack = 1
};

struct PacketDbEntry{
    uint32_t to;
    uint32_t from;
    uint8_t channel;
    uint32_t rx_time;
};


struct internalStorage{
    PacketDBId id;
    struct PacketDbEntry data;
};


PacketDBId add_packet(const struct PacketDbEntry *entry);
