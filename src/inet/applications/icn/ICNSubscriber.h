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

#ifndef INET_APPLICATIONS_ICN_ICNSUBSCRIBER_H_
#define INET_APPLICATIONS_ICN_ICNSUBSCRIBER_H_

#include "inet/common/INETDefs.h"

#include "inet/applications/icn/ICNPacket_m.h"
#include "inet/common/packet/Packet.h"
#include "inet/applications/icn/ICNName.h"
#include "inet/applications/icn/MessageKinds.h"

namespace inet {

class ICNSubscriber: public cSimpleModule, public cListener {
public:

    /**
     * Constructor.
     */
    ICNSubscriber();

    /**
     * Destructor.
     */
    virtual ~ICNSubscriber();

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage* msg) override;

    /**
     * Override receive signal from cListener.
     */
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) override;

private:

    /**
     * Stores the name of the gate used to send data to
     * icnBase.
     */
    const std::string SUBSCRIPTION_GATE = "subscriptions";

    /**
     * Stores if this subscriber should send periodic subscriptions.
     */
    bool mPeriodicSubscriber;

    /**
     * Stores the time between two periodic subscriptions.
     */
    int mPeriodicSubscriptionDelay;

    /**
     * Stores if this subscriber should send a subscription message
     * when a heartbeat of an access point was received.
     */
    bool mHeartbeatSubscriber;

    /**
     * This stores how long to wait until to react to another heartbeat.
     */
    int mHeartbeatSubscriptionDelay;

    /**
     * Stores the name of the data this is subscribing to.
     */
    ICNName mSubscriptionICNName;

    /**
     * States if this subscriber should react to the next advertisement.
     */
    bool mReactToHeartbeat;

    /**
     * This is is sent to myself to trigger periodic subscriptions.
     */
    cMessage* mSubscribeMessage;

    /**
     * This is sent to myself
     */
    cMessage* mResetHeartbeatSubscriptionMessage;

    /**
     * Used to check if a subscription result is a heartbeat message
     * from an access point.
     */
    ICNName mHeartbeatPrefix;

    /**
     * This is to store which access point we were last subscribed to.
     */
    ICNName mLastSubscribedAccessPointHeartbeat;

    /**
     * This is to store if this subscriber should send a subscription
     * when the node is associated to an access point.
     */
    bool mSubscribeOnAssociation;

    simsignal_t mDataArrivedSignal;

    /**
     * Helper method to send icn packet to the given gate.
     */
    void createAndSendPacket(const int chunkLength, const std::string& icnName, const ICNPacketType packetType, const std::string packetName, const std::string& gateName, MessageKinds messageKind);

    /**
     * Helper method to handle situation when a heartbeat is received.
     */
    void handleReceivedHeartbeat(ICNName& heartbeatName);

};

} /* namespace inet */

#endif /* INET_APPLICATIONS_ICN_ICNSUBSCRIBER_H_ */
