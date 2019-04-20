#include "ICNPublisher.h"

#include <omnetpp.h>

#include "inet/applications/icn/ICNPacket_m.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/packet/Packet.h"

namespace inet {

Define_Module(ICNPublisher);

ICNPublisher::ICNPublisher()
: mDataName("")
, mDelay(0)
, mRepeat(false)
, mDataSize(0)
, mSendMessage(nullptr)
{
}

ICNPublisher::~ICNPublisher() {
    cancelAndDelete(mSendMessage);
}

void ICNPublisher::initialize() {

    // parse parameters
    mDataName = par("dataName").stdstringValue();
    mDelay = par("delay").intValue();
    mRepeat = par("repeat").boolValue();
    mDataSize = par("dataSize").intValue();
    mSendMessage = new cMessage("sendMessage");

    // schedule the first self-message
    scheduleAt(simTime() + mDelay, mSendMessage);
}

void ICNPublisher::handleMessage(cMessage* msg) {

    if (msg == mSendMessage) {

        // create and fill ICNPacket
        const auto& payload = makeShared<ICNPacket>();
        payload->setChunkLength(B(mDataSize));
        payload->setName(mDataName.c_str());
        payload->addTag<CreationTimeTag>()->setCreationTime(simTime());

        // encapsulate into packet
        Packet* packet = new Packet("ICNPublisherData");
        packet->insertAtBack(payload);

        send(packet, "icnPublisherOut");

        // if we want to repeat sending this will handle it
        if (mRepeat) {
            scheduleAt(simTime() + mDelay, mSendMessage);
        }

    } else {
        throw cRuntimeError("Publisher encountered message he can't handle!");
    }

}

} /* namespace inet */
