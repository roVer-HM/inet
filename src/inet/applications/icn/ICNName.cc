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
        if (mLevelContainer.size() == 0) {
            stringStream << "*";
        } else {
            stringStream << "/*";
        }

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

ICNName ICNName::removeLastLevel(void) const {
    // String representation of this
    std::string stringRepresentation = generateString();

    auto const position = stringRepresentation.find_last_of('/');

    std::string substring = stringRepresentation.substr(0, position+1);
    ICNName result(substring);
    return result;
}

ICNName ICNName::makePrefixMatched(void) const {
    // String representation of this
    std::string stringRepresentation = generateString();

    std::stringstream stringStream;
    stringStream << stringRepresentation << "/*";

    ICNName result(stringStream.str());
    return result;
}

bool ICNName::matchWithoutVersion(ICNName& other) {
    // this whole thing is kinda dirty but only needed at one place
    bool result = false;
    if (isPrefixMatched() || other.isPrefixMatched()) {
        throw cRuntimeError("Cant match without version when one of them is prefix matched!");
    } else {
        if (!hasVersion() || !other.hasVersion()) {
            throw cRuntimeError("Match without version expects two icn names with version at the end'!");
        } else {
            ICNName thisComparisonObject = removeLastLevel();
            ICNName otherComparisonObject = other.removeLastLevel();
            result = thisComparisonObject.matches(otherComparisonObject);
        }
    }
    return result;
}

bool ICNName::hasHigherVersion(ICNName& other) {
    bool result = false;
    if (hasVersion() && other.hasVersion()) {
        int thisVersion = std::stoi(mLevelContainer.at(mLevelContainer.size() - 1));
        int otherVersion = std::stoi(other.getLevels().at(other.getNumberOfLevels() - 1));
        result = otherVersion > thisVersion;
    }
    return result;
}

bool ICNName::hasVersion(void) {
    bool result = true;
    std::string toCheck = mLevelContainer[mLevelContainer.size() - 1];
    for (char& character: toCheck) {
        result = result && std::isdigit(character);
    }
    return result;
}

bool ICNNameCompare::operator()(const ICNName& lhs, const ICNName& rhs) const {
    return lhs.generateString() < rhs.generateString();
}



} /* namespace inet */
