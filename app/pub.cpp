
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
using namespace std;

struct msg
{
    std::uint64_t instrumentId_;
    double lastTradedPrice_;
    double bondYield_;                // only for BondPublisher
    std::uint64_t lastDayVolume_;     // only for EquityPublisher
};


class sub
{
    public:
    int id;
    msg last_msg;
    sub(){
    }
    void recv(msg _msg){
        last_msg = _msg;
    }
};

class free_sub : public sub
{
    public:
    uint16_t remaining_msg;
    free_sub(){
        remaining_msg = 100;
    }
    void recv(msg _msg){
        if(remaining_msg > 0){
            last_msg = _msg;
            remaining_msg--;
        }
        else{
            msg null_msg = msg{0,0,0,0};
            last_msg = null_msg;
        }
    }
    
};

class paid_sub : public sub
{

};


unordered_map<int,sub> sub_map; 

class pub
{
    public:
    int id;
    set<int> subs;
    pub(int _id){
        id = _id;
    }
    void broadcast(msg _msg){
        for(auto s : subs){
            sub_map[s].recv(_msg);
            // cout << "Broadcast to " << s << endl;
            // //cout << sub_map[s].last_msg.lastTradedPrice_ << endl;
            // cout << _msg.lastDayVolume_ << endl;
            // cout << sub_map[s].last_msg.lastDayVolume_ << endl;
        }
    }
    void add_sub(int s){
        subs.insert(s);
    }
    private:

};

class equity_pub : public pub
{
    public:
    equity_pub(int _id) : pub(_id){
    }   
};  

class bond_pub : public pub
{
    public:
    bond_pub(int _id) : pub(_id){
    }
};


vector<pub> init_pubs(){
    vector<pub> pubs;
    for(int i = 0;i < 2000;i++){
        if(i >= 1000){
            pubs.push_back(bond_pub(i));
        }else{
            pubs.push_back(equity_pub(i));
        }
    }
    return pubs;
}
vector<pub> pubs = init_pubs();


int main()
{
    int N;
    cin >> N;
    for(int i = 0;i < N;i++){
        char c;
        cin >> c;
        if(c == 'P'){
            int id;
            double lastTradedPrice=0;
            double bondYield=0;
            double lastDayVolume;
            cin >> id >> lastTradedPrice;
            if(id >= 1000){
                cin >> bondYield;
            }else{
                cin >> lastDayVolume;
            }
            msg _msg = {id,lastTradedPrice,bondYield,lastDayVolume};
            pubs[id].broadcast(_msg);
        }
        else if(c == 'S'){
            char type;
            int s_id,p_id;
            string s;
            cin >> type >> s_id >> s >> p_id;
            if(s == "subscribe"){
                if(type == 'P'){
                    sub_map[s_id] = free_sub();
                }else{
                    sub_map[s_id] = paid_sub();
                }
                pubs[p_id].add_sub(s_id);
            }
            else if(s == "get_data"){
                if(sub_map[s_id].last_msg.instrumentId_ == p_id){
                    cout << type << " " << s_id << " " << p_id << " " << sub_map[s_id].last_msg.lastTradedPrice_ << " ";
                    if(p_id >= 1000){
                        cout << sub_map[s_id].last_msg.bondYield_ << endl;
                    }else{
                        cout << sub_map[s_id].last_msg.lastDayVolume_ << endl;
                    }
                }else{
                    cout << type << " " << s_id << " " << p_id << " invalid_request" << endl;
                }
            }
        }
    }
    return 0;
}