#pragma once

#include "quant_review/trade_log_row.hpp"

#include <string>
#include <string_view>

namespace quant_review {

struct ParseError {
  std::string message;
};

/// Parse one data row (no header). Expects exactly 11 comma-separated fields
/// in schema order. Direction / offset / PnL-bucket checks belong to
/// validate_direction_and_offset (B3), not parse.
/// Returns false and fills `err` on failure; `out` is unspecified then.
bool parse_trade_log_row(std::string_view line, TradeLogRow& out, ParseError& err);

}  // namespace quant_review
