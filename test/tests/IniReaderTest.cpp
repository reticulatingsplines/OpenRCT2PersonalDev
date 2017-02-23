#include <gtest/gtest.h>
#include <limits>
#include "openrct2/config/ConfigEnum.hpp"
#include "openrct2/config/IniReader.hpp"
#include "openrct2/core/MemoryStream.h"
#include "openrct2/core/Util.hpp"

class IniReaderTest : public testing::Test
{
protected:
    static const utf8 predefined[];
};

static auto Enum_Currency = ConfigEnum<sint32>({});

TEST_F(IniReaderTest, create_empty)
{
    MemoryStream * ms = new MemoryStream(0);
    ASSERT_NE(ms, nullptr);
    ASSERT_EQ(ms->CanRead(), true);
    ASSERT_EQ(ms->CanWrite(), true);
    IIniReader * ir = CreateIniReader(ms);
    ASSERT_NE(ir, nullptr);
    ASSERT_EQ(ir->GetBoolean("nobody", true), true);
    ASSERT_EQ(ir->GetCString("expects", nullptr), nullptr);
    ASSERT_EQ(ir->GetEnum<sint32>("spanish", 12345, Enum_Currency), 12345);
    ASSERT_EQ(ir->GetFloat("inquisition", 1.234f), 1.234f);
    ASSERT_EQ(ir->GetSint32("universal_answer", 42), 42);
    delete ir;
    delete ms;
}

TEST_F(IniReaderTest, read_prepared)
{
    MemoryStream * ms = new MemoryStream(predefined, 99);
    ASSERT_NE(ms, nullptr);
    ASSERT_EQ(ms->CanRead(), true);
    ASSERT_EQ(ms->CanWrite(), false);
    IIniReader * ir = CreateIniReader(ms);
    ASSERT_NE(ir, nullptr);
    ASSERT_EQ(ir->ReadSection("doesnt_exist"), false);
    ASSERT_EQ(ir->ReadSection("bool"), true);
    // name of section
    ASSERT_EQ(ir->GetSint32("bool", 42), 42);
    // value from different section
    ASSERT_EQ(ir->GetSint32("one", 42), 42);
    // existing value as different type
    EXPECT_THROW(ir->GetSint32("boolval", 42), std::invalid_argument);
    ASSERT_EQ(ir->GetBoolean("boolval", false), true);
    // skip one section
    ASSERT_EQ(ir->ReadSection("string"), true);
    // values from different sections
    ASSERT_EQ(ir->GetSint32("one", 42), 42);
    ASSERT_EQ(ir->GetBoolean("boolval", false), true);
    const utf8 * str = ir->GetCString("path", nullptr);
    ASSERT_STREQ(str, u8"C:'\\some/dir\\here/神鷹暢遊");
    Memory::Free(str);
    // go backa section
    ASSERT_EQ(ir->ReadSection("int"), true);
    ASSERT_EQ(ir->GetSint32("one", 42), 1);
    delete ir;
    delete ms;
}

const utf8 IniReaderTest::predefined[] = "[bool]\nboolval = true\n\n[int]\none = 1\nzero = 0\n\n[string]\npath = "
                                         "\"C:'\\\\some/dir\\\\here/\xE7\xA5\x9E\xE9\xB7\xB9\xE6\x9A\xA2\xE9\x81\x8A\"\n";
