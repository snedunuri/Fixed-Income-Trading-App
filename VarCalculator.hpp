//
//  VarCalculator.hpp
//  Homework 8
//
//  Created by  Satya Nedunuri on 11/28/15.
//  Copyright © 2015  Satya Nedunuri. All rights reserved.
//

#ifndef VarCalculator_hpp
#define VarCalculator_hpp

#include <stdio.h>
#include "TradingBook.hpp"

using namespace std;

class VarCalculator{
    
public:
    
    VarCalculator(TradingBook tb, TradingBook yield_curve): tb(tb), yield_curve(yield_curve){
    }
    
    void process_historical_data();
    
    vector<vector<double> > all_pnl_vectors;
    vector<double> book_pnl_vector;
    vector<double> spread_collection;
    double get_var(double confid_interval);
    double get_spread_var(double confid_interval);
    
private:
    TradingBook tb;
    TradingBook yield_curve;
    
    void calculate_book_vector();
    Bond find_benchmark_treasury(vector<Bond> collection, Bond current_bond);

    
};

#endif /* VarCalculator_hpp */
