//
// Copyright (C) OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#include "inet/physicallayer/communicationcache/MapCommunicationCache.h"
#include "inet/physicallayer/contract/packetlevel/IRadio.h"

namespace inet {
namespace physicallayer {

Define_Module(MapCommunicationCache);

MapCommunicationCache::~MapCommunicationCache()
{
    for (auto& it : transmissionCache)
        delete static_cast<std::map<int, ReceptionCacheEntry> *>(it.second.receptionCacheEntries);
}

MapCommunicationCache::RadioCacheEntry *MapCommunicationCache::getRadioCacheEntry(const IRadio *radio)
{
    return &radioCache[radio->getId()];
}

MapCommunicationCache::TransmissionCacheEntry *MapCommunicationCache::getTransmissionCacheEntry(const ITransmission *transmission)
{
    return &transmissionCache[transmission->getId()];
}

MapCommunicationCache::ReceptionCacheEntry *MapCommunicationCache::getReceptionCacheEntry(const IRadio *radio, const ITransmission *transmission)
{
    TransmissionCacheEntry *transmissionCacheEntry = getTransmissionCacheEntry(transmission);
    auto receptionCacheEntries = static_cast<std::map<int, ReceptionCacheEntry> *>(transmissionCacheEntry->receptionCacheEntries);
    return &(*receptionCacheEntries)[radio->getId()];
}

void MapCommunicationCache::addRadio(const IRadio *radio)
{
    auto radioCacheEntry = getRadioCacheEntry(radio);
    radioCacheEntry->radio = radio;
}

void MapCommunicationCache::removeRadio(const IRadio *radio)
{
    auto radioId = radio->getId();
    auto it = radioCache.find(radioId);
    if (it != radioCache.end())
        radioCache.erase(it);
    for (auto& it : transmissionCache) {
        std::map<int, ReceptionCacheEntry> *receptionCacheEntries = static_cast<std::map<int, ReceptionCacheEntry> *>(it.second.receptionCacheEntries);
        if (receptionCacheEntries != nullptr) {
            auto jt = receptionCacheEntries->find(radioId);
            if (jt != receptionCacheEntries->end())
                receptionCacheEntries->erase(jt);
        }
    }
}

const IRadio *MapCommunicationCache::getRadio(int id) const
{
    auto it = radioCache.find(id);
    if (it == radioCache.end())
        return nullptr;
    else
        return it->second.radio;
}

void MapCommunicationCache::mapRadios(std::function<void (const IRadio *)> f) const
{
    for (auto& it : radioCache)
        f(it.second.radio);
}

void MapCommunicationCache::addTransmission(const ITransmission *transmission)
{
    auto transmissionCacheEntry = getTransmissionCacheEntry(transmission);
    transmissionCacheEntry->transmission = transmission;
    transmissionCacheEntry->receptionCacheEntries = new std::map<int, ReceptionCacheEntry>();
}

void MapCommunicationCache::removeTransmission(const ITransmission *transmission)
{
    auto it = transmissionCache.find(transmission->getId());
    if (it != transmissionCache.end()) {
        const TransmissionCacheEntry& transmissionCacheEntry = it->second;
        std::map<int, ReceptionCacheEntry> *receptionCacheEntries = static_cast<std::map<int, ReceptionCacheEntry> *>(transmissionCacheEntry.receptionCacheEntries);
        if (receptionCacheEntries != nullptr) {
            for (auto& jt : *receptionCacheEntries) {
                const RadioCacheEntry& radioCacheEntry = radioCache[jt.first];
                const ReceptionCacheEntry& receptionCacheEntry = jt.second;
                const IntervalTree::Interval *interval = receptionCacheEntry.interval;
                if (interval != nullptr)
                    radioCacheEntry.receptionIntervals->deleteNode(interval);
            }
        }
        delete static_cast<std::map<int, ReceptionCacheEntry> *>(transmissionCacheEntry.receptionCacheEntries);
        transmissionCache.erase(it);
    }
}

const ITransmission *MapCommunicationCache::getTransmission(int id) const
{
    auto it = transmissionCache.find(id);
    if (it == transmissionCache.end())
        return nullptr;
    else
        return it->second.transmission;
}

void MapCommunicationCache::mapTransmissions(std::function<void (const ITransmission *)> f) const
{
    for (auto& it : transmissionCache)
        f(it.second.transmission);
}

void MapCommunicationCache::removeNonInterferingTransmissions(std::function<void (const ITransmission *transmission)> f)
{
    int transmissionCount = 0;
    const simtime_t now = simTime();
    for (auto it = transmissionCache.cbegin(); it != transmissionCache.cend();) {
        const TransmissionCacheEntry& transmissionCacheEntry = it->second;
        if (transmissionCacheEntry.interferenceEndTime <= now) {
            if (transmissionCacheEntry.receptionCacheEntries != nullptr) {
                std::vector<ReceptionCacheEntry> *receptionCacheEntries = static_cast<std::vector<ReceptionCacheEntry> *>(transmissionCacheEntry.receptionCacheEntries);
                auto radioIt = radioCache.cbegin();
                auto receptionIt = receptionCacheEntries->cbegin();
                while (radioIt != radioCache.cend() && receptionIt != receptionCacheEntries->cend()) {
                    const RadioCacheEntry& radioCacheEntry = radioIt->second;
                    const ReceptionCacheEntry& receptionCacheEntry = *receptionIt;
                    const IntervalTree::Interval *interval = receptionCacheEntry.interval;
                    if (interval != nullptr)
                        radioCacheEntry.receptionIntervals->deleteNode(interval);
                    radioIt++;
                    receptionIt++;
                }
            }
            f(transmissionCacheEntry.transmission);
            transmissionCache.erase(it++);
            transmissionCount++;
        }
        else
            it++;
    }
    EV_DEBUG << "Removed " << transmissionCount << " non interfering transmissions\n";
}

} // namespace physicallayer
} // namespace inet

