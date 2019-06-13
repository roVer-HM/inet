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

import inet.node.inet.WirelessHost;
import inet.applications.icn.ICN;

module ICNHostBase extends WirelessHost {
	parameters:
	    numApps = 1;
	    ipv4.arp.typename = "GlobalArp";
	    app[0].typename = "ICN";
	    numLoInterfaces = 0;
	    wlan[0].radio.typename = "Ieee80211ScalarRadio";
	    double icnBitrate @unit(bps) = default(54Mbps);
	    // this is roughly 150m
	    wlan[0].radio.transmitter.power = 0.75mW;
	    wlan[0].bitrate = icnBitrate;
	    wlan[0].mac.*.rateSelection.multicastFrameBitrate = icnBitrate;
}