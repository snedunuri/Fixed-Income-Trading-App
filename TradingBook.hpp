//
//  TradingBook.hpp
//  Midterm Submission
//
//  Created by  Satya Nedunuri on 10/24/15.
//  Copyright © 2015  Satya Nedunuri. All rights reserved.
//

#ifndef TradingBook_hpp
#define TradingBook_hpp

#include <stdio.h>
#include "SBB_io.h"
#include <vector>
#include "Bond.hpp"
#include "YieldCurve.hpp"

class TradingBook{

public:
    
    TradingBook(SBB_instrument_fields* data, int num_bonds){
        this->data = data;
        this->num_bonds = num_bonds;
        largest_long_position = std::numeric_limits<int>::min();
        largest_short_position = std::numeric_limits<int>::max();
        position_with_most_risk = 0.0;
        total_risk = 0.0;
        total_position = 0.0;
        total_LGD = 0.0;
    }
    ~TradingBook(){
        
    }
    
    void process_bonds(YieldCurve yield_curve);
    
    void print_info_per_bond();
    
    double market_value_change_100bp_up();
    
    double calc_total_position();
    
   
    
    std::vector<Bond> bond_collection;
    std::vector<Bond> two_yr_bucket;
    std::vector<Bond> five_yr_bucket;
    std::vector<Bond> ten_yr_bucket;
    std::vector<Bond> thirty_yr_bucket;
    
    int get_largest_long_position() { return largest_long_position; }
    int get_largest_short_position() { return largest_short_position; }
    double get_position_with_most_risk() { return position_with_most_risk; }
    double get_total_risk() { return total_risk; }
    double calc_30yr_hedge(YieldCurve yield_curve);
    double get_total_market_value() { return total_market_value * 1000; }
    double get_total_position() { return total_position; }
    double get_total_LGD() { return total_LGD; }
    std::vector<Bond> get_bond_collection() { return bond_collection; }
   
    
    
    void set_largest_long_position(int p) { largest_long_position = p; }
    void set_largest_short_position(int s) { largest_short_position = s; }
    void set_position_with_most_risk(double r) { position_with_most_risk = r; }
    void set_total_risk(double r) { total_risk = r; }
    
    
private:
    SBB_instrument_fields *data;
    int num_bonds;
    int largest_long_position;
    int largest_short_position;
    double position_with_most_risk;
    double total_risk;
    double total_market_value;
    double total_position;
    double total_LGD;
    
    void place_in_bucket(Bond* current_bond, int remaining_term);
};

#endif /* TradingBook_hpp */
