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

#include "ICNRouter.h"

namespace inet {

Define_Module(ICNRouter);

ICNRouter::ICNRouter()
: mRoutingTable()
{
}

ICNRouter::~ICNRouter() {
}

void ICNRouter::initialize() {
}

void ICNRouter::handleMessage(cMessage *msg)
{
    throw cRuntimeError("This module doesn't process messages");
}

bool ICNRouter::addSubscription(int interfaceId, std::string& subscription) {
    bool result = false;

    auto search = mRoutingTable.find(subscription);
    if (search != mRoutingTable.end()) {
        // we've found something
        std::vector<int> interfaces = search->second;
        // check if the interface is already stored
        bool alreadyStored = false;
        for (int& interface: interfaces) {
            alreadyStored = (interface == interfaceId) || alreadyStored;
        }
        if (!alreadyStored) {
            search->second.push_back(interfaceId);
            // when we reach this we added a new entry
            result = true;
            EV_INFO << "Added a new interface to subscription with name " << subscription << endl;
        }
    } else {
        mRoutingTable[subscription] = {interfaceId};
        EV_INFO << "Added a new entry to the routing table for subscription with name " << subscription << endl;
        // we also added a new entry when we reached this
        result = true;
    }
    return result;
}

std::vector<int> ICNRouter::getInterestedInterfaces(std::string& publication) {
    std::vector<int> result;
    auto search = mRoutingTable.find(publication);
    if (search != mRoutingTable.end()) {
        result = search->second;
    }
    return result;
}


} /* namespace inet */
