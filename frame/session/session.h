#ifndef _APE_COMMON_SESSION_H_
#define _APE_COMMON_SESSION_H_

#include "connection.h"
#include "sessioncallback.h"
#include "protocol.h"
#include "timermanager.h"

namespace ape{
namespace net{
typedef boost::shared_ptr<CConnection> Connection_ptr;
class CSession {
 public:
    typedef enum { WAITING = 0, CONNECTED, TIME_OUT, CONNECTING, CLOSED} EStatus;
    CSession();
    virtual void Init(boost::asio::io_service &io, ape::protocol::EProtocolType pro, CSessionCallBack *o,
        ape::common::CTimerManager *tm = NULL, bool autoreconnect = false, int heartbeat = 0);
    virtual void Connect(const std::string &ip, unsigned int port);
    virtual void DoConnect();
    virtual void ConnectResult(int result);
    virtual void OnAccept();
    virtual void OnConnected();
    virtual void OnPeerClose();
    virtual void OnRead(ape::message::SNetMessage *msg);
    virtual void DoSendTo(void *para, int timeout);
    virtual void DoSendBack(void *para, bool close = false);
    virtual void Close();
    virtual void Dump();
    virtual ~CSession();

    void SetOwner(CSessionCallBack *o) {owner_ = o;}
    CSessionCallBack *GetOwner(){return owner_;}
    void SetTimerManager(ape::common::CTimerManager *o) {timer_owner_ = o;}
    ape::common::CTimerManager *GetTimerManager(){return timer_owner_;}
    unsigned int Id() {return ptrconn_->Id();}
    Connection_ptr GetConnectPrt() const {return ptrconn_;}
    void SetAddr(const std::string &addr){addr_ = addr;}
    void SetName(const std::string &name){session_name_ = name;}
    const std::string &GetName() const {return session_name_;}
    const std::string &GetAddr() const {return addr_;}
    const std::string &GetRemoteIp(){return ptrconn_->GetRemoteIp();}
    unsigned int GetRemotePort(){return ptrconn_->GetRemotePort();}
    EStatus GetStatus(){return status_;}
    void SetStatus(EStatus s){status_ = s;}
    void CleanRequestAndCallBack();
 private:
    void DealWaitingList();
    void DoRequestTimeOut(void *para);
    void DoHeartBeat();
    void DoSendRequest(ape::message::SNetMessage *msg, int timeout);
private:
    EStatus status_;
    ape::protocol::EProtocolType proto_;
    Connection_ptr ptrconn_;
    CSessionCallBack *owner_;
    ape::common::CTimerManager *timer_owner_;
    std::string session_name_;
    std::string addr_;
    std::string ip_;
    unsigned int port_;
    bool autoreconnect_;
    int heartbeatinterval_;
    ape::common::CThreadTimer *timer_reconn_;
    ape::common::CThreadTimer *timer_heartbeat_;

    typedef std::multimap<unsigned int, boost::shared_ptr<ape::common::CThreadTimer> > RequestHistory;
    RequestHistory request_history_;
    typedef struct stReadyPacket {
        void *packet;
        int timeout;
        stReadyPacket() : packet(NULL), timeout(0) {}
        stReadyPacket(void *p, int t) : packet(p), timeout(t) {}
    }SReadyPacket;
    std::deque<SReadyPacket> waitinglist_;

};
//typedef boost::shared_ptr<CSession> Session_Ptr;

class SessionFactory {
 public:
    SessionFactory() {}
    virtual ~SessionFactory() {}
    virtual CSession *CreateSession() = 0;

    static void RegisterFactory(ape::protocol::EProtocolType protocol, SessionFactory *factory);
    static CSession *CreateSession(ape::protocol::EProtocolType protocol);
 private:
    static SessionFactory *factories_[ape::protocol::E_PROTOCOL_ALL];
};

}
}
#endif

