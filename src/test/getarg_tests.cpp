#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-CWY");
    BOOST_CHECK(GetBoolArg("-CWY"));
    BOOST_CHECK(GetBoolArg("-CWY", false));
    BOOST_CHECK(GetBoolArg("-CWY", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-CWYo"));
    BOOST_CHECK(!GetBoolArg("-CWYo", false));
    BOOST_CHECK(GetBoolArg("-CWYo", true));

    ResetArgs("-CWY=0");
    BOOST_CHECK(!GetBoolArg("-CWY"));
    BOOST_CHECK(!GetBoolArg("-CWY", false));
    BOOST_CHECK(!GetBoolArg("-CWY", true));

    ResetArgs("-CWY=1");
    BOOST_CHECK(GetBoolArg("-CWY"));
    BOOST_CHECK(GetBoolArg("-CWY", false));
    BOOST_CHECK(GetBoolArg("-CWY", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noCWY");
    BOOST_CHECK(!GetBoolArg("-CWY"));
    BOOST_CHECK(!GetBoolArg("-CWY", false));
    BOOST_CHECK(!GetBoolArg("-CWY", true));

    ResetArgs("-noCWY=1");
    BOOST_CHECK(!GetBoolArg("-CWY"));
    BOOST_CHECK(!GetBoolArg("-CWY", false));
    BOOST_CHECK(!GetBoolArg("-CWY", true));

    ResetArgs("-CWY -noCWY");  // -CWY should win
    BOOST_CHECK(GetBoolArg("-CWY"));
    BOOST_CHECK(GetBoolArg("-CWY", false));
    BOOST_CHECK(GetBoolArg("-CWY", true));

    ResetArgs("-CWY=1 -noCWY=1");  // -CWY should win
    BOOST_CHECK(GetBoolArg("-CWY"));
    BOOST_CHECK(GetBoolArg("-CWY", false));
    BOOST_CHECK(GetBoolArg("-CWY", true));

    ResetArgs("-CWY=0 -noCWY=0");  // -CWY should win
    BOOST_CHECK(!GetBoolArg("-CWY"));
    BOOST_CHECK(!GetBoolArg("-CWY", false));
    BOOST_CHECK(!GetBoolArg("-CWY", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--CWY=1");
    BOOST_CHECK(GetBoolArg("-CWY"));
    BOOST_CHECK(GetBoolArg("-CWY", false));
    BOOST_CHECK(GetBoolArg("-CWY", true));

    ResetArgs("--noCWY=1");
    BOOST_CHECK(!GetBoolArg("-CWY"));
    BOOST_CHECK(!GetBoolArg("-CWY", false));
    BOOST_CHECK(!GetBoolArg("-CWY", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-CWY", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CWY", "eleven"), "eleven");

    ResetArgs("-CWY -bar");
    BOOST_CHECK_EQUAL(GetArg("-CWY", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CWY", "eleven"), "");

    ResetArgs("-CWY=");
    BOOST_CHECK_EQUAL(GetArg("-CWY", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CWY", "eleven"), "");

    ResetArgs("-CWY=11");
    BOOST_CHECK_EQUAL(GetArg("-CWY", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-CWY", "eleven"), "11");

    ResetArgs("-CWY=eleven");
    BOOST_CHECK_EQUAL(GetArg("-CWY", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-CWY", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-CWY", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-CWY", 0), 0);

    ResetArgs("-CWY -bar");
    BOOST_CHECK_EQUAL(GetArg("-CWY", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-CWY=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-CWY", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-CWY=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-CWY", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--CWY");
    BOOST_CHECK_EQUAL(GetBoolArg("-CWY"), true);

    ResetArgs("--CWY=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-CWY", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noCWY");
    BOOST_CHECK(!GetBoolArg("-CWY"));
    BOOST_CHECK(!GetBoolArg("-CWY", true));
    BOOST_CHECK(!GetBoolArg("-CWY", false));

    ResetArgs("-noCWY=1");
    BOOST_CHECK(!GetBoolArg("-CWY"));
    BOOST_CHECK(!GetBoolArg("-CWY", true));
    BOOST_CHECK(!GetBoolArg("-CWY", false));

    ResetArgs("-noCWY=0");
    BOOST_CHECK(GetBoolArg("-CWY"));
    BOOST_CHECK(GetBoolArg("-CWY", true));
    BOOST_CHECK(GetBoolArg("-CWY", false));

    ResetArgs("-CWY --noCWY");
    BOOST_CHECK(GetBoolArg("-CWY"));

    ResetArgs("-noCWY -CWY"); // CWY always wins:
    BOOST_CHECK(GetBoolArg("-CWY"));
}

BOOST_AUTO_TEST_SUITE_END()
