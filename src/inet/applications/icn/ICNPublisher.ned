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

simple ICNPublisher {

parameters:
    // if this is true the publishment will be repeated
    bool repeat = default(false);
    // set this to specify when the first message is being sent and in case repeat 
    // is true this will specifiy the delay between to messages
    int delay @unit(s);
    // the name of the data that is being published
    string dataName = default("ICNPublisherDefaultDataName");
    // the size of the data
    int dataSize @unit(B) = default(1000B);
	gates:
	    output icnPublisherOut;
	    
}