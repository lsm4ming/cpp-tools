#include <gtest/gtest.h>
#include "json/jsonparse.h"

TEST(CppToolsJsonTest, JsonValue)
{
    cpptools::json::JsonValue value;
    value = "1";
    EXPECT_EQ(value.getType(), cpptools::json::JsonToken::StringValue);
    value = 1;
    EXPECT_EQ(value.getType(), cpptools::json::JsonToken::IntValue);
    value = 1.0;
    EXPECT_EQ(value.getType(), cpptools::json::JsonToken::DoubleValue);
    value = true;
    EXPECT_EQ(value.getType(), cpptools::json::JsonToken::BoolValue);
    value = nullptr;
    EXPECT_EQ(value.getType(), cpptools::json::JsonToken::NullValue);
    value = cpptools::json::JsonArray::newArray();
    EXPECT_EQ(value.getType(), cpptools::json::JsonToken::ArrayValue);
    value = cpptools::json::JsonObject::newObject();
    EXPECT_EQ(value.getType(), cpptools::json::JsonToken::ObjectValue);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}