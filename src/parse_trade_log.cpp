#include "quant_review/parse_trade_log.hpp"

#include <cctype>
#include <cstdlib>
#include <vector>

namespace quant_review {
namespace {

constexpr std::size_t kFieldCount = 11;

std::vector<std::string_view> split_csv(std::string_view line) {
  std::vector<std::string_view> fields;
  fields.reserve(kFieldCount);
  std::size_t start = 0;
  for (std::size_t i = 0; i <= line.size(); ++i) {
    if (i == line.size() || line[i] == ',') {
      fields.push_back(line.substr(start, i - start));
      start = i + 1;
    }
  }
  return fields;
}

std::string_view trim(std::string_view s) {
  while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front()))) {
    s.remove_prefix(1);
  }
  while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) {
    s.remove_suffix(1);
  }
  return s;
}

bool parse_required_double(std::string_view cell, double& out, ParseError& err,
                           const char* name) {
  cell = trim(cell);
  if (cell.empty()) {
    err.message = std::string(name) + " is empty";
    return false;
  }
  const std::string text(cell);
  char* end = nullptr;
  out = std::strtod(text.c_str(), &end);
  if (end == text.c_str() || *end != '\0') {
    err.message = std::string("invalid ") + name + ": " + text;
    return false;
  }
  return true;
}

bool parse_optional_double(std::string_view cell, std::optional<double>& out,
                           ParseError& err, const char* name) {
  cell = trim(cell);
  if (cell.empty()) {
    out = std::nullopt;
    return true;
  }
  const std::string text(cell);
  char* end = nullptr;
  const double value = std::strtod(text.c_str(), &end);
  if (end == text.c_str() || *end != '\0') {
    err.message = std::string("invalid ") + name + ": " + text;
    return false;
  }
  out = value;
  return true;
}

}  // namespace

bool parse_trade_log_row(std::string_view line, TradeLogRow& out, ParseError& err) {
  err.message.clear();
  const auto fields = split_csv(line);
  if (fields.size() != kFieldCount) {
    err.message = "expected " + std::to_string(kFieldCount) + " fields, got " +
                  std::to_string(fields.size());
    return false;
  }

  TradeLogRow row;
  row.trade_id = std::string(trim(fields[0]));
  row.datetime = std::string(trim(fields[1]));
  row.symbol = std::string(trim(fields[2]));
  row.direction = std::string(trim(fields[3]));
  row.offset = std::string(trim(fields[4]));

  if (row.trade_id.empty()) {
    err.message = "trade_id is empty";
    return false;
  }
  if (!parse_required_double(fields[5], row.price, err, "price")) {
    return false;
  }
  if (!parse_required_double(fields[6], row.volume, err, "volume")) {
    return false;
  }
  if (!parse_optional_double(fields[7], row.commission_open, err, "commission_open")) {
    return false;
  }
  if (!parse_optional_double(fields[8], row.commission_close, err,
                             "commission_close")) {
    return false;
  }
  if (!parse_optional_double(fields[9], row.realized_pnl, err, "realized_pnl")) {
    return false;
  }
  if (!parse_optional_double(fields[10], row.unrealized_pnl, err,
                             "unrealized_pnl")) {
    return false;
  }

  out = std::move(row);
  return true;
}

}  // namespace quant_review
