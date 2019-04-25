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

namespace inet {

Define_Module(ICNSubscriber);

ICNSubscriber::ICNSubscriber()
: mPeriodicSubscriber(false)
, mDelay(0)
, mAdvertisementSubscriber(false)
, mAdvertisementDelay(0)
, mSubscriptionName("")
, mSubscribeMessage(nullptr)
, mResetAdvertisementMessage(nullptr)
, mReactToAdvertisement(true)
{
}

ICNSubscriber::~ICNSubscriber() {
    cancelAndDelete(mSubscribeMessage);
    cancelAndDelete(mResetAdvertisementMessage);
}

void ICNSubscriber::initialize() {

    // parse parameters
    mSubscriptionName = par("subscriptionName").stdstringValue();
    mPeriodicSubscriber = par("periodicSubscriber").boolValue();
    mDelay = par("delay").intValue();
    mAdvertisementSubscriber = par("advertisementSubscriber").boolValue();
    mAdvertisementDelay = par("delayAdvertismentReactions").intValue();
    mSubscribeMessage = new cMessage("subscribeMessage");
    mResetAdvertisementMessage = new cMessage("advertismentReset");

    // are we a periodic subscriber?
    if (mPeriodicSubscriber) {
        // schedule the first self-message
        scheduleAt(simTime(), mSubscribeMessage);
    }

}

void ICNSubscriber::handleMessage(cMessage* msg) {

    if (msg == mSubscribeMessage) {

        createAndSendPacket(1000, mSubscriptionName, ICNPacketType::SUBSCRIBE, "ICNPeriodicSubscription", ICN_SUBSCRIBER_OUT);

        if (mPeriodicSubscriber) {
            // schedule the first self-message
            scheduleAt(simTime() + mDelay, mSubscribeMessage);
        }

    } else if (msg == mResetAdvertisementMessage) {
      mReactToAdvertisement = true;
    } else {
        throw cRuntimeError("Publisher encountered message he can't handle!");
    }
}

void ICNSubscriber::receiveData(const inet::Ptr<const ICNPacket>& icnPacket) {
    Enter_Method_Silent();
    EV_INFO << "Received data to my subscription. Type: " << icnPacket->getPacketType() << " Name: " << icnPacket->getIcnName() << endl;
}

void ICNSubscriber::advertisementReceived() {
    Enter_Method_Silent();
    if (mAdvertisementSubscriber && mReactToAdvertisement) {
        createAndSendPacket(1000, mSubscriptionName, ICNPacketType::SUBSCRIBE, "ICNAdvertisementSubscription", ICN_SUBSCRIBER_OUT);
        mReactToAdvertisement = false;
        scheduleAt(simTime() + mAdvertisementDelay, mResetAdvertisementMessage);

    }
}

void ICNSubscriber::createAndSendPacket(const int chunkLength, const std::string& icnName, const ICNPacketType packetType, const std::string packetName, const std::string& gateName) {
    // create and fill ICNPacket
    const auto& payload = makeShared<ICNPacket>();
    payload->setChunkLength(B(chunkLength));
    payload->setIcnName(icnName.c_str());
    payload->setPacketType(packetType);

    // encapsulate into packet
    Packet* packet = new Packet(packetName.c_str());
    packet->insertAtBack(payload);

    send(packet, gateName.c_str());
}

} /* namespace inet */
