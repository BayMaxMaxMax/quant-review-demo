#include "quant_review/aggregate_trade_log.hpp"
#include "quant_review/parse_trade_log.hpp"

#include <gtest/gtest.h>

#include <vector>

using quant_review::ParseError;
using quant_review::TradeLogRow;
using quant_review::parse_trade_log_row;
using quant_review::sum_realized_pnl;

namespace {

TradeLogRow MustParse(const char* line) {
  TradeLogRow row;
  ParseError err;
  EXPECT_TRUE(parse_trade_log_row(line, row, err)) << err.message;
  return row;
}

}  // namespace

TEST(SumRealizedPnl, TwoRowFixtureSumsOnlyRealized) {
  // Aligns Day12 mock: t001 +6 in; t002 unrealized +10 skipped → total +6
  const std::vector<TradeLogRow> rows = {
      MustParse("t001,2026-07-27T10:00:00,MOCK_FUT,long,close,100,1,2,2,6,"),
      MustParse("t002,2026-07-27T15:00:00,MOCK_FUT,long,open,110,1,2,,,10"),
  };
  EXPECT_DOUBLE_EQ(sum_realized_pnl(rows), 6.0);
}

TEST(SumRealizedPnl, EmptyIsZero) {
  EXPECT_DOUBLE_EQ(sum_realized_pnl({}), 0.0);
}

TEST(SumRealizedPnl, ThreeRealizedLikeDay7) {
  const std::vector<TradeLogRow> rows = {
      MustParse("t1,2026-07-27T10:00:00,MOCK_FUT,long,close,100,1,2,2,6,"),
      MustParse("t2,2026-07-27T11:00:00,MOCK_FUT,long,close,100,1,2,2,-2,"),
      MustParse("t3,2026-07-27T12:00:00,MOCK_FUT,long,close,100,1,2,2,4,"),
  };
  EXPECT_DOUBLE_EQ(sum_realized_pnl(rows), 8.0);
}

TEST(SumRealizedPnl, OnlyUnrealizedIsZero) {
  const std::vector<TradeLogRow> rows = {
      MustParse("t002,2026-07-27T15:00:00,MOCK_FUT,long,open,110,1,2,,,10"),
  };
  EXPECT_DOUBLE_EQ(sum_realized_pnl(rows), 0.0);
}

TEST(SumRealizedPnl, ZeroRealizedStillCounts) {
  // 空 ≠ 0：单元格写 0 表示有值 0，应进总和
  const std::vector<TradeLogRow> rows = {
      MustParse("tz,2026-07-27T10:00:00,MOCK_FUT,long,close,100,1,2,2,0,"),
      MustParse("t002,2026-07-27T15:00:00,MOCK_FUT,long,open,110,1,2,,,10"),
  };
  EXPECT_DOUBLE_EQ(sum_realized_pnl(rows), 0.0);
}
