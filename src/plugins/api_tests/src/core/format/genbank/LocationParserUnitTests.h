#ifndef _U2_GENBANK_LOCATION_PARSER_UNIT_TESTS_H_
#define _U2_GENBANK_LOCATION_PARSER_UNIT_TESTS_H_

#include <unittest.h>
#include <U2Core/U2OpStatusUtils.h>

namespace U2 {

DECLARE_TEST(LocationParserTestData, locationParser);
DECLARE_TEST(LocationParserTestData, locationParserEmpty);
DECLARE_TEST(LocationParserTestData, locationParserCompare);
DECLARE_TEST(LocationParserTestData, locationParserDuplicate);
DECLARE_TEST(LocationParserTestData, locationParserInvalid);
DECLARE_TEST(LocationParserTestData, hugeLocationParser);
DECLARE_TEST(LocationParserTestData, locationParserComplement);
DECLARE_TEST(LocationParserTestData, locationParserComplementInvalid);
DECLARE_TEST(LocationParserTestData, buildLocationString);
DECLARE_TEST(LocationParserTestData, buildLocationStringDuplicate);
DECLARE_TEST(LocationParserTestData, buildLocationStringInvalid);
DECLARE_TEST(LocationParserTestData, locationOperatorJoin);
DECLARE_TEST(LocationParserTestData, locationOperatorJoinInvalid);
DECLARE_TEST(LocationParserTestData, locationOperatorOrder);
DECLARE_TEST(LocationParserTestData, locationOperatorOrderInvalid);
DECLARE_TEST(LocationParserTestData, locationParserParenthesis);
DECLARE_TEST(LocationParserTestData, locationParserParenthesisInvalid);
DECLARE_TEST(LocationParserTestData, locationParserLeftParenthesisMissed);
DECLARE_TEST(LocationParserTestData, locationParserRightParenthesisMissed);
DECLARE_TEST(LocationParserTestData, locationParserPeriod);
DECLARE_TEST(LocationParserTestData, locationParserPeriodInvalid);
DECLARE_TEST(LocationParserTestData, locationParserDoublePeriodInvalid);
DECLARE_TEST(LocationParserTestData, locationParserCommaInvalid);
DECLARE_TEST(LocationParserTestData, locationParserNumberInvalid);
DECLARE_TEST(LocationParserTestData, locationParserLessInvalid);
DECLARE_TEST(LocationParserTestData, locationParserGreaterInvalid);
DECLARE_TEST(LocationParserTestData, locationParserName);
DECLARE_TEST(LocationParserTestData, locationParserNameInvalid);
DECLARE_TEST(LocationParserTestData, locationBuildStringNumberInvalid);
}

DECLARE_METATYPE(LocationParserTestData, locationParser);
DECLARE_METATYPE(LocationParserTestData, locationParserEmpty);
DECLARE_METATYPE(LocationParserTestData, locationParserCompare);
DECLARE_METATYPE(LocationParserTestData, locationParserDuplicate);
DECLARE_METATYPE(LocationParserTestData, locationParserInvalid);
DECLARE_METATYPE(LocationParserTestData, hugeLocationParser);
DECLARE_METATYPE(LocationParserTestData, locationParserComplement);
DECLARE_METATYPE(LocationParserTestData, locationParserComplementInvalid);
DECLARE_METATYPE(LocationParserTestData, buildLocationString);
DECLARE_METATYPE(LocationParserTestData, buildLocationStringDuplicate);
DECLARE_METATYPE(LocationParserTestData, buildLocationStringInvalid);
DECLARE_METATYPE(LocationParserTestData, locationOperatorJoin);
DECLARE_METATYPE(LocationParserTestData, locationOperatorJoinInvalid);
DECLARE_METATYPE(LocationParserTestData, locationOperatorOrder);
DECLARE_METATYPE(LocationParserTestData, locationOperatorOrderInvalid);
DECLARE_METATYPE(LocationParserTestData, locationParserParenthesis);
DECLARE_METATYPE(LocationParserTestData, locationParserParenthesisInvalid);
DECLARE_METATYPE(LocationParserTestData, locationParserLeftParenthesisMissed);
DECLARE_METATYPE(LocationParserTestData, locationParserRightParenthesisMissed);
DECLARE_METATYPE(LocationParserTestData, locationParserPeriod);
DECLARE_METATYPE(LocationParserTestData, locationParserPeriodInvalid);
DECLARE_METATYPE(LocationParserTestData, locationParserDoublePeriodInvalid);
DECLARE_METATYPE(LocationParserTestData, locationParserCommaInvalid);
DECLARE_METATYPE(LocationParserTestData, locationParserNumberInvalid);
DECLARE_METATYPE(LocationParserTestData, locationParserLessInvalid);
DECLARE_METATYPE(LocationParserTestData, locationParserGreaterInvalid);
DECLARE_METATYPE(LocationParserTestData, locationParserName);
DECLARE_METATYPE(LocationParserTestData, locationParserNameInvalid);
DECLARE_METATYPE(LocationParserTestData, locationBuildStringNumberInvalid);

#endif
