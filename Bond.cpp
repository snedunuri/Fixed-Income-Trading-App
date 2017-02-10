//
//  Bond.cpp
//  Midterm Submission
//
//  Created by  Satya Nedunuri on 10/24/15.
//  Copyright © 2015  Satya Nedunuri. All rights reserved.
//

#include "Bond.hpp"
#include <math.h>

int
Bond::calc_num_periods(SBB_date from_dt, SBB_date to_dt, int frequency){
    int period_count = 0;
    int period_length = 12/frequency;
    
    while (from_dt != to_dt) {
        to_dt.add_months(-period_length);
        period_count++;
    }
    return period_count;
}

double
Bond::calc_coupon_bearing_price(){
    //calculating price
    double yield = current_bond->Yield() / 100;
    double frequency = current_bond->Frequency();
    double coupon_adj = current_bond->Coupon() / frequency;
    double yield_adj = yield / frequency;
    double PV_factor = 1 / pow((1 + yield_adj), num_periods);
    double CF_factor = (1 - PV_factor) / yield_adj;
    double price = coupon_adj * CF_factor + 100 * PV_factor;
    return price;
}

double
Bond::calc_dv01(){
        
    double yield = current_bond->Yield();
    double new_up_price = 0.0;
    double new_down_price = 0.0;
    double temp1 = yield + 0.01;
    double temp2 = yield - 0.01;
    
    current_bond->Yield(temp1);
    
    new_up_price = calc_coupon_bearing_price();
    
    current_bond->Yield( (int) yield );
    current_bond->Yield(temp2);
    
    new_down_price = calc_coupon_bearing_price();
    
    current_bond->Yield( yield );
    
    //resetting the yield rate
    double dv01 = ((new_up_price - 100) + (100 - new_down_price)) / -2.0;
    
    return dv01;

}

double
Bond::calc_risk(){
   return (get_current_dv01() * current_bond->Amount()) / 100;
}

double
Bond::calc_market_value(){
    return current_price / 100 * current_bond->Amount();
}