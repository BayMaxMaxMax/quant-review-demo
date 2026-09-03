#pragma once

#include "quant_review/trade_log_row.hpp"

#include <map>
#include <vector>

namespace quant_review {

/// Sum `realized_pnl` over rows where it is present.
/// Rows with only `unrealized_pnl` (or neither) contribute nothing.
/// Does not validate direction/offset; call validate first in a full pipeline.
double sum_realized_pnl(const std::vector<TradeLogRow>& rows);

/// Sum `unrealized_pnl` over rows where it is present (floating hint only).
/// Never treat this total as a settled / closed-round result.
double sum_unrealized_pnl(const std::vector<TradeLogRow>& rows);

/// Two-bucket report: realized total + unrealized hint total (kept separate).
struct SeparatePnlReport {
  double realized_total = 0.0;
  double unrealized_hint_total = 0.0;
};

SeparatePnlReport report_separate_pnl(const std::vector<TradeLogRow>& rows);

/// Win-rate draft (Day14–17): denominator = rows with `realized_pnl` present;
/// wins = among those, `realized_pnl > 0`. Unrealized-only rows do not enter.
/// Zero realized (`== 0`, scratch) counts in the denominator but not as a win
/// (Day15 locked: 进分母、不计胜).
/// Empty / no closed rows → wins=0, closed_count=0 (Day16: report 0/0, not 0%).
/// Closed rows with zero wins → 0/n (Day17: all-loss may be said as 0%; still
/// not 0/0).
struct WinRateReport {
  std::size_t wins = 0;
  std::size_t closed_count = 0;
};

WinRateReport win_rate_closed(const std::vector<TradeLogRow>& rows);

/// One-shot review summary (Day18): two buckets + closed win-rate in one report.
/// Composes `report_separate_pnl` and `win_rate_closed` — does not change those
/// rules. Does not read files (`load_trade_log_csv`, Day19). Call
/// `gate_rows_for_report` first (Day20) before treating a list as reportable.
/// Day14 teaching mock → realized +2, unrealized hint +10, wins/closed 1/2.
struct ReviewSummaryReport {
  double realized_total = 0.0;
  double unrealized_hint_total = 0.0;
  std::size_t wins = 0;
  std::size_t closed_count = 0;
};

ReviewSummaryReport report_review_summary(const std::vector<TradeLogRow>& rows);

/// Hourly realized totals (Day21): key = hour-of-day 0–23 from `datetime`
/// (`...THH:...`); only rows with `realized_pnl` present enter that hour's sum.
/// Unrealized-only rows do not create or increase an hour bucket.
/// Datetimes without a parseable `THH` are skipped (teaching ISO-8601 mocks).
/// Day21 mock: 10:00 closed +6 → {10: 6}; 15:00 open +10 → hour 15 absent.
std::map<int, double> sum_realized_pnl_by_hour(
    const std::vector<TradeLogRow>& rows);

}  // namespace quant_review
