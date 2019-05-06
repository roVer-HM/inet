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

import inet.node.icn.ICNHostBase;

module ICNSubscriberHost extends ICNHostBase {
	parameters:
	    @display("i=device/cellphone");
	    // this is what this subscriber will subscribe to (format see ICNName.h)
	    string subscribeTo;
	    
	    // set subscriber params
	    app[0].icnForwarding = false;
	    app[0].hasSubscriber = true;
	    app[0].subscriber.associationSubscriber = true;
	    app[0].subscriber.subscriptionName = subscribeTo;	      
}