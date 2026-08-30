#pragma once

#include "quant_review/parse_trade_log.hpp"
#include "quant_review/trade_log_row.hpp"

#include <string>
#include <vector>

namespace quant_review {

struct LoadError {
  std::string message;
};

/// Load a whole mock trade-log CSV into a row list (Day19).
/// Skips the first non-empty line as header; skips blank lines; parses each
/// data line with `parse_trade_log_row`. Does **not** run validate / dirty-row
/// gates (Day20) and does **not** compute a review summary (Day18).
/// Returns false and fills `err` on I/O or parse failure; `out` is unspecified
/// then.
bool load_trade_log_csv(const std::string& path, std::vector<TradeLogRow>& out,
                        LoadError& err);

}  // namespace quant_review
