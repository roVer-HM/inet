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
, mRepeatPublication(true)
, mRepeatsPublication(3)
, mRepeatPublicationDelay(3)
, mPublicationTimer(nullptr)
, mCurrentPublication(nullptr)
{
}

ICNPublisher::~ICNPublisher() {
    cancelAndDelete(mPublicationTimer);
    cancelAndDelete(mCurrentPublication);
}

void ICNPublisher::initialize() {

    // parse parameters
    mDataName = par("dataName").stdstringValue();
    mDelay = par("delay").intValue();
    mRepeat = par("repeat").boolValue();
    mDataSize = par("dataSize").intValue();
    mPublicationTimer = new cMessage("publicationTimer");
    mBroadcastPublisher = par("broadcast").boolValue();
    mRepeatPublication = par("repeatPublication").boolValue();
    mRepeatsPublication = par("repeatsPublication").intValue();
    mRepeatPublicationDelay = par("repeatPublicationDelay").intValue();

    simtime_t startTime = par("startTime").intValue();
    if (startTime < SimTime::ZERO) {
        throw cRuntimeError("Invalid start time!");
    }
    // schedule the first self-message
    if (startTime <= simTime()) {
        startTime = simTime();
    }

    scheduleAt(startTime, mPublicationTimer);

}

void ICNPublisher::handleMessage(cMessage* msg) {

    if (msg->isSelfMessage()) {

        if (msg == mPublicationTimer) {
            schedulePublicationSending();
        } else {
            send(msg, PUBLICATIONS_GATE.c_str());
            EV_INFO << "Publisher send message to base to publish some data!" << endl;
        }

    } else {
        throw cRuntimeError("Publisher encountered message he can't handle!");
    }

}

void ICNPublisher::schedulePublicationSending() {
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

    // delete old current publication and overwrite with the new one
    cancelAndDelete(mCurrentPublication);
    mCurrentPublication = packet;

    // record scalar for statistical analysis
    recordScalar(stringStream.str().c_str(), 0);

    if (mRepeat) {
        // schedule the next new publication
        scheduleAt(simTime() + mDelay, mPublicationTimer);
    }

    if (mRepeatPublication) {
        // repeat the very same publication multiple times
        simtime_t scheduleTime = simTime();
        for (int index = 0; index < mRepeatsPublication; ++index) {
            scheduleAt(scheduleTime, mCurrentPublication->dup());
            scheduleTime = scheduleTime + mRepeatPublicationDelay;
        }
    } else {
        // just send it once
        scheduleAt(simTime(), mCurrentPublication);
    }
}

} /* namespace inet */
