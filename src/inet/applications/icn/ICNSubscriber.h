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

namespace inet {

class ICNSubscriber: public cSimpleModule {
public:

    /**
     * Constructor.
     */
    ICNSubscriber();

    /**
     * Destructor.
     */
    virtual ~ICNSubscriber();

    /**
     * This can be called to notify the subscriber of data that
     * arrived for it.
     */
    void receiveData(const inet::Ptr<const ICNPacket>& icnPacket);

    /**
     * Tell this subscriber that an advertisement was received.
     */
    void advertisementReceived();

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage* msg) override;

private:

    /**
     * Stores the name of the gate used to send data to
     * icnBase.
     */
    const std::string ICN_SUBSCRIBER_OUT = "icnSubscriberOut";

    /**
     * Stores if this subscriber should send periodic subscriptions.
     */
    bool mPeriodicSubscriber;

    /**
     * Stores the time between two periodic subscriptions.
     */
    int mDelay;

    /**
     * Stores if this subscriber should send a subscription message
     * when an advertisement of an access point was received.
     */
    bool mAdvertisementSubscriber;

    /**
     * This stores how long to wait until to react to another advertisment.
     */
    int mAdvertisementDelay;

    /**
     * Stores the name of the data this is subscribing to.
     */
    std::string mSubscriptionName;

    /**
     * This is is sent to myself to trigger periodic subscriptions.
     */
    cMessage* mSubscribeMessage;

    /**
     * This is sent to myself
     */
    cMessage* mResetAdvertisementMessage;

    /**
     * States if this subscriber should react to the next advertisement.
     */
    bool mReactToAdvertisement;

    /**
     * Helper method to send icn packet to the given gate.
     */
    void createAndSendPacket(const int chunkLength, const std::string& icnName, const ICNPacketType packetType, const std::string packetName, const std::string& gateName);
};

} /* namespace inet */

#endif /* INET_APPLICATIONS_ICN_ICNSUBSCRIBER_H_ */
