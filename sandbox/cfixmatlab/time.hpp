#ifndef TIME_HPP
#define TIME_HPP

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <sstream>



    //! Gregorian Calendar (Date)
    typedef boost::gregorian::date              date_type;
    typedef boost::gregorian::date_duration     date_duration_type;
    typedef boost::gregorian::date_period       date_period_type;
    
    //! POSIX time system (Date + Time, UTC)
    typedef boost::posix_time::ptime            time_type;
    typedef boost::posix_time::time_duration    time_duration_type;
    typedef boost::posix_time::time_period      time_period_type;

    time_type now() 
    { 
        return boost::posix_time::microsec_clock::universal_time(); 
    }

    /// convert timefields to a time
    time_type timefields_to_time(
        const int year, const int month, const int day, 
        const int hours, const int minutes, const int seconds, 
        const long micro)
    {
        return boost::posix_time::ptime( 
                boost::gregorian::date( year, month, day), 
                boost::posix_time::seconds( (hours*60 + minutes)*60 + seconds ) + 
                boost::posix_time::microseconds( micro ) 
           );
    }
    
    /// convert timefields to a string
    std::string timefields_to_string(
        const int year, const int month, const int day, 
        const int hours, const int minutes, const int seconds, 
        const long micro)
    {
        std::string s("0000-00-00 00:00:00.000000");
        s[0] +=  year/1000;
        s[1] += (year%1000)/100;
        s[2] += (year%100)/10;
        s[3] +=  year%10;
        
        s[5] += month/10;
        s[6] += month%10;

        s[8] += day/10;
        s[9] += day%10;

        s[11] += hours/10;
        s[12] += hours%10;
        
        s[14] += minutes/10;
        s[15] += minutes%10;

        s[17] += seconds/10;
        s[18] += seconds%10;
        
        s[20] += static_cast<char>( micro/100000 );
        s[21] += static_cast<char>((micro%100000)/10000);
        s[22] += static_cast<char>((micro%10000 )/1000);
        s[23] += static_cast<char>((micro%1000  )/100);
        s[24] += static_cast<char>((micro%100   )/10);
        s[25] +=  micro%10;
        return s;
    }
    
    /// convert a time to a list of fields
    bool time_to_timefields(
        const time_type& pt,
        int& year, int& month, int& day, 
        int& hours, int& minutes, int& seconds, 
        long& micro)
    {
        date_type          d = pt.date();
        time_duration_type t = pt.time_of_day();

        year    = d.year();
        month   = d.month();
        day     = d.day();
        hours   = t.hours();
        minutes = t.minutes();
        seconds = t.seconds();
        micro   = (t.total_microseconds()%1000000);
        return true;
    }
    
    /// convert to string to a list of fields
    bool string_to_timefields(
        const std::string& s,
        int& year, int& month, int& day, 
        int& hours, int& minutes, int& seconds, 
        long& micro)
   {
        year                    = s[0]-'0';
        year *= 10;    year    += s[1]-'0';
        year *= 10;    year    += s[2]-'0';
        year *= 10;    year    += s[3]-'0';

        month                   = s[5]-'0';
        month*= 10;    month   += s[6]-'0';
        
        day                     = s[8]-'0';
        day  *= 10;    day     += s[9]-'0';

        hours                   = s[11]-'0';
        hours *= 10;   hours   += s[12]-'0';
        
        minutes                 = s[14]-'0';
        minutes *= 10; minutes += s[15]-'0';
        
        seconds                 = s[17]-'0';
        seconds *= 10; seconds += s[18]-'0';

        micro                   = s[20]-'0';
        micro *=10;    micro   += s[21]-'0';
        micro *=10;    micro   += s[22]-'0';
        micro *=10;    micro   += s[23]-'0';
        micro *=10;    micro   += s[24]-'0';
        micro *=10;    micro   += s[25]-'0';
		return true;
   }
   
    /// conver a time to the iso string: "YYYY-MM-DD HH:MM:SS.FFFFFF"
    std::string time_to_string(const time_type& pt)
    {
        int year, month, day, hours, minutes, seconds;
        long micro;

        time_to_timefields( pt, year, month, day, hours, minutes, seconds, micro);
        return timefields_to_string(year, month, day, hours, minutes, seconds, micro);
    }

#endif