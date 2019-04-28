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
#include "inet/applications/icn/ICNName.h"

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
     * @param subscription ICNName
     *
     * @return bool true when a new entry was added to the table. False if not.
     * This should indicate if the subscription should be forwarded to neighbours.
     */
    bool addSubscription(int interfaceId, ICNName& subscription);

    /**
     * This will return interfaces that the given publication should be forwarded
     * to.
     *
     * @param publication ICNName
     *
     * @return vector<int> a vector of interfaces (integers).
     */
    std::vector<int> getInterestedInterfaces(ICNName& publication);

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
    std::map<ICNName, std::vector<int>, ICNNameCompare> mRoutingTableOld;

    /**
     * Stores all possible mappings from ICNName to a list of interfaces.
     *
     * When searching for interfaces that a message with a given ICNName
     * should be forwarded to this vector will be iterated and the
     * first element of each pair will be matched with the search parameter.
     */
    std::vector<std::pair<ICNName, std::vector<int>>> mRoutingTable;

    /**
     * A small helper method which will return the index of the element
     * that is to be found.
     *
     * @param nameToFind The icn name to find in the routing table.
     * @return vector<int> A vector of indices (empty if nothing was found).
     */
    std::vector<int> find(ICNName& nameToFind) const;

};

} /* namespace inet */

#endif /* INET_APPLICATIONS_ICN_ICNROUTER_H_ */
