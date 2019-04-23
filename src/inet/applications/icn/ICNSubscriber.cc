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

namespace inet {

Define_Module(ICNSubscriber);

ICNSubscriber::ICNSubscriber()
: mSubscriptionName("")
, mSubscribeMessage(nullptr)
{
}

ICNSubscriber::~ICNSubscriber() {
    cancelAndDelete(mSubscribeMessage);
}

void ICNSubscriber::initialize() {

    // parse parameters
    mSubscriptionName = par("subscriptionName").stdstringValue();
    mSubscribeMessage = new cMessage("subscribeMessage");

    // schedule the first self-message
    scheduleAt(simTime(), mSubscribeMessage);
}

void ICNSubscriber::handleMessage(cMessage* msg) {

    if (msg->isSelfMessage()) {

        // create and fill ICNPacket
        const auto& payload = makeShared<ICNPacket>();
        payload->setChunkLength(B(1000));
        payload->setIcnName(mSubscriptionName.c_str());
        payload->setPacketType(ICNPacketType::SUBSCRIBE);

        // encapsulate into packet
        Packet* packet = new Packet("ICNSubscriberData");
        packet->insertAtBack(payload);

        send(packet, ICN_SUBSCRIBER_OUT.c_str());

    } else {
        throw cRuntimeError("Publisher encountered message he can't handle!");
    }
}

void ICNSubscriber::receiveData(const inet::Ptr<const ICNPacket>& icnPacket) {
    EV_INFO << "Received data to my subscription. Type: " << icnPacket->getPacketType() << " Name: " << icnPacket->getIcnName() << endl;
}

} /* namespace inet */
