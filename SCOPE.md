# Review-only scope

## In scope

1. Mock CSV schema and parsing
2. Deterministic analytics for post-hoc review
3. Aggregated JSON suitable for an LLM review prompt (no raw tick dumps in prompts)
4. Markdown report rendering from model JSON

## Out of scope (hard)

1. Trading decisions (`buy` / `sell` / `order` / `signal`)
2. Strategy backtest or simulated “what if I traded”
3. Real-time monitoring of live logs or market feeds
4. Broker / exchange integration
5. Any optimization of return-like metrics

If a change request adds an out-of-scope item, reject it and document why in the learning notes.
