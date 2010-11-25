#ifndef EXECUTOR_APPLICATION_H
#define EXECUTOR_APPLICATION_H

#include "quickfix/Application.h"
#include "quickfix/MessageCracker.h"
#include "quickfix/Values.h"
#include "quickfix/Utility.h"
#include "quickfix/Mutex.h"
#include "quickfix/Session.h"

#include "quickfix/FieldConvertors.h"

#include "quickfix/fix40/Heartbeat.h"
#include "quickfix/fix41/Heartbeat.h"
#include "quickfix/fix42/Heartbeat.h"
#include "quickfix/fix43/Heartbeat.h"
#include "quickfix/fix44/Heartbeat.h"

#include "quickfix/fix40/ExecutionReport.h"
#include "quickfix/fix41/ExecutionReport.h"
#include "quickfix/fix42/ExecutionReport.h"
#include "quickfix/fix43/ExecutionReport.h"
#include "quickfix/fix44/ExecutionReport.h"

#include "quickfix/fix40/NewOrderSingle.h"
#include "quickfix/fix41/NewOrderSingle.h"
#include "quickfix/fix42/NewOrderSingle.h"
#include "quickfix/fix43/NewOrderSingle.h"
#include "quickfix/fix44/NewOrderSingle.h"


#include "mex.h"
#include "limit_order_manager.hpp"


class FixMessageHandler
: public FIX::Application, public FIX::MessageCracker
{
public:
    bool            m_created;
    bool            m_loggedon;
    std::string     m_session_string;
    std::string     m_heartbeat_time;
    limit_order_manager  lom;
    
    FixMessageHandler()  : m_created(false), m_loggedon(false), m_session_string("")
    {
    }


    void reset()
    {
        m_created = false;
        m_loggedon = false;
        m_session_string = "";
        m_heartbeat_time = "";
    }
    

    //----------------------------------------------------------------------------------------------
    /// 
    //----------------------------------------------------------------------------------------------
    bool send_limit_order( const limit_order& lo )
    {
        lom.Handle_send_limit_order( lo );
        
         if (lo.protocol == "FIX.4.3") {
            FIX43::NewOrderSingle o( lo.clOrdID, FIX::HandlInst('1'), lo.side, FIX::TransactTime(), FIX::OrdType(FIX::OrdType_LIMIT) );
            o.set(FIX::Symbol(lo.symbol) );
            o.set(FIX::OrderQty(lo.orderQty));
            o.set(FIX::TimeInForce(lo.timeInForce));
            o.set(FIX::Price(lo.price));
            o.getHeader().setField( FIX::SenderCompID(lo.sender_id) );
            o.getHeader().setField( FIX::TargetCompID(lo.target_id) );
            FIX::Session::sendToTarget( o );
            return true;
            //todo return false on error
         }
         
         if (lo.protocol == "FIX.4.4") {
          FIX44::NewOrderSingle o( lo.clOrdID, lo.side, FIX::TransactTime(), FIX::OrdType(FIX::OrdType_LIMIT) );
            o.set(FIX::HandlInst('1'));
            o.set(FIX::Symbol(lo.symbol) );
            o.set(FIX::OrderQty(lo.orderQty));
            o.set(FIX::TimeInForce(lo.timeInForce));
            o.set(FIX::Price(lo.price));
            o.getHeader().setField(FIX::SenderCompID(lo.sender_id));
            o.getHeader().setField(FIX::TargetCompID(lo.target_id));
            FIX::Session::sendToTarget(o);
            return true;
            // todo: return false on error
         }
         return false;
    }

    //----------------------------------------------------------------------------------------------
    /// 
    //----------------------------------------------------------------------------------------------
    void onCreate( const FIX::SessionID& sessionID)
    {
        m_created = true;
        m_session_string = sessionID.toString();
    }
  
    //----------------------------------------------------------------------------------------------
    /// 
    //----------------------------------------------------------------------------------------------
    void onLogon( const FIX::SessionID& sessionID)
    {
        m_loggedon = true;
        m_session_string = sessionID.toString();
        mexPrintf("Application::onLogon \n");
    }

    //----------------------------------------------------------------------------------------------
    /// 
    //----------------------------------------------------------------------------------------------
    void onLogout( const FIX::SessionID& sessionID)
    {
        m_loggedon = false;
        m_session_string = sessionID.toString();
        mexPrintf("Application::onLogout \n");
    }

    //----------------------------------------------------------------------------------------------
    /// 
    //----------------------------------------------------------------------------------------------
    void toAdmin( FIX::Message&, const FIX::SessionID& sessionID)
    {
    }

    //----------------------------------------------------------------------------------------------
    /// 
    //----------------------------------------------------------------------------------------------
    void toApp( FIX::Message&, const FIX::SessionID& sessionID)
        throw( FIX::DoNotSend )
    {
    }
  

    //----------------------------------------------------------------------------------------------
    /// 
    //----------------------------------------------------------------------------------------------
    void fromAdmin( const FIX::Message& message, const FIX::SessionID& sessionID)
        throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon )
    {
        crack(message, sessionID);
    }
  
    //----------------------------------------------------------------------------------------------
    /// 
    //----------------------------------------------------------------------------------------------
    void fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
        throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType )
    {
        crack(message, sessionID);
    }

    //----------------------------------------------------------------------------------------------
    /// HEARTBEAT MESSAGES
    //----------------------------------------------------------------------------------------------
    void onMessage( const FIX40::Heartbeat& m, const FIX::SessionID& s) { onMessage_Heartbeat(m,s); }
    void onMessage( const FIX41::Heartbeat& m, const FIX::SessionID& s) { onMessage_Heartbeat(m,s); }
    void onMessage( const FIX42::Heartbeat& m, const FIX::SessionID& s) { onMessage_Heartbeat(m,s); }
    void onMessage( const FIX43::Heartbeat& m, const FIX::SessionID& s) { onMessage_Heartbeat(m,s); }
    void onMessage( const FIX44::Heartbeat& m, const FIX::SessionID& s) { onMessage_Heartbeat(m,s); }
    
    template <typename T>
    void onMessage_Heartbeat( const T& message, const FIX::SessionID& session)
    {
        mexPrintf("Application::onMessage_Heartbeat( ... \n");
        FIX::SendingTime SendingTime;
        message.getHeader().get(SendingTime);
        m_heartbeat_time = FIX::UtcTimeStampConvertor::convert(SendingTime, true);
    }
  
    //----------------------------------------------------------------------------------------------
    /// EXECUTION REPORT MESSAGES
    //----------------------------------------------------------------------------------------------
    void onMessage( const FIX43::ExecutionReport& m, const FIX::SessionID& s) { lom.Handle_ExecutionReport(m, s); }
    void onMessage( const FIX44::ExecutionReport& m, const FIX::SessionID& s) { lom.Handle_ExecutionReport(m, s); }


};

#endif
