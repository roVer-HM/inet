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

package inet.applications.icn;

// The ICNSubscriber will publish messages which contain an ICN subscription 
// to icn base who will manage it.
simple ICNSubscriber {
    parameters:
        // specifies if this subscriber should subscribe periodically to certain data
        bool periodicSubscriber = default(false);
        // specifies the seconds between two periodic subsciption messages
        int delay @unit(s) = default(5s);
        // specifies if this subscriber should subscribe when an advertisement is encountered
        bool advertisementSubscriber = default(false);
        // specifies the seconds until this is allowed to react to a subscription again
        int delayAdvertismentReactions @unit(s) = default(60s);
        // specify the data you want to subscribe to
        string subscriptionName = default("TestDataSubscription"); 
    gates:
        // messages going to icn base
        output icnSubscriberOut;
}