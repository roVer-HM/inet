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

bool ICNRouter::addSubscription(int interfaceId, ICNName& subscription) {
    bool result = false;

//    if (subscription.isPrefixMatched()) {
//        EV_INFO << "Routing table got a subscription that is prefix matched. That is not allowed! Discarding subscription with name "
//                << subscription.generateString() << "!" << endl;
//    }

    std::vector<int> search = find(subscription);
    if (!search.empty()) {

        // iterate all entries
        for (int& index: search) {
            // extract already stored interfaces
            std::vector<int> alreadyStoredInterfaces = mRoutingTable[index].second;

            // check if the interface we want to add is already stored
            bool alreadyStored = false;
            for (int& interface: alreadyStoredInterfaces) {
                alreadyStored = (interface == interfaceId) || alreadyStored;
            }
            if (!alreadyStored) {
                mRoutingTable[index].second.push_back(interfaceId);
                // when we reach this we added a new entry
                result = true;
                EV_INFO << "Added a new interface to subscription with name " << subscription.generateString() << endl;
            }
        }


    } else {
        // add a new entry
        std::vector<int> interfaceList;
        interfaceList.push_back(interfaceId);
        mRoutingTable.push_back(std::make_pair(subscription, interfaceList));
        EV_INFO << "Added a new entry to the routing table for subscription with name " << subscription.generateString() << endl;
        // we also added a new entry when we reached this
        result = true;
    }
    return result;
}

std::vector<int> ICNRouter::getInterestedInterfaces(ICNName& publication) {
    std::vector<int> result;
    std::vector<int> relevantIndices = find(publication);
    for (auto& index: relevantIndices) {
        for (auto& interfaceID: mRoutingTable[index].second) {
            result.push_back(interfaceID);
        }
    }
    return result;
}

std::vector<int> ICNRouter::find(ICNName& nameToFind) const {
    std::vector<int> result;

    for (size_t index = 0; index < mRoutingTable.size(); index++) {
        if (mRoutingTable.at(index).first.matches(nameToFind)) {
            result.push_back(index);
        }
    }
    return result;
}


} /* namespace inet */
