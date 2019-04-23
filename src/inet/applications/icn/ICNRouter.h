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

#ifndef INET_APPLICATIONS_ICN_ICNROUTER_H_
#define INET_APPLICATIONS_ICN_ICNROUTER_H_

#include <omnetpp/csimplemodule.h>
#include <map>

#include "inet/common/INETDefs.h"

namespace inet {

class ICNRouter: public omnetpp::cSimpleModule {
public:

    /**
     * This will be stored in the routing table
     * in case the name should be forwarded to a
     * local application.
     */
    static const int LOCAL_APPLICATION_ID = -1;

    /**
     * Constructor.
     */
    ICNRouter();

    /**
     * Destructor.
     */
    virtual ~ICNRouter();

    /**
     * Use this to add a subscription.
     *
     * This will add a subscription to the internal routing table. An
     * entry to this routing table is simple a mapping between from
     * subscription to interfaces.
     *
     * @param interfaceId The id of the interface that the given subscription
     * arrived on.
     * @param subscription A string containing the subscription. The subscription
     * is the name of the packet.
     *
     * @return bool true when a new entry was added to the table. False if not.
     * This should indicate if the subscription should be forwarded to neighbours.
     */
    bool addSubscription(int interfaceId, std::string& subscription);

    /**
     * This will return interfaces that the given publication should be forwarded
     * to.
     *
     * @param publication identfied by its name.
     *
     * @return vector<int> a vector of interfaces (integers).
     */
    std::vector<int> getInterestedInterfaces(std::string& publication);

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage* msg);

private:

    /**
     * Mapping from a subscription (name) to a list of interfaces.
     *
     * Example:
     *      "alert" --> (101, 102)
     *
     * A message containing the name "alert" should be forwarded to intrfaces
     * given in the list. In this case it should be forwarded to 101 and 102.
     *
     * Note: This class simply provides the interface where data with certain
     * names should be forwarded to. It does not do the actual forwarding.
     *
     * TODO: At some point this needs to contain information about timeouts
     * of subscriptions as well. This will likely need a different data structure.
     */
    std::map<std::string, std::vector<int>> mRoutingTable;

};

} /* namespace inet */

#endif /* INET_APPLICATIONS_ICN_ICNROUTER_H_ */
