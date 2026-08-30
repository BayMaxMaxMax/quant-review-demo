#include "quant_review/load_trade_log.hpp"

#include <fstream>
#include <string>

namespace quant_review {
namespace {

void strip_cr(std::string& line) {
  if (!line.empty() && line.back() == '\r') {
    line.pop_back();
  }
}

bool is_blank(const std::string& line) {
  for (char c : line) {
    if (c != ' ' && c != '\t') {
      return false;
    }
  }
  return true;
}

}  // namespace

bool load_trade_log_csv(const std::string& path, std::vector<TradeLogRow>& out,
                        LoadError& err) {
  err.message.clear();
  std::ifstream in(path);
  if (!in) {
    err.message = "cannot open file: " + path;
    return false;
  }

  std::vector<TradeLogRow> rows;
  std::string line;
  bool saw_header = false;
  std::size_t line_no = 0;

  while (std::getline(in, line)) {
    ++line_no;
    strip_cr(line);
    if (is_blank(line)) {
      continue;
    }
    if (!saw_header) {
      saw_header = true;  // first non-empty line = header; do not parse
      continue;
    }

    TradeLogRow row;
    ParseError parse_err;
    if (!parse_trade_log_row(line, row, parse_err)) {
      err.message = "line " + std::to_string(line_no) + ": " + parse_err.message;
      return false;
    }
    rows.push_back(std::move(row));
  }

  if (!saw_header) {
    err.message = "empty file (no header): " + path;
    return false;
  }

  out = std::move(rows);
  return true;
}

}  // namespace quant_review
