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

// A simple module that should be used with ICN. You can use this to make a
// node send test data.
simple ICNPublisher {

parameters:
    // specifiy when the first message is being sent
    int startTime @unit(s);
    // if this is true the publication will be repeated with new timestamps appended
    bool repeat = default(false);
    // the delay between two publications (only has an effect if repeat is set to true)
    int delay @unit(s);
    // set this to true if you want to send a publication multiple times
    bool repeatPublication = default(true);
    // how often to repeat a single publication (only has an effect if repeatPublciation is true)
    int repeatsPublication = default(5);
    // the delay between two repetions of one publication (only has an effect if repeatPublciation is true)
    int repeatPublicationDelay @unit(s) = default(10s);
    // the name of the data that is being published
    string dataName = default("ICNPublisherDefaultDataName");
    // the size of the data
    int dataSize @unit(B) = default(500B);
    // indicates if this should broadcast all messages --> means sending to all interfaces no matter if interest are present
    bool broadcast = default(false);
	gates:
	    // this will transport publications to icnBase who will send them 
	    output publications;
	    
}