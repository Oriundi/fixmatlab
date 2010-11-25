#ifndef LIMIT_ORDER_HPP
#define LIMIT_ORDER_HPP

#include "type_conversion.hpp"

struct limit_order {
public:
        // Session details
        std::string     protocol;
        std::string     sender_id;
        std::string     target_id;
        
        FIX::TimeInForce timeInForce;
        
        // order detailss
        std::string    clOrdID;    // Unique identifier for Order as assigned by institution
        std::string     symbol;     //
        char       side;       //
        long   orderQty;   // Either CashOrderQty or OrderQty is required. 
        double      price;      // Required if specified on the order

        // fill status details
        long  leavesQty;  // Amount of shares open for further execution
        long     cumQty;     // Currently executed shares for chain of orders
        char  ordStatus;  // Identifies current status of order
        
        
public:
        limit_order() {};
        
        limit_order(
            const std::string&     protocol_,
            const std::string&     sender_id_,
            const std::string&     target_id_,
            const FIX::ClOrdID&    clOrdID_,
            const FIX::Symbol&     symbol_,
            const FIX::Side&       side_,
            const FIX::OrderQty&   orderQty_,
            const FIX::Price&      price_) 
            :   protocol(protocol_),
                sender_id(sender_id_),
                target_id(target_id_),
                clOrdID(clOrdID_),  
                symbol(symbol_),   
                side(side_),     
                orderQty(orderQty_), 
                price(price_),
                // --------------------- default values ------------
                timeInForce(),
                leavesQty(orderQty_),
                cumQty(0),
                ordStatus(' ')
            {};
        
        void update(const limit_order& lo)
        {
             mexPrintf("update: %f <- %f \n",(double)leavesQty, (double)lo.leavesQty);
             mexPrintf("update: %f <- %f \n",(double)cumQty, (double)lo.cumQty);
             //mexPrintf("update: %c <- %c \n",ordStatus, lo.ordStatus);

            //protocol  = lo.protocol;    // todo ASSERT instead of asign
            //sender_id = lo.sender_id;   // todo ASSERT instead of asign
            //target_id = lo.target_id;   // todo ASSERT instead of asign
            
            clOrdID   = lo.clOrdID;     // todo ASSERT instead of asign
            //symbol    = lo.symbol;      // todo ASSERT instead of asign
            //side      = lo.side;        // todo ASSERT instead of asign
            //orderQty  = lo.orderQty;    // todo ASSERT instead of asign
            //price     = lo.price;       // todo ASSERT instead of asign
            
            leavesQty = FIX::LeavesQty(lo.leavesQty);
            cumQty     = lo.cumQty;
            ordStatus = lo.ordStatus;
            
            // todo ASSERT orderQty == leaverQty + cumQty
        }
        
};


class limit_order_manager {

private:
    /// a map m_order_id -> limit_order
    typedef std::map< std::string, limit_order > OMAP;
    OMAP omap;
    
public:

    void Handle_send_limit_order(const limit_order& lo)
    {
        omap.insert( make_pair( std::string(lo.clOrdID), lo) );
    }
    
