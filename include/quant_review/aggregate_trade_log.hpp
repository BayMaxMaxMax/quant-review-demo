#pragma once

#include "quant_review/trade_log_row.hpp"

#include <vector>

namespace quant_review {

/// Sum `realized_pnl` over rows where it is present.
/// Rows with only `unrealized_pnl` (or neither) contribute nothing.
/// Does not validate direction/offset; call validate first in a full pipeline.
double sum_realized_pnl(const std::vector<TradeLogRow>& rows);

}  // namespace quant_review
