//
//  VarCalculator.cpp
//  Homework 8
//
//  Created by  Satya Nedunuri on 11/28/15.
//  Copyright © 2015  Satya Nedunuri. All rights reserved.
//

#include "VarCalculator.hpp"
#include "Bond.hpp"
#include <algorithm>

using namespace std;

void
VarCalculator::process_historical_data(){
    
    vector<Bond> bond_collection = tb.get_bond_collection();
    // I added this @rakan, declare vector that will contain spread priced bonds pnl change
    
    for(int i = 0; i < bond_collection.size(); i++){
    
        char filename_buffer[50];
        Bond current_bond = bond_collection.at(i);
        SBB_instrument_fields* bond_ptr = current_bond.get_bond_ptr();
        
        snprintf(filename_buffer, sizeof(filename_buffer), "var/%s.txt", bond_ptr->SecurityID());
    
        const char* name = (const char*) filename_buffer;
    
        SBB_var_input_file var_data;
        int num_records = 0;
        
        var_data.open(name);
    
        SBB_var_day_record* var_rec_ptr = var_data.records(num_records);
        
        //create the PnL vector
        vector<double> pnl_vector;
        
        //get the amount
        double amount = bond_ptr->Amount();
        
        //get todays yield
        double todays_yield = bond_ptr->Yield();
        
        //get the base price
        double base_price = current_bond.get_current_price();
        
        if(var_rec_ptr->is_yield_priced()){
            
            //need to start from the last entry in the file and work up
            for(int j = 2; j < num_records; j++){
                
                double current_yield = var_rec_ptr[j].ValValue();
                double prev_yield = var_rec_ptr[j-1].ValValue();
                double yield_change = current_yield - prev_yield;
                
                //change yield to calculate new price
                bond_ptr->Yield(todays_yield + yield_change);
                double new_price = current_bond.calc_coupon_bearing_price();
                
                double price_change = new_price - base_price;
                double total_PnL_change = price_change/100 * amount;
                    
                //add to the PnL vector
                pnl_vector.push_back(total_PnL_change);
            }

        }else{
            
            //get the benchmark treasury
            vector<Bond> treasury_collection = yield_curve.get_bond_collection();
            Bond treasury = find_benchmark_treasury(treasury_collection, current_bond);
            SBB_instrument_fields* treasury_ptr = treasury.get_bond_ptr();
            
            char benchmark_name[50];
            
            snprintf(benchmark_name, sizeof(benchmark_name), "var/%s.txt", treasury_ptr->SecurityID());
            
            const char* benchmark_file = (const char*) benchmark_name;
            
            SBB_var_input_file benchmark_data;
            int num_records2 = 0;
            
            var_data.open(benchmark_file);
            
            SBB_var_day_record* benchmark_rec_ptr = var_data.records(num_records2);
            
            double tyield = treasury_ptr->Yield();
            
            //find the closest bond in the yield curve
            for(int j = 2; j < num_records2; j++){
                
                //calc change for the treasury
                double current_tyield = benchmark_rec_ptr[j].ValValue();
                double prev_tyield = benchmark_rec_ptr[j-1].ValValue();
                double daily_tyield_change = current_tyield - prev_tyield;
                double total_yield = tyield + daily_tyield_change;
                
                //change yield of bond and recalculate price
                treasury_ptr->Yield(total_yield);
                double new_price = treasury.calc_coupon_bearing_price();
                treasury_ptr->Yield(tyield); //change back yield
                
                //calculate the total PnL change for the treasury
                double price_change = new_price - treasury.get_current_price();
                double total_PnL_change = price_change/100 * treasury_ptr->Amount();
                
                //calculate the yield change for the bond
                double current_spread = var_rec_ptr[j].ValValue();
                double prev_spread = var_rec_ptr[j-1].ValValue();
                double spread_change = current_spread - prev_spread;
                double total_spread_change = (todays_yield + spread_change) / 100;
                double total_yield_change = total_spread_change + total_yield;
                
                //calculate new bond price
                bond_ptr->Yield(total_yield_change);
                double new_bond_price = current_bond.calc_coupon_bearing_price();
                bond_ptr->Yield(todays_yield);
                
                //calaculate bond price
                double bond_price_change = new_bond_price - base_price;
                double total_bond_PnL_change = bond_price_change/100 * amount;
                
                //add to the PnL vector
                pnl_vector.push_back(total_bond_PnL_change);
                
                // I added this @rakan, this will push above total pnl change to vector
                spread_collection.push_back(total_bond_PnL_change);
            }

        }
        
        //add current vector to total vectors
        all_pnl_vectors.push_back(pnl_vector);
    
    }
    
    //calculate the book vector
    calculate_book_vector();
    
    //sort the book vector
    sort(book_pnl_vector.begin(), book_pnl_vector.end());
    
    // this will sort the spread price pnl
    sort(spread_collection.begin(),spread_collection.end());
    
}

void
VarCalculator::calculate_book_vector(){
    
    //assumes that all historical files have same number of entries
    for(int i = 0; i < all_pnl_vectors.at(0).size(); i++){
        
        double sum = 0;
        for(int j = 0; j < all_pnl_vectors.size(); j++){
            sum += all_pnl_vectors.at(j).at(i);
            
        }
        book_pnl_vector.push_back(sum);
    }
    
}

double
VarCalculator::get_spread_var(double conf_interval){
    long size = spread_collection.size();
    int index = (int) (size - (conf_interval / 100) * size);
    
    return spread_collection[index] / 1000; //var is reported in millions
}

double
VarCalculator::get_var(double conf_interval){
    long size = book_pnl_vector.size();
    int index = (int) (size - (conf_interval / 100) * size);
    
    return book_pnl_vector[index] / 1000; //var is reported in millions
}

Bond
VarCalculator::find_benchmark_treasury(vector<Bond> collection, Bond current_bond){
    
    int bond_index = 0;
    int num_periods = current_bond.get_num_periods();
    int min_distance = std::numeric_limits<int>::max();
    
    for(int i = 0; i < collection.size(); i++){
        int temp = abs(num_periods - collection.at(i).get_num_periods());
        if(temp < min_distance){
            min_distance = temp;
            bond_index = i;
        }
    }
    Bond ans = collection[bond_index];
    return ans;
}