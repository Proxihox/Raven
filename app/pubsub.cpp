// clang-format off
/*
We are to build a publisher and subscriber system
We will have 2 types of publishers:
1. EquityPublisher
2. BondPublisher
We will have 2 types of subscribers:
1. PaidSubscriber
2. FreeSubscriber
Publishers are meant to maintain and publish the following data
struct
{
    std::uint64_t instrumentId_;
    double lastTradedPrice_;
    double bondYield_;                // only for BondPublisher
    std::uint64_t lastDayVolume_;     // only for EquityPublisher
};
Subscriber (PaidSubscriber and FreeSubscriber) can subscribe to either the Equity or Bond publisher.
FreeSubscriber can make only 100 successful `get_data` requests after which requests will be denied.
PaidSubscriber can make any number of requests.
Publisher Interface:
Publisher::update_data(...)  // Takes InstrumentId and and data points (lastTradedPrice_ and bondYield_/lastDayVolume_) and stores it
Publisher::subscribe(...)    // interface for subscriber to subscribe to the publisher
Publisher::get_data(...)     // interface for subscriber to get data, if subscriber does not exist or data point does not exist it is an invalid request
Subscriber Interface:
Subscriber::subscribe(...)   // takes instrumentId and calls subscribe to the relevent publisher
Subscriber::get_data(...)    // interface for subscriber to get data for a particular instrumentId, works by calling the publisher's get_data(...) method
instrumentId of [0, 1000) are reserved for EquityPublisher and [1000, 2000) are reserved for BondPublisher
Input Format:
<number of lines of input>
each line will be of one of the following
    P <instrumentId> <lastTradedPrice> <bondYield or lastDayVolume>
    S <subscriber type (P | F)> <subscriberId> subscribe <instrumentId>
    S <subscriber type (P | F)> <subscriberId> get_data <instrumentId>
Output Format:
For every input line of format
S <subscriber type (P | F)> <subscriberId> get_data <instrumentId>
print comma separated list of 
<subscriber type (P | F)>, <subscriberId>, <instrument id>, <lastTradedPrice>, <bondYield/lastDayVolume> if valid request
else print <subscriber type (P | F)>, <subscriberId>, <instrument id>, invalid_request
Please design the system with the following assumptions:
1. Number of subscribers can be very large
2. In future we might introduce more types of publishers, subscribers, data points (make your systems extensible)
3. Focus on correctness, extensibility and maintainability of the code
*/
// clang-format on

#include <cstdint>
#include <iostream>

// You can use the following callables to generate random messages

auto generate_publisher_update_message = [] {
  std::string msg = "P ";
  std::uint64_t instrumentId = rand() % 2000;
  msg += std::to_string(instrumentId) + " ";
  msg += std::to_string(rand()) + " ";
  if (instrumentId >= 1000)
    // Bond update
    // bond yield is percentage (< 100)
    msg += std::to_string(static_cast<double>(rand() % 10000) / 100);
  else
    // Equity update
    // last day volume
    msg += std::to_string(rand());

  msg += "\n";
  return msg;
};

auto generate_subscribe_message = [] {
  std::string msg = "S ";
  if(rand() % 2)
      msg += "F ";
  else
      msg += "P ";
  std::uint64_t subscriberId = rand();
  msg += std::to_string(subscriberId) + " ";
  msg += "subscribe ";
  msg += std::to_string(rand() % 2000) + "\n";

  return msg;
};

auto generate_get_data_message = [] {
  std::string msg = "S ";
  if(rand() % 2)
      msg += "F ";
  else
      msg += "P ";
  std::uint64_t subscriberId = rand();
  msg += std::to_string(subscriberId) + " ";
  msg += "get_data ";
  msg += std::to_string(rand() % 2000) + "\n";

  return msg;
};
int main(){
    std::cout << generate_subscribe_message();
    std::cout << generate_subscribe_message();
    std::cout << generate_subscribe_message();
    std::cout << generate_publisher_update_message();
    std::cout << generate_get_data_message();
    return 0;
}