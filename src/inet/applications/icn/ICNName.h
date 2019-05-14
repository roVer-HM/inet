#ifndef INET_APPLICATIONS_ICN_ICNNAME_H_
#define INET_APPLICATIONS_ICN_ICNNAME_H_

#include <string>
#include <vector>

#include "inet/common/INETDefs.h"

namespace inet {

/**
 * This class represents ICNNames.
 *
 * An ICN name is hierarchical. They can be
 * represented as strings. Each level is delimited
 * with a '/' from other levels. Much like in a URL.
 *
 * Example:
 *
 *      - /test/bla/super/mega
 *
 * The string always starts with '/' then the different
 * levels follow. The example above would identify data
 * an access point would publish to draw attention
 * to itself.
 *
 * The class offers methods to parse ICN names from strings,
 * compare them, parse them to strings etc.
 *
 * An ICNName can be compared in two ways to another icn name:
 *  1. complete Matching -> all levels will be compared -> if they are not equal in length they do not match
 *  2. prefix Matching -> the shorter one of both will determine what will be compared
 *
 * To create an ICNName with prefix matching place an asterisk
 * at the end of the initialization string.
 *
 */
class ICNName {
public:
    /**
     * Constructor.
     *
     * Use this one to intialize the name with the given string. If parsing
     * fails a runtimeError will be thrown.
     *
     * If the provided name has an asterisk at the end this will be a prefix
     * matched string.
     *
     * @param icnName an icn name encoded into a string
     */
    ICNName(std::string icnName);

    /**
     * Destructor.
     */
    virtual ~ICNName() = default;

    /**
     * Parse the represented ICN name
     * to a string.
     *
     * @return std::string the parsed name as a string.
     */
    std::string generateString(void) const;

    /**
     * Returns true if this icn name matches another one. This
     * will take into account if one of them should be
     * prefix matched.
     *
     * @return bool
     */
    bool matches(ICNName& other) const;

    /**
     * Returns the number of levels this ICN name has.
     *
     * @return int number of levels.
     */
    int getNumberOfLevels(void) const;

    /**
     * Returns the levels of this icn name. This will not
     * include an asterisk if this is a prefix matched string.
     *
     * @return std::vector<std::string> containing all levels.
     */
    std::vector<std::string> getLevels(void) const;

    /**
     * Returns true if this icn name is matched by prefix.
     *
     * @return bool
     */
    bool isPrefixMatched(void) const;

    /**
     * Remove the last level from this icnname.
     * This will not modify this object and return a new ICNName. An exact
     * copy of this will be returned if there is no more level to remove.
     *
     * @return ICNName The new icn with the last level removed
     */
    ICNName removeLastLevel(void) const;

    /**
     * This will turn a non prefix matched name into a prefix matched
     * name.
     */
    ICNName makePrefixMatched(void) const;

    /**
     * Some ICNName have version numbers as their last level. This checks
     * if both have a version number and both match without it.
     */
    bool matchWithoutVersion(ICNName& other);

    /**
     * This checks if other has the higher version. This checks if both
     * have a version number at the end.
     */
    bool hasHigherVersion(ICNName& other);

    /**
     * Checks if the last level of this icnname has a version
     * number in it.
     */
    bool hasVersion(void);

private:

    /**
     * This stores the different levels of
     * the represented icn name.
     *
     * The first element is the highest in the
     * hierarchy.
     */
    std::vector<std::string> mLevelContainer;

    /**
     * Stores if this icnname is matched with the
     * prefix.
     */
    bool mIsPrefixMatched;

    /**
     * Compares two ICNNames and returns the
     * number of levels that match.
     */
    int compare(ICNName& other) const;

    /**
     * Same as matches but only compares the levels
     * until one of them has no elements left. If
     * everything matched until that point its fine.
     *
     * Note: It doesn't matter if other or this is
     * the prefix.
     *
     * Example:
     *      - /example/test partly matches /example/test/amazing
     *      - /example/test/amazing partly matches /example/test
     *      - /example/super/amazing not partly matches /example/test
     */
    bool prefixMatches(ICNName& other) const;

    /**
     * Check if this icn name matches another one.
     *
     * @return true if they match.
     */
    bool completeMatches(ICNName& other) const;


};

class ICNNameCompare {
public:
    bool operator()(const ICNName& lhs, const ICNName& rhs) const;
};

} /* namespace inet */

#endif /* INET_APPLICATIONS_ICN_ICNNAME_H_ */
