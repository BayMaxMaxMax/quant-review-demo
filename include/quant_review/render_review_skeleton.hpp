#pragma once

#include <string>

namespace quant_review {

/// Day28: turn a **fixture** fake model reply into a fixed report skeleton.
/// Deterministic template only — **no network, no LLM, no API key**.
/// Missing keys still emit headings (empty fixture → 小标题 + 「（无）」).
/// Day28 teaching mock fixture must surface +2 / +10 / 1/2 in the skeleton.
std::string render_review_skeleton(const std::string& fixture_reply);

}  // namespace quant_review
