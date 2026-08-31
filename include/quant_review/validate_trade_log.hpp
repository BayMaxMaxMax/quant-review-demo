#pragma once

#include "quant_review/trade_log_row.hpp"

#include <string>
#include <vector>

namespace quant_review {

struct ValidationError {
  std::string message;
};

/// Check direction / offset enums and that open-close matches the review
/// bucket when a PnL column is present (realized → close, unrealized → open).
/// Does not aggregate; mutual exclusion of both PnL columns is only lightly
/// reported when both are set (reject). Neither set → reject (no bucket).
bool validate_direction_and_offset(const TradeLogRow& row, ValidationError& err);

/// Day20 dirty-row gate before a review report: run
/// `validate_direction_and_offset` on every row. Any failure → return false
/// (**do not** produce a report). Does not skip dirty rows and continue.
/// Does not call `report_review_summary` itself.
bool gate_rows_for_report(const std::vector<TradeLogRow>& rows,
                          ValidationError& err);

}  // namespace quant_review
