/*
compile with:
mex -I/usr/local/include -lboost_thread -lquickfix matfix.cpp

or
mex -I'C:\boost_1_42_0' -I'C:\quickfix\include' -L'C:\boost_1_42_0\stage\lib' -llibboost_thread-vc90-mt-1_42 -llibboost_date_time-vc90-mt-1_42 -L'C:\quickfix\lib' -lquickfix  -L'C:\Program Files\Microsoft SDKs\Windows\v6.0A\Lib' -lws2_32 matfix.cpp

mex -I'C:\boost_1_42_0' -I'C:\quickfix\include' -L'C:\boost_1_42_0\stage\lib' -L'C:\quickfix-1.13.3\quickfix\lib\debug' -L'C:\Progra~1\Mi2578~1\Windows\v6.0A\Lib' -llibboost_thread-vc90-mt-1_42 -llibboost_date_time-vc90-mt-1_42  -lquickfix  -lws2_32 -g matfix.cpp
mex -g -I'C:\boost_1_42_0' -I'C:\quickfix\include' -L'C:\boost_1_42_0\stage\lib' -L'C:\quickfix-1.13.3\quickfix\lib\debug' -L'C:\Progra~1\Mi2578~1\Windows\v6.0A\Lib' -llibboost_thread-vc90-mt-1_42 -llibboost_date_time-vc90-mt-1_42  -lquickfix  -lws2_32 matfix.cpp
done:
- added cyclic buffers
- added a asyncronous thread for fix messaging
- added mutex to prevent read/write tick issues
- added matlab time suport
- fixed at_exit: stop the threads
- added peekt, and peekp command
- added start, stop command
- added symbol command: return a list of symbols in a cell array
- added quickfix applciation!

todo: 
- check the mutex locking, does it need to be recursive?
- make to_pmxArray a free function in stad of member function?

*/

#define debug
//---------------------------------------------
// STL
//---------------------------------------------
#include <map>
#include <string>

//---------------------------------------------
// Boost
//---------------------------------------------
//#include <boost/thread/thread.hpp>
//#include <boost/thread/mutex.hpp>
//#include <boost/bind.hpp>
#include <boost/circular_buffer.hpp>

//---------------------------------------------
/// A mutex used for licking tick access
//---------------------------------------------
//boost::mutex tick_mutex;
//boost::mutex run_status_mutex;

//---------------------------------------------
// Matlab
//---------------------------------------------
#include "mex.h"

#include "time.hpp"
#include "type_conversion.hpp"
#include "usage.hpp"
//#include "mxstruct.hpp"

//---------------------------------------------
// QuickFIX stuff
//---------------------------------------------
#include "quickfix/FileStore.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/SessionSettings.h"
#include "quickfix/Exceptions.h"

#include "fix_message_handler.hpp"
#include "tick_manager.hpp"
#include "limit_order_manager.hpp"

class FixClient
{

private:    

    FixMessageHandler       m_fix_handler;
    std::string             m_config_filename;
    FIX::SessionSettings*   p_settings;
    FIX::FileStoreFactory*  p_storeFactory;
    FIX::SocketInitiator*   p_initiator;
    
public:
    symbol_tick_buffers     buffers;

    /// Constructor
    FixClient() : m_fix_handler(), m_config_filename("matfix.cfg"),  p_settings(NULL), p_storeFactory(NULL), p_initiator(NULL)
    {
        
    }
    
    void connect(int ins, const mxArray *in[])
    {
        try {
            if (p_initiator != NULL) disconnect();

			//m_config_filename = std::string("matfix.cfg");
            if ( (ins>=2) && !pmxArrayToString(in[1], m_config_filename) ) 
                mexErrMsgTxt("Unable to read the config_file name (second argument)."); 

            p_settings     = new FIX::SessionSettings( m_config_filename );
            p_storeFactory = new FIX::FileStoreFactory( *p_settings );
            p_initiator    = new FIX::SocketInitiator( m_fix_handler, *p_storeFactory, *p_settings );
            p_initiator->start();
        }
		catch (FIX::Exception& ce){
			int i=3;
		}
        catch ( std::exception& e ) {
            //mexErrMsgTxt( e.what() );
        }
        catch (...) {
           int a=4;
        }
        
    }
    
    void disconnect()
    {
        try {
            if (p_initiator   != NULL) p_initiator->stop();
            if (p_initiator   != NULL) delete p_initiator;
            if (p_storeFactory!= NULL) delete p_storeFactory;
            if (p_settings    != NULL) delete p_settings;
            p_initiator = NULL;
            p_storeFactory = NULL;
            p_settings = NULL;
            m_fix_handler.reset();
        }
        catch ( std::exception & e ) {
            mexErrMsgTxt( e.what() );
        }
    }
    
    
    void status(mxArray *&p) 
    {
        //--------------------------------------------------------
        // Create a Matlab Structure
        //--------------------------------------------------------
        const char *field_names[] = {
            "config_filename", 
            "created", 
            "loggedon", 
            "sessionId",
            "heartbeat"
        };
        mwSize dims[2] = {1,1};
        p = mxCreateStructArray(2, dims, sizeof(field_names)/sizeof(*field_names), field_names);
        
        //--------------------------------------------------------
        // collect all the info
        //--------------------------------------------------------
        mxSetFieldByNumber(p, 0,  0, mxCreateString( m_config_filename.c_str() )  );
        mxSetFieldByNumber(p, 0,  1, mxCreateLogicalScalar( (m_fix_handler.m_created) ? 1 : 0)  );
        mxSetFieldByNumber(p, 0,  2, mxCreateLogicalScalar( (m_fix_handler.m_loggedon) ? 1 : 0)  );
        mxSetFieldByNumber(p, 0,  3, mxCreateString( m_fix_handler.m_session_string.c_str() ) );
        mxSetFieldByNumber(p, 0,  4, mxCreateString( m_fix_handler.m_heartbeat_time.c_str() ) );
    }
    
