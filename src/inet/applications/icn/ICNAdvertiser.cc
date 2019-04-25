#include "ICNAdvertiser.h"

#include <omnetpp.h>

#include "inet/applications/icn/ICNPacket_m.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/packet/Packet.h"

namespace inet {

Define_Module(ICNAdvertiser);

ICNAdvertiser::ICNAdvertiser()
: mDataName("")
, mDelay(0)
, mDataSize(0)
, mSendMessage(nullptr)
{
}

ICNAdvertiser::~ICNAdvertiser() {
    cancelAndDelete(mSendMessage);
}

void ICNAdvertiser::initialize() {

    // parse parameters
    mDataName = par("dataName").stdstringValue();
    mDelay = par("delay").intValue();
    mDataSize = par("dataSize").intValue();
    mSendMessage = new cMessage("sendMessage");

    // schedule the first self-message
    scheduleAt(simTime() + mDelay, mSendMessage);
}

void ICNAdvertiser::handleMessage(cMessage* msg) {

    if (msg == mSendMessage) {

        // create and fill ICNPacket
        const auto& payload = makeShared<ICNPacket>();
        payload->setChunkLength(B(mDataSize));
        payload->setIcnName(mDataName.c_str());
        payload->setPacketType(ICNPacketType::ADVERTISEMENT);
        payload->addTag<CreationTimeTag>()->setCreationTime(simTime());

        // encapsulate into packet
        Packet* packet = new Packet("ICNAdvertiserData");
        packet->insertAtBack(payload);

        send(packet, "icnAdvertiserOut");

        // repeat...
        scheduleAt(simTime() + mDelay, mSendMessage);

        EV_INFO << "Advertiser send message to base to advertise!" << endl;

    } else {
        throw cRuntimeError("Publisher encountered message he can't handle!");
    }

}

} /* namespace inet */
