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

package inet.node.icn;

import inet.node.icn.ICNSubscriberHost;
import inet.physicallayer.ieee80211.packetlevel.Ieee80211ScalarRadioMedium;

// This extends the subscriber Host definition by another
// wireless card that is configured for local communication
module ICNAdvancedSubscriberHost extends ICNSubscriberHost {
    parameters:
		// 2 wireless cards
	    numWlanInterfaces = 2;
		
		// the first one of the wireless cards will operate with default
		// parameters
		// the second one will operate in adhoc mode meaning that it will
		// discard all mgmt frames from access points but will allow me
		// to listen to all packets received
		wlan[1].mgmt.typename = "Ieee80211MgmtAdhoc";
		wlan[1].agent.typename = "";
		// use different frequency on 2nd card
		wlan[1].radio.bandName = "5 GHz";
}