
import inet.common.INETDefs;
import inet.common.packet.chunk.Chunk;

namespace inet;

enum PacketType
{
   SUBSCRIBE = 0;
   PUBLISH = 1;
};

// 
// Message class for event notification service based on TCP.
// 
class EventNotificationMsg extends FieldsChunk
{
    int type @enum(PacketType);
	// identifies data to compare with icn
	string icnComparisonName;
	
}



