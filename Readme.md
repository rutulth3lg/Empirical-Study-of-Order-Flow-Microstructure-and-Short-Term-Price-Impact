# Order Flow Microstructure & Short-Term Price Impact

built a limit order book engine from scratch in C++ to answer one question: **does order imbalance predict where price goes next?**

---

## what it does

- reconstructs full LOB state at every timestamp from tick data
- supports ADD and CANCEL events replayed from CSV
- price-time priority matching engine (continuous double auction)
- computes normalized order imbalance at every tick:

```
OI = (V_bid - V_ask) / (V_bid + V_ask)
```

- tracks mid price alongside OI to look for predictive correlation

## the hypothesis

more bid volume than ask volume → price goes up over the next few seconds. and vice versa. this is consistent with what the market microstructure literature says about informed order flow.

## sample output

```
T=1003 MidPrice=99.75 Imbalance=0.0526
MATCH: 80 units at 100.5
T=1004 MidPrice=99.75 Imbalance=0.2245
T=1007 MidPrice=99.75 Imbalance=0.3043   ← bid pressure building
MATCH: 10 units at 100.5
MATCH: 90 units at 100.8
MATCH: 100 units at 101.0
T=1009 MidPrice=100 Imbalance=0.7143     ← aggressive buying follows
```

## internals

| function | what it does |
|---|---|
| `addOrder` | passive order into the book |
| `cancelOrder` | removes by id, cleans empty levels |
| `matchOrder` | matches aggressive orders against resting (price-time priority) |
| `totalVolume` | sums quantity on a side |
| `orderImbalance` | normalized OI metric |
| `MidPrice` | (best bid + best ask) / 2 |
| `parseAndReplay` | reads CSV tick data, replays into LOB |

data structures: `map<double, map<string, Order>>` for both sides. bids sorted descending, asks ascending.

## run it

```bash
g++ main.cpp -o main
./main
```

## what's next

- plug in real NSE tick data (bhavcopy / zerodha API)
- rolling 10-second forward return calculation
- pearson correlation between OI and forward return
- OI vs mid-price time series visualization

## tech

C++17 · STL maps · no external dependencies
