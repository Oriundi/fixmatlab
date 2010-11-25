#ifndef TYPE_CONVERSION_HPP
#define TYPE_CONVERSION_HPP

//------------------------------------------------------------
// *exact* 64 bit integers
//------------------------------------------------------------
#ifdef _MSC_VER
   typedef __int64 int64_t; // Define it from MSVC's internal type
#else
   #include <stdint.h>      // Use the C99 official header
#endif

//------------------------------------------------------------
// Matlab
//------------------------------------------------------------
#include "mex.h"

//------------------------------------------------------------
// QuickFix
//------------------------------------------------------------
#include "quickfix/FixValues.h"
#include "quickfix/FixFields.h"

//------------------------------------------------------------
// Misc
//------------------------------------------------------------
#include <string>
#include "time.hpp"


double time_to_matlab(const time_type& t)
{
    // Matlab serial dates have a value that is 719529 on 1/1/1970:
    // >> datenum(1970,1,1)
    // Matlab serial dates increment with 1 per day 
    
    double days_since_1970 = (t.date() - boost::gregorian::date(1970,boost::gregorian::Jan,1)).days();
    double secs = t.time_of_day().total_seconds();
    double micros = static_cast<double>(t.time_of_day().total_microseconds() % 1000000);

    return days_since_1970 + 719529.0 + (secs + micros/1000000.0) / 86400.0;
}

time_type matlab_to_time(const double d)
{
    int64_t t = static_cast<int64_t>(d * 1000000);
    
    int64_t micro_secs = t % 1000000;
    int64_t secs = (t - micro_secs) / 1000000;
    return boost::posix_time::ptime( 
        boost::gregorian::date( 1970, boost::gregorian::Jan, 1), 
        boost::posix_time::seconds( static_cast<long>(secs) ) + 
        boost::posix_time::microseconds( micro_secs ) 
        );
}

bool pmxArrayToString(const mxArray *p, std::string& s)
{
    if (mxIsChar(p) != 1) return false;
    if (mxGetM(p) != 1) return false;
    int n = ( mxGetM(p) * mxGetN(p) ) + 1;
    char * c_str = (char *)mxCalloc(n, sizeof(char));
    int status = mxGetString(p, c_str, n);
    if (status != 0) return false;
    s = c_str;
    return true;
}

bool pmxArrayToInt(const mxArray *p, int& i)
{
    if (!mxIsNumeric(p)) return false;
    i = (int)(mxGetScalar(p));
    return true;
}

bool pmxArrayToLong(const mxArray *p, long& l)
{
    if (!mxIsNumeric(p)) return false;
    l = (long)(mxGetScalar(p));
    return true;
}

bool pmxArrayToDouble(const mxArray *p, double& d)
{
    if (!mxIsNumeric(p)) return false;
    d = (double)(mxGetScalar(p));
    return true;
}

FIX::Side string_to_Side(const std::string& s)
{
    if (s == "B"  ) return FIX::Side( FIX::Side_BUY );
    if (s == "S"  ) return FIX::Side( FIX::Side_SELL );
    if (s == "SS" ) return FIX::Side( FIX::Side_SELL_SHORT );
    if (s == "SSE") return FIX::Side( FIX::Side_SELL_SHORT_EXEMPT );
    if (s == "C"  ) return FIX::Side( FIX::Side_CROSS );
    if (s == "CS" ) return FIX::Side( FIX::Side_CROSS_SHORT );

    throw std::exception();
}


std::string Side_to_string_small(const FIX::Side& f)
{
    if (f == FIX::Side( FIX::Side_BUY)) return "B";
    if (f == FIX::Side( FIX::Side_SELL)) return "S";
    if (f == FIX::Side( FIX::Side_SELL_SHORT)) return "SS";
    if (f == FIX::Side( FIX::Side_SELL_SHORT_EXEMPT)) return "SSE";
    if (f == FIX::Side( FIX::Side_CROSS)) return "C";
    if (f == FIX::Side( FIX::Side_CROSS_SHORT)) return "CS";
    return "???";
}

