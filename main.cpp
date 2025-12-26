#include <ixwebsocket/IXWebSocket.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <nlohmann/json.hpp>

using namespace std;
using json=nlohmann::json;

int main(){
    ix::WebSocket ws;
    bool has_last=false;
    long long last_id=-1;

    ws.setUrl("wss://stream.binance.com:9443/ws/btcusdt@trade");

    ws.setOnMessageCallback(
        [&](const ix::WebSocketMessagePtr& msg)
        {
            if(msg->type==ix::WebSocketMessageType::Open)
            {
                cout<<"Websocket Connected Successfully"<<endl;
            }

            else if(msg->type==ix::WebSocketMessageType::Message)
            {
                try
                {
                    json j=json::parse(msg->str);

                    if(j.contains("result") && j.contains("id"))
                    {
                        return;
                    }

                    else if(j.contains("e") && j["e"]=="trade")
                    {
                        long long trade_id=j["t"].get<long long>();
                        long long override=last_id+1;
                        long long normal_tick=last_id+1;

                        if(!has_last)
                        {
                            has_last=true;
                            last_id=trade_id;
                        }

                        else if(last_id==trade_id)
                        {
                            cout<<"Duplicate Trade Id"<<endl;
                        }

                        else if(trade_id>override)
                        {
                            double gap=trade_id-last_id;
                            cout<<"Trade Ids missing : "<<gap<<endl;
                            last_id=trade_id;
                        }

                        else if(trade_id==normal_tick)
                        {
                            cout<<"Normal TICK : "<<trade_id<<endl;
                            last_id=trade_id;
                        }
                    }
                }
                catch(exception& e)
                {
                    cout<<"Erro : "<<e.what()<<endl;
                }
                
            }

        }
    );

    ws.start();

    while(true)
    {
        usleep(500);
    }

    ws.stop();

    return 0;
}