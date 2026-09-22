#include "quant_review/render_review_skeleton.hpp"

#include <sstream>
#include <string>
#include <vector>

namespace quant_review {

namespace {

struct ParsedFixture {
  std::string realized_total;
  std::string unrealized_hint_total;
  std::string win_rate;
  std::vector<std::string> anomaly_alerts;
};

std::string trim_copy(std::string value) {
  while (!value.empty() &&
         (value.front() == ' ' || value.front() == '\t' || value.front() == '\r')) {
    value.erase(value.begin());
  }
  while (!value.empty() &&
         (value.back() == ' ' || value.back() == '\t' || value.back() == '\r')) {
    value.pop_back();
  }
  return value;
}

std::string or_missing(const std::string& value) {
  return value.empty() ? "（无）" : value;
}

ParsedFixture parse_fixture(const std::string& fixture_reply) {
  // Offline teaching fixture only. Key:value lines; never fetch HTTP.
  ParsedFixture parsed;
  std::istringstream in(fixture_reply);
  std::string line;
  bool in_alerts = false;

  while (std::getline(in, line)) {
    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }
    const std::string trimmed = trim_copy(line);
    if (trimmed.empty()) {
      continue;
    }

    if (trimmed.rfind("anomaly_alerts:", 0) == 0) {
      in_alerts = true;
      continue;
    }

    if (in_alerts) {
      if (trimmed.rfind("- ", 0) == 0) {
        parsed.anomaly_alerts.push_back(trimmed.substr(2));
        continue;
      }
      in_alerts = false;
    }

    const auto colon = trimmed.find(':');
    if (colon == std::string::npos) {
      continue;
    }
    const std::string key = trim_copy(trimmed.substr(0, colon));
    const std::string value = trim_copy(trimmed.substr(colon + 1));
    if (key == "realized_total") {
      parsed.realized_total = value;
    } else if (key == "unrealized_hint_total") {
      parsed.unrealized_hint_total = value;
    } else if (key == "win_rate") {
      parsed.win_rate = value;
    }
  }

  return parsed;
}

}  // namespace

std::string render_review_skeleton(const std::string& fixture_reply) {
  const ParsedFixture parsed = parse_fixture(fixture_reply);

  std::ostringstream skeleton;
  skeleton << "# 复盘报告骨架\n"
           << "\n"
           << "## 已平总和\n"
           << or_missing(parsed.realized_total) << "\n"
           << "\n"
           << "## 未平浮动\n"
           << or_missing(parsed.unrealized_hint_total) << "\n"
           << "\n"
           << "## 胜率\n"
           << or_missing(parsed.win_rate) << "\n"
           << "\n"
           << "## 异常提示\n";

  if (parsed.anomaly_alerts.empty()) {
    skeleton << "（无）\n";
  } else {
    for (const std::string& tip : parsed.anomaly_alerts) {
      if (tip == "(none)") {
        skeleton << "（无）\n";
      } else {
        skeleton << "- " << tip << "\n";
      }
    }
  }

  return skeleton.str();
}

}  // namespace quant_review