    /// Update the private variables with the data from a FIX Execution Report
    template <typename T>
    void Handle_ExecutionReport(const T& message,  const FIX::SessionID& session)
    {
        mexPrintf("Handle_ExecutionReport(. \n");
        limit_order lo;
        
        // get the FIX session details
        lo.protocol  = session.getBeginString().getValue();
        lo.sender_id = session.getSenderCompID().getValue();
        lo.target_id = session.getTargetCompID().getValue();
        
        // Get the order field from the message
        FIX::ClOrdID clOrdID; 
        message.get( clOrdID ); 
        lo.clOrdID = clOrdID;
        
        //message.get( lo.symbol );
        //message.get( lo.side );
        //message.get( lo.orderQty );
        //message.get( lo.price );

        // Get the fill-status details
        FIX::OrdStatus ordStatus; 
        message.get( ordStatus ); 
        lo.ordStatus = ordStatus;
        
        FIX::LeavesQty leavesQty;
        message.get( leavesQty );
        lo.leavesQty = leavesQty;
        
        FIX::CumQty cumQty;
        message.get( cumQty );
        lo.cumQty = cumQty;

        // check if we have this order in out map
        OMAP::iterator i = omap.find( lo.clOrdID );
        
        if (i != omap.end())  {
        mexPrintf("-> Handle_ExecutionReport(  UPDATE\n ");
            // update the status, fills etc...
            i->second.update(lo);
            
        } else {
        mexPrintf("-> Handle_ExecutionReport(  INSERT!!!\n");
            // OOPS: we get an execution report of an unknown order!!
            omap.insert( std::make_pair(lo.clOrdID, lo) );
        }

    }
    
    
    void orders_to_pmxArray(mxArray *&p) const
    {
        //--------------------------------------------------------
        // Create a Matlab Structure
        //--------------------------------------------------------
        const char *field_names[] = {
            "protocol",
            "sender_id",
            "target_id",
            "timeInForce",
            "clOrdId",
            "symbol",
            "side",
            "orderQty",
            "price",
            "leavesQty",
            "cumQty",
            "ordStatus"
        };
        

        mwSize dims[2] = {1,omap.size()};
        p = mxCreateStructArray(2, dims, sizeof(field_names)/sizeof(*field_names), field_names);
        
        //--------------------------------------------------------
        // collect all the info
        //--------------------------------------------------------
         OMAP::const_iterator i = omap.begin();
        for (int r=0; r<omap.size(); ++r) {
            mxSetFieldByNumber(p, r,  0, mxCreateString( i->second.protocol.c_str()) );
            mxSetFieldByNumber(p, r,  1, mxCreateString( i->second.sender_id.c_str()) );
            mxSetFieldByNumber(p, r,  2, mxCreateString( i->second.target_id.c_str()) );
            mxSetFieldByNumber(p, r,  3, mxCreateString( TimeInForce_to_string_small(i->second.timeInForce).c_str()) );
            mxSetFieldByNumber(p, r,  4, mxCreateString( std::string(i->second.clOrdID).c_str() ));

            mxSetFieldByNumber(p, r,  5, mxCreateString( std::string(i->second.symbol).c_str() ));
            mxSetFieldByNumber(p, r,  6, mxCreateString( Side_to_string_small(i->second.side).c_str() ));
            mxSetFieldByNumber(p, r,  7, mxCreateDoubleScalar( double(i->second.orderQty) ));
            mxSetFieldByNumber(p, r,  8, mxCreateDoubleScalar( double(i->second.price) ));
            mxSetFieldByNumber(p, r,  9, mxCreateDoubleScalar( double(i->second.leavesQty) ));
            mxSetFieldByNumber(p, r, 10, mxCreateDoubleScalar( double(i->second.cumQty) ));
            mxSetFieldByNumber(p, r, 11, mxCreateString( std::string(i->second.ordStatus,1).c_str() ));

            ++i;
        }
    }   
    
/*
public:
        std::string  ClOrdID;       //  11 Unique identifier for Order as assigned by institution
        std::string  OrderId;       //  37 Unique identifier for Order as assigned by broker.
        char         OrdStatus;     //  39 Identifies current status of order

        std::string  ExecID;        //  17 Must be unique for each Execution Report message
        char         ExecType;      // 150 Describes the specific ExecutionRpt
        char         ExecTransType; //  20 Identifies transaction type

        
        // Fill information
        long         LeavesQty;     // 151 Amount of shares open for further execution. 
        long         CumQty;        // 14 Currently executed shares for chain of orders.

        // Generic information about the (limit) order
        std::string Symbol;         // 55
        std::string Side;           // 54
        long        OrderQty;       // 38 Either CashOrderQty or OrderQty is required. 
        //long        CashOrderQty;   // 152 Either CashOrderQty or OrderQty is required. Specifies the approximate “monetary quantity” conveyed on the order.
        double      Price;          // 44 Required if specified on the order
        
        
        
        // Misc
        //double      AvgPx;          // 6
        //long        LastShares; // 32 Quantity of shares bought/sold on this (last) fill. Not required ExecTransType = 3 (Status). 

*/
};

#endif