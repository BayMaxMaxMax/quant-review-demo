#pragma once

#include "quant_review/trade_log_row.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace quant_review {

/// One static anomaly tip (Day24): mark for a human to read — does **not** gate
/// the report (contrast Day20 `gate_rows_for_report`).
struct AnomalyFlag {
  std::string code;
  std::string message;
};

/// Flag consecutive closed losses among rows with `realized_pnl` present.
/// Unrealized-only rows are skipped (do not break or extend the streak).
/// A closed win (`> 0`) or scratch (`== 0`) resets the streak.
/// When the streak reaches `n` (default 2), emit **exactly one** flag and stop.
/// Empty / fewer than `n` closed losses → empty vector.
/// Does **not** call the dirty-row gate; callers may still `report_review_summary`.
std::vector<AnomalyFlag> flag_consecutive_closed_losses(
    const std::vector<TradeLogRow>& rows, std::size_t n = 2);

}  // namespace quant_review
