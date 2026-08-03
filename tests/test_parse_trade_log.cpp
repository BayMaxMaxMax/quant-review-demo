#include "quant_review/parse_trade_log.hpp"

#include <gtest/gtest.h>

using quant_review::ParseError;
using quant_review::TradeLogRow;
using quant_review::parse_trade_log_row;

TEST(ParseTradeLogRow, RealizedRowT001) {
  // From data/examples/b1-two-rows.csv — trailing comma => empty unrealized_pnl
  constexpr const char* kLine =
      "t001,2026-07-27T10:00:00,MOCK_FUT,long,close,100,1,2,2,6,";

  TradeLogRow row;
  ParseError err;
  ASSERT_TRUE(parse_trade_log_row(kLine, row, err)) << err.message;

  EXPECT_EQ(row.trade_id, "t001");
  EXPECT_EQ(row.datetime, "2026-07-27T10:00:00");
  EXPECT_EQ(row.symbol, "MOCK_FUT");
  EXPECT_EQ(row.direction, "long");
  EXPECT_EQ(row.offset, "close");
  EXPECT_DOUBLE_EQ(row.price, 100.0);
  EXPECT_DOUBLE_EQ(row.volume, 1.0);
  ASSERT_TRUE(row.commission_open.has_value());
  EXPECT_DOUBLE_EQ(*row.commission_open, 2.0);
  ASSERT_TRUE(row.commission_close.has_value());
  EXPECT_DOUBLE_EQ(*row.commission_close, 2.0);
  ASSERT_TRUE(row.realized_pnl.has_value());
  EXPECT_DOUBLE_EQ(*row.realized_pnl, 6.0);
  EXPECT_FALSE(row.unrealized_pnl.has_value());  // empty ≠ 0
}

TEST(ParseTradeLogRow, UnrealizedRowT002) {
  constexpr const char* kLine =
      "t002,2026-07-27T15:00:00,MOCK_FUT,long,open,110,1,2,,,10";

  TradeLogRow row;
  ParseError err;
  ASSERT_TRUE(parse_trade_log_row(kLine, row, err)) << err.message;

  EXPECT_EQ(row.trade_id, "t002");
  EXPECT_EQ(row.offset, "open");
  ASSERT_TRUE(row.commission_open.has_value());
  EXPECT_DOUBLE_EQ(*row.commission_open, 2.0);
  EXPECT_FALSE(row.commission_close.has_value());
  EXPECT_FALSE(row.realized_pnl.has_value());  // empty ≠ 0
  ASSERT_TRUE(row.unrealized_pnl.has_value());
  EXPECT_DOUBLE_EQ(*row.unrealized_pnl, 10.0);
}

TEST(ParseTradeLogRow, ExplicitZeroIsPresentNotEmpty) {
  // Teaching point: written "0" is a value; only blank means absent.
  constexpr const char* kLine =
      "t0,2026-07-27T10:00:00,MOCK_FUT,long,close,100,1,0,0,0,";

  TradeLogRow row;
  ParseError err;
  ASSERT_TRUE(parse_trade_log_row(kLine, row, err)) << err.message;
  ASSERT_TRUE(row.realized_pnl.has_value());
  EXPECT_DOUBLE_EQ(*row.realized_pnl, 0.0);
  EXPECT_FALSE(row.unrealized_pnl.has_value());
}

TEST(ParseTradeLogRow, RejectsWrongFieldCount) {
  TradeLogRow row;
  ParseError err;
  EXPECT_FALSE(parse_trade_log_row("a,b,c", row, err));
  EXPECT_FALSE(err.message.empty());
}
