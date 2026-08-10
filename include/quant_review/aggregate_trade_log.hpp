#pragma once

#include "quant_review/trade_log_row.hpp"

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

/// Win-rate draft (Day14): denominator = rows with `realized_pnl` present;
/// wins = among those, `realized_pnl > 0`. Unrealized-only rows do not enter.
/// Zero realized (`== 0`) counts in the denominator but not as a win (scratch
/// controversy left open for a later note).
struct WinRateReport {
  std::size_t wins = 0;
  std::size_t closed_count = 0;
};

WinRateReport win_rate_closed(const std::vector<TradeLogRow>& rows);

}  // namespace quant_review
