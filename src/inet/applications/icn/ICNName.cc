#include "ICNName.h"

#include <sstream>
#include <iterator>
#include <algorithm>

namespace inet {

ICNName::ICNName(std::string icnName)
: mLevelContainer()
, mIsPrefixMatched(false)
{
    if (icnName[0] != '/') {
        throw cRuntimeError("ICNName: Malformed string passed. Expect '/' at beginning!");
    }
    // remove leading /
    std::string initializationString = icnName.substr(1);

    // check if ends with "/*"
    mIsPrefixMatched = icnName.substr(icnName.size() - 2) == "/*";

    if (mIsPrefixMatched) {
        // cut off last 2 characters
        initializationString.pop_back();
        initializationString.pop_back();
    }

    size_t position = 0;
    std::string delimiter = "/";
    while ((position = initializationString.find(delimiter)) != std::string::npos) {
        // get substring from start to first occurence of "/"
        mLevelContainer.push_back(initializationString.substr(0, position));
        // remove the part we just added
        initializationString.erase(0, position + delimiter.length());
    }
    mLevelContainer.push_back(initializationString);
}

std::string ICNName::generateString() const {
    std::stringstream stringStream;
    // add leading slash
    stringStream << "/";
    for (size_t index = 0; index < mLevelContainer.size(); index++) {
        stringStream << mLevelContainer[index];
        // not the last level?
        if (index != mLevelContainer.size() - 1) {
            stringStream << "/";
        }
    }

    if (mIsPrefixMatched) {
        stringStream << "/*";
    }

    return stringStream.str();
}

int ICNName::compare(ICNName& other) const {
    std::vector<std::string> otherLevels = other.getLevels();

    int matchingLevels = 0;
    for (size_t index = 0; index < std::min(mLevelContainer.size(), otherLevels.size()); index++) {
        if (mLevelContainer[index] == otherLevels[index]) {
            matchingLevels++;
        } else {
            // abort loop
            index = std::max(mLevelContainer.size(), otherLevels.size());
        }
    }
    return matchingLevels;
}

bool ICNName::matches(ICNName& other) const {
    bool result;
    if (isPrefixMatched() || other.isPrefixMatched()) {
        result = prefixMatches(other);
    } else {
        result = completeMatches(other);
    }
    return result;
}

bool ICNName::isPrefixMatched() const {
    return mIsPrefixMatched;
}

bool ICNName::completeMatches(ICNName& other) const {
    bool result = other.getNumberOfLevels() == getNumberOfLevels();
    return result && compare(other) == getNumberOfLevels();
}

bool ICNName::prefixMatches(ICNName& other) const {
    return compare(other) == std::min(other.getNumberOfLevels(), getNumberOfLevels());
}

int ICNName::getNumberOfLevels(void) const {
    return mLevelContainer.size();
}

std::vector<std::string> ICNName::getLevels(void) const {
    return std::vector<std::string>(mLevelContainer);
}

bool ICNNameCompare::operator()(const ICNName& lhs, const ICNName& rhs) const {
    return lhs.generateString() < rhs.generateString();
}

} /* namespace inet */
