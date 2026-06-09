#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <queue>
using namespace std;

enum class Side { BUY, SELL };

struct Order{
string Order_id;
Side side;
double price;
long long quantity;
long long Timestamp;
};

class LOB{  
private:
map<double , map<string,Order>, greater<double>>Bids;
map<double , map<string,Order>>Asks;

public:
void addOrder(Order o){ 
if(o.side == Side::BUY){ 
Bids[o.price][o.Order_id]=o;
}
else if(o.side == Side::SELL){
Asks[o.price][o.Order_id]=o;
 }
 }

void cancelOrder(string Order_id, Side side, double price){
if(side==Side::BUY){
if(Bids.count(price)){
Bids[price].erase(Order_id);
if(Bids[price].empty()) Bids.erase(price);
}
}
else{
if(Asks.count(price)){
Asks[price].erase(Order_id);
if(Asks[price].empty()) Asks.erase(price);
}
}
}

void matchOrder(Order o){
 if(o.side == Side::BUY){ 
while(!Asks.empty() && Asks.begin()->first <= o.price && o.quantity >0){

auto& bestAskLevel = Asks.begin()->second;
auto& resting = bestAskLevel.begin()->second;

long long traded = min(o.quantity, resting.quantity); 

o.quantity -= traded;
resting.quantity -= traded;
cout << "MATCH: " << traded << " units at " << resting.price << "\n";

if(resting.quantity == 0){
bestAskLevel.erase(bestAskLevel.begin());  
if(bestAskLevel.empty()) Asks.erase(Asks.begin());  
}
}
if(o.quantity > 0) Bids[o.price][o.Order_id] = o;
}
else{
while(!Bids.empty() && Bids.begin()->first >= o.price && o.quantity > 0){
            
auto& bestBidLevel = Bids.begin()->second;
auto& resting = bestBidLevel.begin()->second;
            
long long traded = min(o.quantity, resting.quantity);
            
o.quantity -= traded;
resting.quantity -= traded;
            
cout << "MATCH: " << traded << " units at " << resting.price << "\n";
            
if(resting.quantity == 0){
bestBidLevel.erase(bestBidLevel.begin());
if(bestBidLevel.empty()) Bids.erase(Bids.begin());
}
}
if(o.quantity > 0) Asks[o.price][o.Order_id] = o;
}
}

long long totalVolume(Side side){
long long total = 0;
if(side == Side::BUY){
    for(auto& priceLevel : Bids)
        for(auto& entry : priceLevel.second)
            total += entry.second.quantity;
} else {
    for(auto& priceLevel : Asks)
        for(auto& entry : priceLevel.second)
            total += entry.second.quantity;
}
return total;
}
double orderImbalance(){
long long vBid = totalVolume(Side::BUY);
long long vAsk = totalVolume(Side::SELL);
if(vBid + vAsk == 0) return 0; 
return (double)(vBid - vAsk) / (vBid + vAsk);
}



double MidPrice(){ 
if (Bids.empty() || Asks.empty()){
return 0;
}
else
return (Bids.begin()->first + Asks.begin()->first)/2.0;
}
};
void parseAndReplay(string filename, LOB& lob){
ifstream file(filename);
string line;
getline(file, line); // skip header
    
while(getline(file, line)){
stringstream ss(line);
string timestamp, order_id, side, price, quantity, type;
        
getline(ss, timestamp, ',');
getline(ss, order_id, ',');
getline(ss, side, ',');
getline(ss, price, ',');
getline(ss, quantity, ',');
getline(ss, type, ',');
        
Order o;
o.Order_id = order_id;
o.side = (side == "BUY") ? Side::BUY : Side::SELL;
o.price = stod(price);
o.quantity = stoll(quantity);
o.Timestamp = stoll(timestamp);
        
if(type == "ADD"){
lob.matchOrder(o);
} else if(type == "CANCEL"){
lob.cancelOrder(o.Order_id, o.side, o.price);
}
        
cout << "T=" << timestamp 
<< " MidPrice=" << lob.MidPrice() 
<< " Imbalance=" << lob.orderImbalance() << "\n";
}
}

int main(){
LOB lob;
parseAndReplay("data.csv", lob);
return 0;
}


