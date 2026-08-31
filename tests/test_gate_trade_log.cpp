#include "quant_review/aggregate_trade_log.hpp"
#include "quant_review/parse_trade_log.hpp"
#include "quant_review/validate_trade_log.hpp"

#include <gtest/gtest.h>

#include <vector>

using quant_review::ParseError;
using quant_review::TradeLogRow;
using quant_review::ValidationError;
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

std::vector<TradeLogRow> CleanB1TwoRows() {
  return {
      MustParse("t001,2026-07-27T10:00:00,MOCK_FUT,long,close,100,1,2,2,6,"),
      MustParse("t002,2026-07-27T15:00:00,MOCK_FUT,long,open,110,1,2,,,10"),
  };
}

}  // namespace

TEST(GateRowsForReport, CleanB1TwoRowsPassesGate) {
  // Day20 lock: clean list → gate opens; report is allowed afterward.
  const std::vector<TradeLogRow> rows = CleanB1TwoRows();
  ValidationError err;
  ASSERT_TRUE(gate_rows_for_report(rows, err)) << err.message;

  const auto report = report_review_summary(rows);
  EXPECT_DOUBLE_EQ(report.realized_total, 6.0);
  EXPECT_DOUBLE_EQ(report.unrealized_hint_total, 10.0);
  EXPECT_EQ(report.wins, 1u);
  EXPECT_EQ(report.closed_count, 1u);
}

TEST(GateRowsForReport, DirtyRealizedWithOpenFailsGate) {
  // Day20 lock: any dirty row → whole list fails; do not produce a report.
  std::vector<TradeLogRow> rows = CleanB1TwoRows();
  TradeLogRow dirty = MustParse(
      "tbad,2026-07-27T10:00:00,MOCK_FUT,long,close,100,1,2,2,6,");
  dirty.offset = "open";  // 已平有值却标开仓
  rows.push_back(dirty);

  ValidationError err;
  ASSERT_FALSE(gate_rows_for_report(rows, err));
  EXPECT_NE(err.message.find("row 2"), std::string::npos);
  EXPECT_NE(err.message.find("offset=close"), std::string::npos);
  // Teaching point: gate failed → caller must not treat this list as reportable.
  // (We deliberately do not call report_review_summary here.)
}

TEST(GateRowsForReport, EmptyListPassesGate) {
  // No dirty rows present → gate opens (vacuous); report stays empty 0/0.
  std::vector<TradeLogRow> rows;
  ValidationError err;
  ASSERT_TRUE(gate_rows_for_report(rows, err)) << err.message;
}
