import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

namespace inet;

enum ICNPacketType
{
   SUBSCRIBE = 0;
   PUBLISH = 1;
   ADVERTISEMENT = 2;
   UNSUBSCRIBE = 3;
};


class ICNPacket extends FieldsChunk
{
    int packetType @enum(ICNPacketType);
    string icnName;
}


