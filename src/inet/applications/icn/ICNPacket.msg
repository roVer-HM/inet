import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

namespace inet;

enum ICNPacketType
{
   SUBSCRIBE = 0;
   PUBLISH = 1;
   LOCAL_REQUEST = 2;
   RESPONSE = 3;
   UNSUBSCRIBE = 4;
};


class ICNPacket extends FieldsChunk
{
    int packetType @enum(ICNPacketType);
    string name;
}


