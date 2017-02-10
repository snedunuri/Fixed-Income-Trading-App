//
//  YieldCurve.hpp
//  Midterm Submission
//
//  Created by  Satya Nedunuri on 10/24/15.
//  Copyright © 2015  Satya Nedunuri. All rights reserved.
//

#ifndef YieldCurve_hpp
#define YieldCurve_hpp

#include <stdio.h>
#include "SBB_io.h"
#include "SBB_date.h"
#include "Bond.hpp"
#include <vector>

class YieldCurve{
    
public:
    YieldCurve(SBB_instrument_fields* benchmark_data, int num_lines){
        this->benchmark_data = benchmark_data;
        this->num_lines = num_lines;
        this->yield = benchmark_data->Yield();
            }
    ~YieldCurve(){
        
    }
    
    YieldCurve(){};
    
    //YieldCurve& operator=(const YieldCurve& a);
    
    std::vector<int> yield_bonds_num_periods;
    std::vector<double> yield_rates;
    double find_closest(Bond* bond);
    void process_spread_data();
    SBB_instrument_fields* get_benchmark_data() { return benchmark_data; }
    double get_dv01_2yr(){ return dv01_2yr; }
    std::vector<double> get_yield_rates() { return yield_rates; }
    
    void shift_curve(std::vector<double> shift);
    
private:
    double yield;
    double dv01_2yr;
    SBB_instrument_fields* benchmark_data;
    int num_lines;
    
    void calc_dv01_2yr(int num_periods);
    double calc_price(int num_periods);
};

#endif /* YieldCurve_hpp */