FIX::OrdType string_to_OrdType(const std::string& s)
{
    if (s == "M" ) return FIX::OrdType( FIX::OrdType_MARKET );
    if (s == "L" ) return FIX::OrdType( FIX::OrdType_LIMIT );
    if (s == "S" ) return FIX::OrdType( FIX::OrdType_STOP );
    if (s == "SL") return FIX::OrdType( FIX::OrdType_STOP_LIMIT );
    
    throw std::exception();
}

FIX::TimeInForce string_to_TimeInForce(const std::string& s)
{
    if (s == "DAY") return FIX::TimeInForce( FIX::TimeInForce_DAY );
    if (s == "IOC") return FIX::TimeInForce( FIX::TimeInForce_IMMEDIATE_OR_CANCEL );
    if (s == "OPG") return FIX::TimeInForce( FIX::TimeInForce_AT_THE_OPENING );
    if (s == "GTC") return FIX::TimeInForce( FIX::TimeInForce_GOOD_TILL_CANCEL );
    if (s == "GTX") return FIX::TimeInForce( FIX::TimeInForce_GOOD_TILL_CROSSING );
    
    throw std::exception();
}

std::string TimeInForce_to_string_small(const FIX::TimeInForce& f)
{
    if (f == FIX::TimeInForce(FIX::TimeInForce_DAY)) return "DAY";
    if (f == FIX::TimeInForce(FIX::TimeInForce_IMMEDIATE_OR_CANCEL)) return "IOC";
    if (f == FIX::TimeInForce(FIX::TimeInForce_AT_THE_OPENING)) return "OPG";
    if (f == FIX::TimeInForce(FIX::TimeInForce_GOOD_TILL_CANCEL)) return "GTC";
    if (f == FIX::TimeInForce(FIX::TimeInForce_GOOD_TILL_CROSSING)) return "GTX";
    return "???";
}

std::string OrdStatus_to_string(const FIX::OrdStatus& ordStatus)
{
    switch ( static_cast<char>(ordStatus) ) {
        case ' ' : return std::string("Pending New");
        case '0' : return std::string("New");
        case '1' : return std::string("Partially filled");
        case '2' : return std::string("Filled");
        case '3' : return std::string("Done for day");
        case '4' : return std::string("Canceled");
        case '5' : return std::string("Replaced");
        case '6' : return std::string("Pending Cancel");
        case '7' : return std::string("Stopped");
        case '8' : return std::string("Rejected");
        case '9' : return std::string("Suspended");
        case 'A' : return std::string("Pending New");
        case 'B' : return std::string("Calculated");
        case 'C' : return std::string("Expired");
        case 'D' : return std::string("Accepted for bidding");
        case 'E' : return std::string("Pending Replace");
        default: return std::string("#ERROR: unknown OrdStatus!");
    }
}

std::string ExecType_to_string(const FIX::ExecType execType)
{
    switch (  static_cast<char>(execType) ) {
        case '0' : return std::string("New");
        case '1' : return std::string("Partial fill");
        case '2' : return std::string("Fill");
        case '3' : return std::string("Done for day");
        case '4' : return std::string("Canceled");
        case '5' : return std::string("Replace");
        case '6' : return std::string("Pending Cancel");
        case '7' : return std::string("Stopped");
        case '8' : return std::string("Rejected");
        case '9' : return std::string("Suspended");
        case 'A' : return std::string("Pending New");
        case 'B' : return std::string("Calculated"); 
        case 'C' : return std::string("Expired");
        case 'D' : return std::string("Restated");
        case 'E' : return std::string("Pending Replace");
        default: return std::string("#ERROR: unknown ExecType!");
    }
}

/// Identifies transaction type
std::string ExecTransType_to_string(const FIX::ExecTransType& execTransType) 
{
    switch (  static_cast<char>(execTransType) ) {
        // The NEW transaction type indicates that this message represents 
        //   a new order,
        //   a change in status of the order, 
        //   or a new fill against an existing order.
        //
        case '0' : return std::string("New");
        case '1' : return std::string("Cancel");
        case '2' : return std::string("Correct");
        case '3' : return std::string("Status");
        default: return std::string("#ERROR: unknown ExecTransType!");
    }
}
#endif