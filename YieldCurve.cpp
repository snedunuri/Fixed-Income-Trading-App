//
//  YieldCurve.cpp
//  Midterm Submission
//
//  Created by  Satya Nedunuri on 10/24/15.
//  Copyright © 2015  Satya Nedunuri. All rights reserved.
//

#include "YieldCurve.hpp"
#include <math.h>

void
YieldCurve::process_spread_data(){
    
    int num_periods = 0;
    for(int i = 0; i < num_lines; i++){
        SBB_date from_dt, to_dt;
        from_dt.set_from_yyyymmdd(benchmark_data->SettlementDate());
        to_dt.set_from_yyyymmdd(benchmark_data->MaturityDate());

        num_periods = Bond::calc_num_periods(from_dt, to_dt, benchmark_data->Frequency());

        yield_bonds_num_periods.push_back(num_periods);
        yield_rates.push_back(benchmark_data->Yield());
        benchmark_data++;
        
        if(num_periods == 4){
            calc_dv01_2yr(num_periods);
        }

    }
    benchmark_data -= num_lines;
}

double
YieldCurve::find_closest(Bond* bond){
    double temp_yield = 0.0;
    int num_periods = bond->get_num_periods();
    int min_distance = std::numeric_limits<int>::max();
    int bond_index = 0;
    
    for(int i = 0; i < num_lines; i++){
        int temp = abs(num_periods - yield_bonds_num_periods.at(i));
        
        if(temp < min_distance){
            min_distance = temp;
            temp_yield = yield_rates.at(i);
            bond_index = i;
        }else if (temp == min_distance){
            if(num_periods < yield_bonds_num_periods.at(bond_index)){
                temp_yield = yield_rates.at(i);
                bond_index = i;
            }
        }
    }
    return temp_yield;
}

void
YieldCurve::shift_curve(std::vector<double> shift){
    
    yield_rates[0] += shift.at(3) / 1000;
    yield_rates[1] += shift.at(0) / 1000;
    yield_rates[2] += shift.at(1) / 1000;
    yield_rates[3] += shift.at(2) / 1000;
    
    
}



void
YieldCurve::calc_dv01_2yr(int num_periods){
    double yield = benchmark_data->Yield();
    double new_up_price = 0.0;
    double new_down_price = 0.0;
    double temp1 = yield + 0.01;
    double temp2 = yield - 0.01;
    
    benchmark_data->Yield(temp1);
    
    new_up_price = calc_price(num_periods);
    
    benchmark_data->Yield( yield );
    
    benchmark_data->Yield(temp2);
    
    new_down_price = calc_price(num_periods);
    
    benchmark_data->Yield( yield );
    
    //resetting the yield rate
    dv01_2yr = ((new_up_price - 100) + (100 - new_down_price)) / -2.0;
}



double
YieldCurve::calc_price(int num_periods){
    //calculating price
    double yield = benchmark_data->Yield() / 100;
    double frequency = benchmark_data->Frequency();
    double coupon_adj = benchmark_data->Coupon() / frequency;
    double yield_adj = yield / frequency;
    double PV_factor = 1 / pow((1 + yield_adj), num_periods);
    double CF_factor = (1 - PV_factor) / yield_adj;
    double price = coupon_adj * CF_factor + 100 * PV_factor;
    return price;

}