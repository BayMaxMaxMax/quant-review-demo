#include "quant_review/aggregate_trade_log.hpp"

namespace quant_review {

double sum_realized_pnl(const std::vector<TradeLogRow>& rows) {
  double total = 0.0;
  for (const TradeLogRow& row : rows) {
    if (row.realized_pnl.has_value()) {
      total += *row.realized_pnl;
    }
  }
  return total;
}

}  // namespace quant_review
