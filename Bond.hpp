//
//  Bond.hpp
//  Midterm Submission
//
//  Created by  Satya Nedunuri on 10/24/15.
//  Copyright © 2015  Satya Nedunuri. All rights reserved.
//

#ifndef Bond_hpp
#define Bond_hpp

#include <stdio.h>
#include "SBB_io.h"
#include "SBB_date.h"
#include "SBB_ratings.h"

class Bond{
    
public:
    Bond(SBB_instrument_fields* data, int num_periods, double lgd): current_bond(data), num_periods(num_periods), current_lgd(lgd){
    }
    ~Bond(){
        
    }
    
    static int calc_num_periods(SBB_date from_dt, SBB_date to_dt, int frequency);
    double calc_coupon_bearing_price();
    double calc_dv01();
    double calc_risk();
    double calc_market_value();
    
    void set_current_price(double p) { current_price = p; }
    void set_current_dv01(double d) { current_dv01 = d; }
    void set_current_risk(double r) { current_risk = r; }
    void set_current_market_value(double m) { current_market_value = m; }
    
    double get_current_price() { return current_price; }
    double get_current_dv01() { return current_dv01; }
    double get_current_risk() { return current_risk; }
    SBB_instrument_fields* get_bond_ptr() { return current_bond; }
    int get_num_periods() { return num_periods; }
    double get_lgd() { return current_lgd; }
    double get_current_market_value() { return current_market_value; }
    
private:
    double current_price;
    double current_dv01;
    double current_risk;
    double current_lgd;
    double current_market_value;
    SBB_instrument_fields* current_bond;
    int num_periods;
    
    
};

#endif /* Bond_hpp */
