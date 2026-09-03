#include "quant_review/aggregate_trade_log.hpp"

#include <cctype>
#include <optional>
#include <string>

namespace quant_review {

namespace {

/// Parse hour 0–23 from `YYYY-MM-DDTHH:...` (or `... THH:...`).
std::optional<int> hour_from_datetime(const std::string& datetime) {
  const auto t_pos = datetime.find('T');
  if (t_pos == std::string::npos || t_pos + 2 >= datetime.size()) {
    return std::nullopt;
  }
  const char h0 = datetime[t_pos + 1];
  const char h1 = datetime[t_pos + 2];
  if (!std::isdigit(static_cast<unsigned char>(h0)) ||
      !std::isdigit(static_cast<unsigned char>(h1))) {
    return std::nullopt;
  }
  const int hour = (h0 - '0') * 10 + (h1 - '0');
  if (hour < 0 || hour > 23) {
    return std::nullopt;
  }
  return hour;
}

}  // namespace

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

WinRateReport win_rate_closed(const std::vector<TradeLogRow>& rows) {
  WinRateReport out;
  for (const TradeLogRow& row : rows) {
    if (!row.realized_pnl.has_value()) {
      continue;
    }
    ++out.closed_count;
    if (*row.realized_pnl > 0.0) {
      ++out.wins;
    }
  }
  return out;
}

ReviewSummaryReport report_review_summary(const std::vector<TradeLogRow>& rows) {
  const SeparatePnlReport pnl = report_separate_pnl(rows);
  const WinRateReport wr = win_rate_closed(rows);
  ReviewSummaryReport out;
  out.realized_total = pnl.realized_total;
  out.unrealized_hint_total = pnl.unrealized_hint_total;
  out.wins = wr.wins;
  out.closed_count = wr.closed_count;
  return out;
}

std::map<int, double> sum_realized_pnl_by_hour(
    const std::vector<TradeLogRow>& rows) {
  std::map<int, double> by_hour;
  for (const TradeLogRow& row : rows) {
    if (!row.realized_pnl.has_value()) {
      continue;
    }
    const std::optional<int> hour = hour_from_datetime(row.datetime);
    if (!hour.has_value()) {
      continue;
    }
    by_hour[*hour] += *row.realized_pnl;
  }
  return by_hour;
}

}  // namespace quant_review
