#include "quant_review/aggregate_trade_log.hpp"
#include "quant_review/parse_trade_log.hpp"

#include <gtest/gtest.h>

#include <vector>

using quant_review::ParseError;
using quant_review::SeparatePnlReport;
using quant_review::TradeLogRow;
using quant_review::WinRateReport;
using quant_review::parse_trade_log_row;
using quant_review::report_separate_pnl;
using quant_review::sum_realized_pnl;
using quant_review::sum_unrealized_pnl;
using quant_review::win_rate_closed;

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

TEST(SumUnrealizedPnl, TwoRowFixtureSumsOnlyUnrealized) {
  // Day13: floating hint +10; realized row does not enter this bucket
  const std::vector<TradeLogRow> rows = {
      MustParse("t001,2026-07-27T10:00:00,MOCK_FUT,long,close,100,1,2,2,6,"),
      MustParse("t002,2026-07-27T15:00:00,MOCK_FUT,long,open,110,1,2,,,10"),
  };
  EXPECT_DOUBLE_EQ(sum_unrealized_pnl(rows), 10.0);
}

TEST(SumUnrealizedPnl, OnlyRealizedIsZero) {
  const std::vector<TradeLogRow> rows = {
      MustParse("t001,2026-07-27T10:00:00,MOCK_FUT,long,close,100,1,2,2,6,"),
  };
  EXPECT_DOUBLE_EQ(sum_unrealized_pnl(rows), 0.0);
}

TEST(ReportSeparatePnl, TwoBucketsNotMixed) {
  // Aligns Day13 mock: report +6 settled and +10 floating separately (not one +16)
  const std::vector<TradeLogRow> rows = {
      MustParse("t001,2026-07-27T10:00:00,MOCK_FUT,long,close,100,1,2,2,6,"),
      MustParse("t002,2026-07-27T15:00:00,MOCK_FUT,long,open,110,1,2,,,10"),
  };
  const SeparatePnlReport report = report_separate_pnl(rows);
  EXPECT_DOUBLE_EQ(report.realized_total, 6.0);
  EXPECT_DOUBLE_EQ(report.unrealized_hint_total, 10.0);
  EXPECT_FALSE(report.realized_total == 16.0);
  EXPECT_FALSE(report.unrealized_hint_total == 16.0);
}

TEST(ReportSeparatePnl, EmptyBothZero) {
  const SeparatePnlReport report = report_separate_pnl({});
  EXPECT_DOUBLE_EQ(report.realized_total, 0.0);
  EXPECT_DOUBLE_EQ(report.unrealized_hint_total, 0.0);
}

TEST(WinRateClosed, Day14MockIsOneHalfNotOneThird) {
  // Day14: closed +6 win, closed -4 loss, open +10 skipped → 1/2 not 1/3
  const std::vector<TradeLogRow> rows = {
      MustParse("t001,2026-07-27T10:00:00,MOCK_FUT,long,close,100,1,2,2,6,"),
      MustParse("t002,2026-07-27T11:00:00,MOCK_FUT,long,close,100,1,2,2,-4,"),
      MustParse("t003,2026-07-27T15:00:00,MOCK_FUT,long,open,110,1,2,,,10"),
  };
  const WinRateReport report = win_rate_closed(rows);
  EXPECT_EQ(report.wins, 1u);
  EXPECT_EQ(report.closed_count, 2u);
  EXPECT_FALSE(report.closed_count == 3u);
}

TEST(WinRateClosed, Day16EmptyIsZeroOverZeroNotZeroPercent) {
  // Day16: no closed rows → 0/0 (no denominator), do not invent 0%
  const WinRateReport report = win_rate_closed({});
  EXPECT_EQ(report.wins, 0u);
  EXPECT_EQ(report.closed_count, 0u);
}

TEST(WinRateClosed, Day16OnlyUnrealizedIsZeroOverZero) {
  // Day16: unrealized-only table still has no closed denominator → 0/0
  const std::vector<TradeLogRow> rows = {
      MustParse("t003,2026-07-27T15:00:00,MOCK_FUT,long,open,110,1,2,,,10"),
  };
  const WinRateReport report = win_rate_closed(rows);
  EXPECT_EQ(report.wins, 0u);
  EXPECT_EQ(report.closed_count, 0u);
}

TEST(WinRateClosed, Day15ScratchInDenomNotWin) {
  // Day15: closed +6 win, closed -4 loss, closed 0 scratch → 1/3 not 1/2
  // Scratch enters denominator, does not count as a win.
  const std::vector<TradeLogRow> rows = {
      MustParse("t001,2026-07-27T10:00:00,MOCK_FUT,long,close,100,1,2,2,6,"),
      MustParse("t002,2026-07-27T11:00:00,MOCK_FUT,long,close,100,1,2,2,-4,"),
      MustParse("t003,2026-07-27T12:00:00,MOCK_FUT,long,close,100,1,2,2,0,"),
  };
  const WinRateReport report = win_rate_closed(rows);
  EXPECT_EQ(report.wins, 1u);
  EXPECT_EQ(report.closed_count, 3u);
  EXPECT_FALSE(report.closed_count == 2u);
}
