#include "quant_review/check_review_fixture_fields.hpp"

#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace quant_review {

namespace {

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

// Offline teaching fixture only. Key:value lines; never fetch HTTP.
std::unordered_map<std::string, std::string> parse_fixture_keys(
    const std::string& fixture_reply) {
  std::unordered_map<std::string, std::string> values;
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
      // Optional section header counts as present even with no bullets yet.
      values["anomaly_alerts"] = "(present)";
      continue;
    }

    if (in_alerts) {
      if (trimmed.rfind("- ", 0) == 0) {
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
    if (!key.empty()) {
      values[key] = value;
    }
  }

  return values;
}

}  // namespace

FixtureFieldCheck check_review_fixture_fields(const std::string& fixture_reply) {
  static const std::vector<std::string> kRequired = {
      "realized_total",
      "unrealized_hint_total",
      "win_rate",
  };

  const auto values = parse_fixture_keys(fixture_reply);
  FixtureFieldCheck result;
  result.ok = true;

  for (const auto& key : kRequired) {
    const auto it = values.find(key);
    if (it == values.end() || it->second.empty()) {
      result.ok = false;
      result.missing_keys.push_back(key);
    }
  }

  return result;
}

}  // namespace quant_review
