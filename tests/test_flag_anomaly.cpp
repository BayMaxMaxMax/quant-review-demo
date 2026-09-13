#include "quant_review/aggregate_trade_log.hpp"
#include "quant_review/flag_anomaly.hpp"
#include "quant_review/parse_trade_log.hpp"
#include "quant_review/validate_trade_log.hpp"

#include <gtest/gtest.h>

#include <string>
#include <vector>

using quant_review::AnomalyFlag;
using quant_review::ParseError;
using quant_review::TradeLogRow;
using quant_review::ValidationError;
using quant_review::flag_consecutive_closed_losses;
using quant_review::gate_rows_for_report;
using quant_review::parse_trade_log_row;
using quant_review::report_review_summary;

namespace {

TradeLogRow MustParse(const char* line) {
  TradeLogRow row;
  ParseError err;
  EXPECT_TRUE(parse_trade_log_row(line, row, err)) << err.message;
  return row;
}

TradeLogRow Closed(const char* id, double realized) {
  // Minimal closed row via CSV; unrealized cell empty.
  const std::string line =
      std::string(id) + ",2026-09-13T10:00:00,MOCK_FUT,long,close,100,1,2,2," +
      std::to_string(static_cast<int>(realized)) + ",";
  return MustParse(line.c_str());
}

TradeLogRow OpenUnrealized(const char* id, double unrealized) {
  const std::string line =
      std::string(id) + ",2026-09-13T15:00:00,MOCK_FUT,long,open,110,1,2,,," +
      std::to_string(static_cast<int>(unrealized));
  return MustParse(line.c_str());
}

}  // namespace

TEST(FlagConsecutiveClosedLosses, Day24MockTwoLossesFiresOneFlag) {
  // Day24 lock: closed -3 then -2 → one static tip; report still allowed.
  const std::vector<TradeLogRow> rows = {
      Closed("t_loss1", -3),
      Closed("t_loss2", -2),
  };

  const std::vector<AnomalyFlag> flags = flag_consecutive_closed_losses(rows, 2);
  ASSERT_EQ(flags.size(), 1u);
  EXPECT_EQ(flags[0].code, "consecutive_closed_losses");
  EXPECT_NE(flags[0].message.find(">= 2"), std::string::npos);

  // Contrast Day20: flag ≠ gate — summary still comes out.
  ValidationError err;
  ASSERT_TRUE(gate_rows_for_report(rows, err)) << err.message;
  const auto report = report_review_summary(rows);
  EXPECT_DOUBLE_EQ(report.realized_total, -5.0);
  EXPECT_EQ(report.wins, 0u);
  EXPECT_EQ(report.closed_count, 2u);
}

TEST(FlagConsecutiveClosedLosses, CleanClosedWinsHaveNoFlag) {
  const std::vector<TradeLogRow> rows = {
      Closed("t_win1", 6),
      Closed("t_win2", 2),
  };
  EXPECT_TRUE(flag_consecutive_closed_losses(rows, 2).empty());
}

TEST(FlagConsecutiveClosedLosses, WinBetweenLossesResetsStreak) {
  // -3, +5, -2 → never two consecutive closed losses.
  const std::vector<TradeLogRow> rows = {
      Closed("t_a", -3),
      Closed("t_b", 5),
      Closed("t_c", -2),
  };
  EXPECT_TRUE(flag_consecutive_closed_losses(rows, 2).empty());
}

TEST(FlagConsecutiveClosedLosses, UnrealizedBetweenLossesDoesNotBreak) {
  // Rule looks at closed only; floating row is skipped.
  const std::vector<TradeLogRow> rows = {
      Closed("t_a", -3),
      OpenUnrealized("t_open", 10),
      Closed("t_b", -2),
  };
  const std::vector<AnomalyFlag> flags = flag_consecutive_closed_losses(rows, 2);
  ASSERT_EQ(flags.size(), 1u);
  EXPECT_EQ(flags[0].code, "consecutive_closed_losses");
}

TEST(FlagConsecutiveClosedLosses, ThreeLossesStillOneFlag) {
  const std::vector<TradeLogRow> rows = {
      Closed("t_a", -1),
      Closed("t_b", -1),
      Closed("t_c", -1),
  };
  EXPECT_EQ(flag_consecutive_closed_losses(rows, 2).size(), 1u);
}

TEST(FlagConsecutiveClosedLosses, EmptyOrSingleLossHasNoFlag) {
  EXPECT_TRUE(flag_consecutive_closed_losses({}, 2).empty());
  EXPECT_TRUE(flag_consecutive_closed_losses({Closed("t_a", -3)}, 2).empty());
}

TEST(FlagConsecutiveClosedLosses, ScratchResetsStreak) {
  // Scratch (0) is closed but not a loss → resets.
  const std::vector<TradeLogRow> rows = {
      Closed("t_a", -3),
      Closed("t_scratch", 0),
      Closed("t_b", -2),
  };
  EXPECT_TRUE(flag_consecutive_closed_losses(rows, 2).empty());
}
