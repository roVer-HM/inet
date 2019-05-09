//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "ICNSubscriber.h"

#include <omnetpp/simutil.h>

#include "inet/common/ModuleAccess.h"

namespace inet {

Define_Module(ICNSubscriber);

ICNSubscriber::ICNSubscriber()
: mPeriodicSubscriber(false)
, mPeriodicSubscriptionDelay(0)
, mHeartbeatSubscriber(false)
, mHeartbeatSubscriptionDelay(0)
, mSubscriptionICNName("/undefined")
, mReactToHeartbeat(true)
, mSubscribeMessage(nullptr)
, mResetHeartbeatSubscriptionMessage(nullptr)
, mHeartbeatPrefix("/accessPoint/heartbeat/*")
, mLastSubscribedAccessPointHeartbeat("/undefined")
, mSubscribeOnAssociation(false)
, mDataArrivedSignal()
{
}

ICNSubscriber::~ICNSubscriber() {
    cancelAndDelete(mSubscribeMessage);
    cancelAndDelete(mResetHeartbeatSubscriptionMessage);
}

void ICNSubscriber::initialize() {

    // parse parameters
    std::string subscriptionName = par("subscriptionName").stdstringValue();
    mSubscriptionICNName = ICNName(subscriptionName);
    mPeriodicSubscriber = par("periodicSubscriber").boolValue();
    mPeriodicSubscriptionDelay = par("periodicSubscriptionDelay").intValue();
    mHeartbeatSubscriber = par("heartbeatSubscriber").boolValue();
    mHeartbeatSubscriptionDelay = par("heartbeatSubscriptionDelay").intValue();
    mSubscribeMessage = new cMessage("subscribeMessage");
    mResetHeartbeatSubscriptionMessage = new cMessage("advertisementReset");
    mSubscribeOnAssociation = par("associationSubscriber").boolValue();
    mDataArrivedSignal = registerSignal("dataArrived");

    // are we a periodic subscriber?
    if (mPeriodicSubscriber) {
        // schedule the first self-message
        scheduleAt(simTime(), mSubscribeMessage);
    }

    // we need to tell icn base that we are interested in heartbeats from access points
    if (mHeartbeatSubscriber) {
        createAndSendPacket(1000, mHeartbeatPrefix.generateString(), ICNPacketType::SUBSCRIBE, "ICNSilentHeartbeatSubscription", SUBSCRIPTION_GATE, MessageKinds::SILENT_SUBSCRIPTION);
    }
    if (mSubscribeOnAssociation) {
        cModule* host = getContainingNode(this);
        // when we receive the association we send a subscription
        host->subscribe(l2AssociatedSignal, this);
    }
    createAndSendPacket(1000, mSubscriptionICNName.generateString(), ICNPacketType::SUBSCRIBE, "ICNSilentHeartbeatSubscription", SUBSCRIPTION_GATE, MessageKinds::SILENT_SUBSCRIPTION);

}

void ICNSubscriber::handleMessage(cMessage* msg) {

    if (msg->isSelfMessage()) {
        if (msg == mSubscribeMessage) {

            createAndSendPacket(
                    1000,
                    mSubscriptionICNName.generateString(),
                    ICNPacketType::SUBSCRIBE,
                    "ICNPeriodicSubscription",
                    SUBSCRIPTION_GATE,
                    MessageKinds::SUBSCRIPTION
                    );

            if (mPeriodicSubscriber) {
                // schedule the first self-message
                scheduleAt(simTime() + mPeriodicSubscriptionDelay, mSubscribeMessage);
            }

        } else if (msg == mResetHeartbeatSubscriptionMessage) {
          mReactToHeartbeat = true;
        }
    } else {
        // we received a data packet that we previously subscribed!!
        if (msg->getKind() == MessageKinds::SUBSCRIPTION_RESULT) {
            Packet* packet = check_and_cast<Packet*>(msg);
            const Ptr<const ICNPacket> icnPacket = packet->peekAtFront<ICNPacket>(b(-1), Chunk::PF_ALLOW_INCORRECT);
            ICNName resultName(icnPacket->getIcnName());
            if (resultName.matches(mHeartbeatPrefix)) {
                handleReceivedHeartbeat(resultName);
            } else if (resultName.matches(mSubscriptionICNName)) {
                // received requested data
                EV_INFO << "Received data to my subscription. Type: " << icnPacket->getPacketType() << " Name: " << icnPacket->getIcnName() << endl;
                ICNName icnName(icnPacket->getIcnName());
                simtime_t sendTime = std::stoi(icnName.getLevels().at(icnName.getNumberOfLevels() - 1));
                // emit signal for statistic collection
                emit(mDataArrivedSignal, simTime() - sendTime);
            } else {
                // received unknown data --> this should not happen
                throw cRuntimeError("Received data with a name that I did not request!");
            }
            delete msg;
        } else {
            throw cRuntimeError("Publisher encountered message he can't handle!");
        }
    }


}

// TODO: There can be done much more here:
//      1. Store a list access points (heartbeat identifier) which I subscribed to
//      2. When there was no heartbeat received from an access point in a while we remove it
//      3. When we receive heartbeat but a certain time has passed we renew the subscription
void ICNSubscriber::handleReceivedHeartbeat(ICNName& heartbeatName) {
    // received a heartbeat
    EV_INFO << "Received a heartbeat!" << std::endl;
    ASSERT(mHeartbeatSubscriber);

    if (mLastSubscribedAccessPointHeartbeat.matches(heartbeatName)) {
        if (mReactToHeartbeat) {
            createAndSendPacket(1000, mSubscriptionICNName.generateString(), ICNPacketType::SUBSCRIBE, "ICNHeartbeatSubscription", SUBSCRIPTION_GATE, MessageKinds::SUBSCRIPTION);
            mReactToHeartbeat = false;
            scheduleAt(simTime() + mHeartbeatSubscriptionDelay, mResetHeartbeatSubscriptionMessage);
            EV_INFO << "We haven't sent a subscription in a while to access point with heartbeat '"
                    << mLastSubscribedAccessPointHeartbeat.generateString()
                    << "'. Therefore we sent a new one!" << std::endl;
        } else {
            EV_INFO << "A subscription has recently been sent we don't need to send another one to access point with heartbeat '"
                    << mLastSubscribedAccessPointHeartbeat.generateString() << "'!" << EV_INFO;
        }
    } else {
        // this means we found a new access point!
        mLastSubscribedAccessPointHeartbeat = heartbeatName;
        mReactToHeartbeat = false;
        cancelEvent(mResetHeartbeatSubscriptionMessage);
        scheduleAt(simTime() + mHeartbeatSubscriptionDelay, mResetHeartbeatSubscriptionMessage);
        createAndSendPacket(1000, mSubscriptionICNName.generateString(), ICNPacketType::SUBSCRIBE, "ICNHeartbeatSubscription", SUBSCRIPTION_GATE, MessageKinds::SUBSCRIPTION);
    }


}

void ICNSubscriber::createAndSendPacket(const int chunkLength, const std::string& icnName, const ICNPacketType packetType, const std::string packetName, const std::string& gateName, MessageKinds messageKind) {
    // create and fill ICNPacket
    const auto& payload = makeShared<ICNPacket>();
    payload->setChunkLength(B(chunkLength));
    payload->setIcnName(icnName.c_str());
    payload->setPacketType(packetType);

    // encapsulate into packet
    Packet* packet = new Packet(packetName.c_str());
    packet->insertAtBack(payload);
    packet->setKind(messageKind);

    send(packet, gateName.c_str());
}

void ICNSubscriber::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) {
    Enter_Method_Silent();
    printSignalBanner(signalID, obj, details);
    EV_INFO << "We are associated to a signal. Sending a subscription..." << endl;
    createAndSendPacket(1000, mSubscriptionICNName.generateString(), ICNPacketType::SUBSCRIBE, "ICNAssociatedSubscription", SUBSCRIPTION_GATE, MessageKinds::SUBSCRIPTION);
}
} /* namespace inet */
