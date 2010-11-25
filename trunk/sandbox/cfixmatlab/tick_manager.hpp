#ifndef TICK_MANAGER_HPP
#define TICK_MANAGER_HPP

//---------------------------------------------
//tick class
//---------------------------------------------
class tick
{
private:
    time_type   t_;  // timetamp: seconds since epoch.
    double      p_;  //  price

public:
    //constructores
    tick() : t_( boost::posix_time::min_date_time ), p_(0) {};
    tick(const time_type t, const double p) : t_(t), p_(p) {};
    
    // accesors
    time_type   time()  const { return t_; };
    double      price() const { return p_; };
};

//---------------------------------------------
// tick buffer
//---------------------------------------------
class tick_buffer
{
public:
    boost::circular_buffer< tick > ticks;  // this will containt the ticks..
    
    // constructor
    tick_buffer() : ticks(100) {};
    
    void to_pmxArray(mxArray *&p, bool timeCol = true, bool priceCol = true, bool clear=false)
    {
        long columns = 0;
        if (timeCol) columns++;
        if (priceCol) columns++;
        if (columns == 0) return;
        
        long rows = ticks.size();
        
        p = mxCreateDoubleMatrix(rows, columns, mxREAL);
        double *data = (double *) mxMalloc( rows*columns * sizeof(double));
        
        long  elem = 0;
        
        if (timeCol) 
            for (long i=0; i<rows; ++i, elem++) 
                data[elem] = time_to_matlab(ticks[i].time());
                
        if (priceCol) 
            for (long i=0; i<rows; ++i, elem++) 
                data[elem] = ticks[i].price();
        
        mxSetPr(p, data);
        if (clear) ticks.clear();
    }
};

//---------------------------------------------
// symbol tick buffer
//---------------------------------------------
struct symbol_tick_buffers
{   
    typedef std::map< std::string, tick_buffer > container_type;
    
    container_type symbols;
  
    bool has_symbol(const std::string& symbol) const {
        return (symbols.find(symbol) != symbols.end());
    }
    
    void add_tick(const std::string& symbol, time_type time, double price) {
        symbols[symbol].ticks.push_back( tick(time,price) );
    }
    
    void symbols_to_pmxArray(mxArray *&p) const 
    {
        // check if we have symbols...
        if (symbols.size() == 0) return;

        // allocate storage
        p = mxCreateCellMatrix( (mwSize)symbols.size(), (mwSize)3 );
        
        // allocate tmp array
        mxArray *tmp_array = mxCreateDoubleMatrix(1, 1, mxREAL);
        double *pVal = mxGetPr(tmp_array);
        
        // copy data into storage
        mwIndex row = 0;
        mwSize rows = symbols.size() ;
        for (container_type::const_iterator i=symbols.begin(); i!=symbols.end(); ++i) {
        
            // set the symbol
            mxSetCell( p, row, mxCreateString( i->first.c_str() ) );
            
            // set the last (if we have one)
            if ( !i->second.ticks.empty() ) {

                *pVal = time_to_matlab( i->second.ticks.back().time() );
                mxSetCell( p,   rows + row, mxDuplicateArray(tmp_array) );

                *pVal = i->second.ticks.back().price();
                mxSetCell( p, 2*rows + row, mxDuplicateArray(tmp_array) );
            }
            row++;
        }
        
        // de-allocate tmp array
        mxDestroyArray(tmp_array);
    }
};
#endif