    void orders(mxArray *&p) 
    {
        m_fix_handler.lom.orders_to_pmxArray(p);
    }  
    
    void lasts( mxArray *&p) 
    {
        int n = 6;
        
        //--------------------------------------------------------
        // Create a Matlab Structure
        //--------------------------------------------------------
        const int fields = 3;
        const char *field_names[] = {"symbol", "timestamp", "last"};
        mwSize dims[2];
        dims[0] = 1;
        dims[1] = n;
        p = mxCreateStructArray(2, dims, fields, field_names);
        
        
        
        //--------------------------------------------------------
        // allocate temp array: we use this to copy doubles into the struct
        //--------------------------------------------------------
        mxArray *tmp_double_array = mxCreateDoubleMatrix(1, 1, mxREAL);
        double *pDouble = mxGetPr(tmp_double_array);
        
        //--------------------------------------------------------
        // fill all the structures
        //--------------------------------------------------------
        for (int i=0; i<n; ++i) {
            mxSetFieldByNumber(p, i, 0, mxCreateString("AEX") );
            mxSetFieldByNumber(p, i, 1, mxCreateDoubleScalar(2.178*i) );
            mxSetFieldByNumber(p, i, 2, mxCreateDoubleScalar( 3.1415*i) );
        }
        
        //--------------------------------------------------------
        // de-allocate the temp arrays
        //--------------------------------------------------------
        mxDestroyArray(tmp_double_array);        
    }
    
    void new_limit_order( int ins, const mxArray *in[] )
    {

        std::string     protocol;
        std::string     sender_id;
        std::string     target_id;
        std::string     time_type;
        std::string     order_id;
        std::string     symbol;
        std::string     buy_sell;
        long            volume;
        double          price;
        
        if (ins<10)                               { mexPrintf(usage_new_limit_order); mexErrMsgTxt("Not enough arguments: this function call has 10 arguments."); }
        if (!pmxArrayToString(in[1], protocol))   { mexPrintf(usage_new_limit_order); mexErrMsgTxt("Unable to read protocol (arg 2)."); }
        if (!pmxArrayToString(in[2], sender_id))  { mexPrintf(usage_new_limit_order); mexErrMsgTxt("Unable to read sender (arg 3)."); }
        if (!pmxArrayToString(in[3], target_id))  { mexPrintf(usage_new_limit_order); mexErrMsgTxt("Unable to read target (arg 4)."); }
        if (!pmxArrayToString(in[4], time_type))  { mexPrintf(usage_new_limit_order); mexErrMsgTxt("Unable to read time (arg 5)."); }
        if (!pmxArrayToString(in[5], order_id ))  { mexPrintf(usage_new_limit_order); mexErrMsgTxt("Unable to read orderid (arg 6)."); }
        if (!pmxArrayToString(in[6], symbol   ))  { mexPrintf(usage_new_limit_order); mexErrMsgTxt("Unable to read symbol (arg 7)."); }
        if (!pmxArrayToString(in[7], buy_sell ))  { mexPrintf(usage_new_limit_order); mexErrMsgTxt("Unable to read side (arg 8)."); }
        if (!pmxArrayToLong(  in[8], volume   ))  { mexPrintf(usage_new_limit_order); mexErrMsgTxt("Unable to read volume (arg 9)."); }
        if (!pmxArrayToDouble(in[9], price    ))  { mexPrintf(usage_new_limit_order); mexErrMsgTxt("Unable to read price (arg 10)."); }

        limit_order lo(
            protocol,
            sender_id,
            target_id,
            FIX::ClOrdID(order_id),
            FIX::Symbol(symbol),
            string_to_Side(buy_sell),
            FIX::OrderQty(volume),
            FIX::Price(price)
            );
            
        lo.timeInForce = string_to_TimeInForce(time_type);
        
        m_fix_handler.send_limit_order( lo );
        
    }

};

FixClient matfix;


static void at_exit(void)
{
      mexPrintf("stopping matfix..\n");
      matfix.disconnect();
      mexPrintf("ok\n");
}


// ============================================================================
// MAIN
// ============================================================================
extern "C" 
{
    void mexFunction(int outs, mxArray *out[], int ins, const mxArray *in[]) 
    {
        mexAtExit(at_exit);
    
        
        // ----------------------------------------------------------------
        // We need at least 1 argument: "command"
        // ----------------------------------------------------------------
        std::string command;
        if (ins < 1)                             { mexPrintf(usage);mexErrMsgTxt("Missing command string (first argument)."); }
        if (!pmxArrayToString(in[0], command))   { mexPrintf(usage); mexErrMsgTxt("Unable to read the command string (first argument)."); }
    
        // ----------------------------------------------------------------
        // dispath on command
        // ----------------------------------------------------------------
        if (command == "help")      { mexPrintf(usage); return; }
        if (command == "start")             return matfix.connect( ins, in);
        if (command == "stop")              return matfix.disconnect();
        if (command == "status")            return matfix.status(out[0]);
        if (command == "new_limit_order")   return matfix.new_limit_order(ins,in);
        if (command == "orders")            return matfix.orders(out[0]);

        // ----------------------------------------------------------------
        // ..no match 
        // ----------------------------------------------------------------
        mexPrintf(usage);
        mexPrintf("unknown command [%s]\n", command.c_str() );
        mexErrMsgTxt("unknown command");
    } 
}