#include "inet/applications/icn/ICNPublisher.h"

#include <omnetpp.h>

#include "inet/applications/icn/ICNPacket_m.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/packet/Packet.h"
#include "inet/applications/icn/MessageKinds.h"

namespace inet {

Define_Module(ICNPublisher);

ICNPublisher::ICNPublisher()
: mDataName("")
, mDelay(0)
, mRepeat(false)
, mDataSize(0)
, mBroadcastPublisher(false)
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
    mBroadcastPublisher = par("broadcast").boolValue();

    // schedule the first self-message
    scheduleAt(simTime() + mDelay, mSendMessage);
}

void ICNPublisher::handleMessage(cMessage* msg) {

    if (msg->isSelfMessage()) {

        // create and fill ICNPacket
        const auto& payload = makeShared<ICNPacket>();
        payload->setChunkLength(B(mDataSize));
        std::stringstream stringStream;
        stringStream << mDataName << "/" << simTime();
        payload->setIcnName(stringStream.str().c_str());
        payload->setPacketType(ICNPacketType::PUBLISH);
        payload->addTag<CreationTimeTag>()->setCreationTime(simTime());

        // encapsulate into packet
        Packet* packet = new Packet("ICNPublisherData");
        packet->insertAtBack(payload);
        if (mBroadcastPublisher) {
            packet->setKind(MessageKinds::BROADCAST_PUBLICATION);
        } else {
            packet->setKind(MessageKinds::PUBLICATION);
        }

        send(packet, PUBLICATIONS_GATE.c_str());

        // if we want to repeat sending this will handle it
        if (mRepeat) {
            scheduleAt(simTime() + mDelay, mSendMessage);
        }

        EV_INFO << "Publisher send message to base to publish some data!" << endl;

    } else {
        throw cRuntimeError("Publisher encountered message he can't handle!");
    }

}

} /* namespace inet */
