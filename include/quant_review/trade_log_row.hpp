#pragma once

#include <optional>
#include <string>

namespace quant_review {

/// One row of the trade journal CSV (schema locked in data/README.md).
/// Optional numeric fields: empty CSV cell -> std::nullopt (空 ≠ 0).
struct TradeLogRow {
  std::string trade_id;
  std::string datetime;
  std::string symbol;
  std::string direction;  // long | short
  std::string offset;     // open | close
  double price = 0.0;
  double volume = 0.0;
  std::optional<double> commission_open;
  std::optional<double> commission_close;
  std::optional<double> realized_pnl;
  std::optional<double> unrealized_pnl;
};

}  // namespace quant_review
