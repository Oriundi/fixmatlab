#ifndef USAGE_HPP
#define USAGE_HPP
const char *usage = "\
usage:\n\
matfix(command, ... )\n\
\n\t'start'           : 'Start the FIX client. Connects to the FIX server, login'\n\
\n\t'stop'            : 'Stop the FIX client. Logout, disconnects to the FIX server'\n\
\n\t'status'          : 'Show the status of the FIX client'\n\
\n\t'new_limit_order' : 'Send a new limit order.'\n\
";

/*
cancel_order

*/

const char *usage_new_limit_order = "\
usage:\n\
matfix('new_order_single_limit', proto, sender, target, time, orderid, symbol, side, volume, price)\n\
\n\tproto:    string : FIX protocol string\n\
\t\t\t\t'FIX.4.3'\n\
\t\t\t\t'FIX.4.4'\n\
\n\tsender:   string : Sender Computer ID, e.g. 'CLIENT'\n\
\n\ttarget:   string : Target Computer ID, e.g. 'EXECUTOR'\n\
\n\ttime:     string : Time in Force\n\
\t\t\t\t'DAY' Day order\n\
\t\t\t\t'IOC' Immediate Or Cancel\n\
\t\t\t\t'OPG' At The Opening\n\
\t\t\t\t'GTC' Good Till Cancel\n\
\t\t\t\t'GTX' Good Till Crossing\n\
\n\torderid:  string : Order ID e.g. 'x03242'\n\
\n\tsymbol:   string : Symbol, e.g. 'IBM'\n\
\n\tside:     string : Side indicator\n\
\t\t\t\t'B'   Buy\n\
\t\t\t\t'S'   Sell\n\
\t\t\t\t'SS'  Sell Short\n\
\t\t\t\t'SSE' Sell Short Exempt\n\
\t\t\t\t'C'   Cross\n\
\t\t\t\t'CS'  Cross Short\n\
\t\t\t\t'CSE' Cross Short Exempt\n\
\n\tvolume:   long   : Quantity of the order, e.g. 100\n\
\n\tprice:    double : Limit price, e.g. 54.60\n\
";

#endif
