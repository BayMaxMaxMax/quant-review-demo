#include "quant_review/validate_trade_log.hpp"

namespace quant_review {
namespace {

bool is_direction(const std::string& value) {
  return value == "long" || value == "short";
}

bool is_offset(const std::string& value) {
  return value == "open" || value == "close";
}

}  // namespace

bool validate_direction_and_offset(const TradeLogRow& row, ValidationError& err) {
  err.message.clear();

  if (!is_direction(row.direction)) {
    err.message = "invalid direction: expected long|short, got " + row.direction;
    return false;
  }
  if (!is_offset(row.offset)) {
    err.message = "invalid offset: expected open|close, got " + row.offset;
    return false;
  }

  const bool has_realized = row.realized_pnl.has_value();
  const bool has_unrealized = row.unrealized_pnl.has_value();

  if (has_realized && has_unrealized) {
    err.message = "realized_pnl and unrealized_pnl both set";
    return false;
  }
  if (!has_realized && !has_unrealized) {
    err.message = "neither realized_pnl nor unrealized_pnl set";
    return false;
  }

  // 已平 bucket ↔ 平仓；未平 bucket ↔ 开仓（教学复盘投影）
  if (has_realized && row.offset != "close") {
    err.message =
        "realized row requires offset=close (direction/open-close mismatch)";
    return false;
  }
  if (has_unrealized && row.offset != "open") {
    err.message =
        "unrealized row requires offset=open (direction/open-close mismatch)";
    return false;
  }

  return true;
}

bool gate_rows_for_report(const std::vector<TradeLogRow>& rows,
                          ValidationError& err) {
  err.message.clear();
  for (std::size_t i = 0; i < rows.size(); ++i) {
    ValidationError row_err;
    if (!validate_direction_and_offset(rows[i], row_err)) {
      err.message = "row " + std::to_string(i) + ": " + row_err.message;
      return false;
    }
  }
  return true;
}

}  // namespace quant_review
