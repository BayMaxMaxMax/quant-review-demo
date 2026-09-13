#include "quant_review/flag_anomaly.hpp"

namespace quant_review {

std::vector<AnomalyFlag> flag_consecutive_closed_losses(
    const std::vector<TradeLogRow>& rows, std::size_t n) {
  std::vector<AnomalyFlag> out;
  if (n == 0) {
    return out;
  }

  std::size_t streak = 0;
  for (const TradeLogRow& row : rows) {
    if (!row.realized_pnl.has_value()) {
      continue;  // unrealized-only: ignore for this rule
    }
    if (*row.realized_pnl < 0.0) {
      ++streak;
      if (streak >= n) {
        AnomalyFlag flag;
        flag.code = "consecutive_closed_losses";
        flag.message = "consecutive closed losses >= " + std::to_string(n);
        out.push_back(std::move(flag));
        return out;  // one tip only
      }
    } else {
      streak = 0;  // win or scratch resets
    }
  }
  return out;
}

}  // namespace quant_review
