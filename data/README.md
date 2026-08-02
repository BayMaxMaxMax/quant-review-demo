# Mock data

All files under `data/` are **fictional**, for software-engineering learning only.

They are unrelated to any real account, instrument, or market.

## B1 schema (**locked** · review subset)

> **Locked for theme-map B1** (`docs/data: lock mock schema`).  
> Parser / tests (**B2+**) must follow this file; schema changes need a new `docs/data:` commit.  
> **Subset rule:** names/semantics may *align with* common trade-log models (e.g. direction / offset / fees).  
> This CSV is **only** what post-hoc review needs. It is **not** a full OMS object model (no order book, no gateway, no yd/td split, no signals).

### Row meaning

One row = one **fill / journal line** for teaching (open, close, or open-position mark).  
Partially filled multi-trade orders are **out of scope** for W2; keep one logical fill per row.

### Columns

| Column | Type (logical) | Required | Meaning (review-only) | In this repo? |
|---|---|---|---|---|
| `trade_id` | string | yes | Stable unique id for the row (derived key OK) | **subset · yes** |
| `timestamp` | ISO-8601 UTC+8 or naive local | yes | Event time for the row | **subset · yes** |
| `instrument` | string | yes | Mock symbol only (e.g. `MOCK_FUT`) | **subset · yes** |
| `direction` | enum | yes | `long` \| `short` | **subset · yes** |
| `offset` | enum | yes | `open` \| `close` | **subset · yes** |
| `status` | enum | yes | `closed` \| `open` — closed ⇒ realized path; open ⇒ floating only | **subset · yes** |
| `price` | number | yes | Fill price; for `status=open` mark row use mark price here | **subset · yes** |
| `volume` | number | yes | Size (mock units) | **subset · yes** |
| `commission_open` | number | if known | Fee already incurred on open | **subset · yes** |
| `commission_close` | number | if closed | Fee on close; **empty/0** while `status=open` | **subset · yes** |
| `realized_net` | number | if `status=closed` | Net after fees for this closed round-trip (teaching); **must be empty** if open | **subset · yes** |
| `floating_indication` | number | if `status=open` | Unsettled PnL hint only; **must be empty** if closed | **subset · yes** |

### Explicitly **not** in this subset (later / other systems)

- Order lifecycle / `vt_orderid` / partial fills chain  
- Yesterday vs today position (`yd` / `td`) and `CLOSETODAY`  
- Account balance / frozen / available  
- Tick / depth / live gateway fields  
- Strategy signals, targets, or backtest fills  

### Aggregation rules (must match public Day5–D8 cards)

1. **Sum `realized_net` only** over `status=closed` rows.  
2. **Report `floating_indication` separately** for `status=open` rows — never add into realized total as a “settled” number.  
3. Reject rows that set both `realized_net` and `floating_indication`.

### Header line

```text
trade_id,timestamp,instrument,direction,offset,status,price,volume,commission_open,commission_close,realized_net,floating_indication
```

### Example fixture

See [`examples/b1-two-rows.csv`](examples/b1-two-rows.csv) (also inlined below).

```text
trade_id,timestamp,instrument,direction,offset,status,price,volume,commission_open,commission_close,realized_net,floating_indication
t001,2026-07-27T10:00:00,MOCK_FUT,long,close,closed,100,1,2,2,6,
t002,2026-07-27T15:00:00,MOCK_FUT,long,open,open,110,1,2,,,10
```

- Row `t001`: closed teaching net **+6** (aligns Day4: price-diff story − fees).  
- Row `t002`: open floating **+10** (aligns Day5); do **not** treat +6 and +10 as one settled +16.  
- Empty cells mean “no value” (not zero) for the mutually exclusive PnL columns.

### Status

- **Locked** for public note **B1** (复盘日志字段怎么设计).  
- Next: **B2** — parse one CSV row + minimal tests against this schema.
