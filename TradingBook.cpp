//
//  TradingBook.cpp
//  Midterm Submission
//
//  Created by  Satya Nedunuri on 10/24/15.
//  Copyright © 2015  Satya Nedunuri. All rights reserved.
//

#include "TradingBook.hpp"
#include "Bond.hpp"
#include "SBB_date.h"
#include "SBB_ratings.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctime>

using namespace std;

void
TradingBook::process_bonds(YieldCurve yield_curve){
    
        for(int i = 0; i < num_bonds; i++){
        
        string type = data->ValType();
        SBB_date from_dt, to_dt;
        from_dt.set_from_yyyymmdd(data->SettlementDate());
        to_dt.set_from_yyyymmdd(data->MaturityDate());
        int num_periods = Bond::calc_num_periods(from_dt, to_dt, data->Frequency());
        SBB_bond_ratings rating;
        double current_lgd = rating.LGD_given_SnP_Fitch(data->Quality()) * data->Amount();
        
        Bond *current_bond = new Bond(data, num_periods, current_lgd);
        
        if(type.compare("YIELD") == 0){
            current_bond->set_current_price(current_bond->calc_coupon_bearing_price());
            current_bond->set_current_dv01(current_bond->calc_dv01());
            current_bond->set_current_risk(current_bond->calc_risk());
            current_bond->set_current_market_value(current_bond->calc_market_value());
        }
        else{
            double closest_yield = yield_curve.find_closest(current_bond);
            SBB_instrument_fields* ptr = current_bond->get_bond_ptr();
            double spread = ptr-> Spread() / 100;
            double total_yield = closest_yield + spread;
            ptr->Yield(total_yield);
            
            current_bond->set_current_price(current_bond->calc_coupon_bearing_price());
            current_bond->set_current_dv01(current_bond->calc_dv01());
            current_bond->set_current_risk(current_bond->calc_risk());
            current_bond->set_current_market_value(current_bond->calc_market_value());
            
            //set the yield back
            ptr->Yield(spread * 100);
            
        }
        
        //get the remaining term of the bond
        int remaining_term = num_periods / 2;
        
        //place bond into proper bucket
        place_in_bucket(current_bond, remaining_term);
        
        bond_collection.push_back(*current_bond);
            
        ++data;
    }
    
    
}

void
TradingBook::print_info_per_bond(){
    
    for(int i = 0; i < bond_collection.size(); i++){
        Bond current_bond = bond_collection[i];
        SBB_instrument_fields* bond = current_bond.get_bond_ptr();
        
         printf("%s %.3f %.3f %.3f\n", bond->SecurityID(), current_bond.get_current_price(),
                current_bond.get_current_dv01(), current_bond.get_current_risk());
    }
}
   
void
TradingBook::place_in_bucket(Bond* current_bond, int remaining_term){
    if(remaining_term == 0)
        return;
    
    if(remaining_term > 0 && remaining_term <= 2){
        two_yr_bucket.push_back(*current_bond);
    }else if(remaining_term > 2 && remaining_term <= 5){
        five_yr_bucket.push_back(*current_bond);
    }else if (remaining_term > 5 && remaining_term <= 10){
        ten_yr_bucket.push_back(*current_bond);
    }else{
        thirty_yr_bucket.push_back(*current_bond);
    }
}

double
TradingBook::calc_30yr_hedge(YieldCurve yield_curve){
    
    double sum_risk = 0;
    for(int i = 0; i < thirty_yr_bucket.size(); i++){
        sum_risk += thirty_yr_bucket[i].get_current_risk();
    }
    
    return (sum_risk / yield_curve.get_dv01_2yr()) * -1;
}

double
TradingBook::market_value_change_100bp_up(){
    
    double sum_market_val = 0.0;
    for(int i = 0; i < bond_collection.size(); i++){
        Bond current_bond = bond_collection[i];
        SBB_instrument_fields* ptr;
        ptr = current_bond.get_bond_ptr();
        double current_yield = ptr->Yield();
        ptr->Yield(current_yield + 1);
        
        double new_price = current_bond.calc_coupon_bearing_price();
        
        sum_market_val += new_price / 100 * ptr->Amount();
        
        //change the yield back
        ptr->Yield(current_yield);
    }
    
    double change = (sum_market_val * 1000) - (total_market_value * 1000);
    
    return change;
}
