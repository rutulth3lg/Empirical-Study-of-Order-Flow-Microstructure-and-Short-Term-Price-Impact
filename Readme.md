# Empirical Study of Price Impact and Order Flow in a Continuous Double Auction Market

## Overview
A high-performance **Limit Order Book (LOB)** engine built from scratch in C++, designed to analyze market microstructure behavior using simulated NSE tick data.

The core research question: **does order imbalance predict short-term price movement?**

## What This Project Does
- Reconstructs a full Limit Order Book state at each timestamp
- Supports ADD and CANCEL order events replayed from tick data
- Implements a price-time priority matching engine (continuous double auction)
- Computes a normalized **Order Imbalance** metric at every tick:

```
OI = (V_bid - V_ask) / (V_bid + V_ask)
```

- Tracks Mid Price evolution alongside order imbalance to study predictive correlation

## Key Components

### Data Structures
- `Order` — struct holding order_id, side, price, quantity, timestamp
- `LOB` — Limit Order Book class with:
  - `map<double, map<string, Order>, greater<double>> Bids` — bid side, sorted descending by price
  - `map<double, map<string, Order>> Asks` — ask side, sorted ascending by price

### Core Functions
| Function | Description |
|---|---|
| `addOrder` | Places a passive order into the book |
| `cancelOrder` | Removes an order by id, cleans up empty price levels |
| `matchOrder` | Matches incoming aggressive order against resting orders (price-time priority) |
| `totalVolume` | Sums all quantity on a given side |
| `orderImbalance` | Computes normalized OI metric |
| `MidPrice` | Returns (best bid + best ask) / 2 |
| `parseAndReplay` | Reads CSV tick data and replays events into the LOB |

## Research Hypothesis
A positive order imbalance (more bid volume than ask volume) predicts upward mid-price movement over the next 10 seconds, and vice versa. This aligns with findings in market microstructure literature on the informativeness of limit order book state.

## Sample Output
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

## How to Run
```bash
g++ main.cpp -o main
./main
```

## Tech Stack
- C++17
- STL: `map`, `string`, `fstream`, `sstream`
- No external dependencies

## Next Steps
- Feed real NSE tick data (Bhavcopy / Zerodha API)
- Compute rolling 10-second forward return
- Calculate Pearson correlation between OI and forward return
- Visualize OI vs mid-price time series
