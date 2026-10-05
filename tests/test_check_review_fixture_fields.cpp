#include "quant_review/check_review_fixture_fields.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <string>

using quant_review::check_review_fixture_fields;
using quant_review::FixtureFieldCheck;

namespace {

const char* kDay29CompleteFixture =
    "realized_total: +2\n"
    "unrealized_hint_total: +10\n"
    "win_rate: 1/2\n"
    "anomaly_alerts:\n"
    "- [consecutive_closed_losses] consecutive closed losses >= 2\n";

bool has_missing(const FixtureFieldCheck& check, const std::string& key) {
  return std::find(check.missing_keys.begin(), check.missing_keys.end(), key) !=
         check.missing_keys.end();
}

}  // namespace

TEST(CheckReviewFixtureFields, CompleteFixtureIsOk) {
  const FixtureFieldCheck check = check_review_fixture_fields(kDay29CompleteFixture);
  EXPECT_TRUE(check.ok);
  EXPECT_TRUE(check.missing_keys.empty());
}

TEST(CheckReviewFixtureFields, MissingOneRequiredKeyReportsThatKey) {
  const std::string fixture =
      "realized_total: +2\n"
      "win_rate: 1/2\n";
  const FixtureFieldCheck check = check_review_fixture_fields(fixture);
  EXPECT_FALSE(check.ok);
  EXPECT_EQ(check.missing_keys.size(), 1u);
  EXPECT_TRUE(has_missing(check, "unrealized_hint_total"));
}

TEST(CheckReviewFixtureFields, EmptyValueCountsAsMissing) {
  const std::string fixture =
      "realized_total: +2\n"
      "unrealized_hint_total:\n"
      "win_rate: 1/2\n";
  const FixtureFieldCheck check = check_review_fixture_fields(fixture);
  EXPECT_FALSE(check.ok);
  EXPECT_TRUE(has_missing(check, "unrealized_hint_total"));
}

TEST(CheckReviewFixtureFields, EmptyFixtureReportsAllRequiredKeys) {
  const FixtureFieldCheck check = check_review_fixture_fields("");
  EXPECT_FALSE(check.ok);
  EXPECT_EQ(check.missing_keys.size(), 3u);
  EXPECT_TRUE(has_missing(check, "realized_total"));
  EXPECT_TRUE(has_missing(check, "unrealized_hint_total"));
  EXPECT_TRUE(has_missing(check, "win_rate"));
}

TEST(CheckReviewFixtureFields, AnomalyAlertsOptional) {
  const std::string fixture =
      "realized_total: +2\n"
      "unrealized_hint_total: +10\n"
      "win_rate: 1/2\n";
  const FixtureFieldCheck check = check_review_fixture_fields(fixture);
  EXPECT_TRUE(check.ok);
  EXPECT_TRUE(check.missing_keys.empty());
}

TEST(CheckReviewFixtureFields, GuardrailNoNetworkOrKeyHintsInApiSurface) {
  // Teaching guardrail: this step is offline field check only.
  // The API name / result must not imply calling a model.
  const FixtureFieldCheck check = check_review_fixture_fields(kDay29CompleteFixture);
  EXPECT_TRUE(check.ok);
  // Keep the teaching fixture free of network / key tokens (same as Day28).
  const std::string fixture(kDay29CompleteFixture);
  EXPECT_EQ(fixture.find("DeepSeek"), std::string::npos);
  EXPECT_EQ(fixture.find("http"), std::string::npos);
  EXPECT_EQ(fixture.find("api_key"), std::string::npos);
}
