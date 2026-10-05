#pragma once

#include <string>
#include <vector>

namespace quant_review {

/// Result of Day29 offline fixture field check.
/// `ok == true` only when every required key is present with a non-empty value.
struct FixtureFieldCheck {
  bool ok = false;
  std::vector<std::string> missing_keys;
};

/// Day29: check that a **fixture** fake model reply has required keys.
/// Required (teaching): `realized_total`, `unrealized_hint_total`, `win_rate`.
/// `anomaly_alerts` is optional. Deterministic only — **no network, no LLM, no API key**.
/// Does **not** call a model and does **not** re-render the report skeleton.
FixtureFieldCheck check_review_fixture_fields(const std::string& fixture_reply);

}  // namespace quant_review
