#include "quant_review/parse_trade_log.hpp"
#include "quant_review/validate_trade_log.hpp"

#include <gtest/gtest.h>

using quant_review::ParseError;
using quant_review::TradeLogRow;
using quant_review::ValidationError;
using quant_review::parse_trade_log_row;
using quant_review::validate_direction_and_offset;

namespace {

TradeLogRow MustParse(const char* line) {
  TradeLogRow row;
  ParseError err;
  EXPECT_TRUE(parse_trade_log_row(line, row, err)) << err.message;
  return row;
}

}  // namespace

TEST(ValidateDirectionOffset, AcceptsRealizedT001) {
  const TradeLogRow row = MustParse(
      "t001,2026-07-27T10:00:00,MOCK_FUT,long,close,100,1,2,2,6,");
  ValidationError err;
  EXPECT_TRUE(validate_direction_and_offset(row, err)) << err.message;
}

TEST(ValidateDirectionOffset, AcceptsUnrealizedT002) {
  const TradeLogRow row = MustParse(
      "t002,2026-07-27T15:00:00,MOCK_FUT,long,open,110,1,2,,,10");
  ValidationError err;
  EXPECT_TRUE(validate_direction_and_offset(row, err)) << err.message;
}

TEST(ValidateDirectionOffset, RejectsInvalidDirection) {
  TradeLogRow row = MustParse(
      "tbad,2026-07-27T10:00:00,MOCK_FUT,long,close,100,1,2,2,6,");
  row.direction = "buy";
  ValidationError err;
  EXPECT_FALSE(validate_direction_and_offset(row, err));
  EXPECT_NE(err.message.find("direction"), std::string::npos);
}

TEST(ValidateDirectionOffset, RejectsInvalidOffset) {
  TradeLogRow row = MustParse(
      "tbad,2026-07-27T10:00:00,MOCK_FUT,long,close,100,1,2,2,6,");
  row.offset = "flat";
  ValidationError err;
  EXPECT_FALSE(validate_direction_and_offset(row, err));
  EXPECT_NE(err.message.find("offset"), std::string::npos);
}

TEST(ValidateDirectionOffset, RejectsRealizedWithOpen) {
  // Teaching anti-example: 已平数字却标开仓 → 开平对不上
  TradeLogRow row = MustParse(
      "tbad,2026-07-27T10:00:00,MOCK_FUT,long,close,100,1,2,2,6,");
  row.offset = "open";
  ValidationError err;
  EXPECT_FALSE(validate_direction_and_offset(row, err));
  EXPECT_NE(err.message.find("offset=close"), std::string::npos);
}

TEST(ValidateDirectionOffset, RejectsUnrealizedWithClose) {
  TradeLogRow row = MustParse(
      "tbad,2026-07-27T15:00:00,MOCK_FUT,long,open,110,1,2,,,10");
  row.offset = "close";
  ValidationError err;
  EXPECT_FALSE(validate_direction_and_offset(row, err));
  EXPECT_NE(err.message.find("offset=open"), std::string::npos);
}
