#pragma once

#include "quant_review/trade_log_row.hpp"

#include <string>

namespace quant_review {

struct ValidationError {
  std::string message;
};

/// Check direction / offset enums and that open-close matches the review
/// bucket when a PnL column is present (realized → close, unrealized → open).
/// Does not aggregate; mutual exclusion of both PnL columns is only lightly
/// reported when both are set (reject). Neither set → reject (no bucket).
bool validate_direction_and_offset(const TradeLogRow& row, ValidationError& err);

}  // namespace quant_review
