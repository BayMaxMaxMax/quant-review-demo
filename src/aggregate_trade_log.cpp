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

double sum_unrealized_pnl(const std::vector<TradeLogRow>& rows) {
  double total = 0.0;
  for (const TradeLogRow& row : rows) {
    if (row.unrealized_pnl.has_value()) {
      total += *row.unrealized_pnl;
    }
  }
  return total;
}

SeparatePnlReport report_separate_pnl(const std::vector<TradeLogRow>& rows) {
  SeparatePnlReport out;
  out.realized_total = sum_realized_pnl(rows);
  out.unrealized_hint_total = sum_unrealized_pnl(rows);
  return out;
}

}  // namespace quant_review
