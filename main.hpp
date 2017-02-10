//
//  main.hpp
//  Homework 6
//
//  Created by  Satya Nedunuri on 11/7/15.
//  Copyright © 2015  Satya Nedunuri. All rights reserved.
//

#ifndef main_h
#define main_h

#include "ChangeBond.hpp"

using namespace std;

int main(int argc, const char * argv[]);
void run_server();

YieldCurve main_yield_curve;

std::vector<std::string>
&split(const std::string &s, char delim,std::vector<std::string> &elems);

std::vector<std::string>
split(const std::string &s, char delim);

void processTradingBooks();
void processTradingBooks(YieldCurve yield_curve);
YieldCurve getNewYieldCurve();
YieldCurve getModifiedYieldCurve(double yield_shift);
void calculateIntradayChanges();
string bondToString(vector<Bond> bonds);
string changeBondToString(vector<ChangeBond> bonds);
string getAllDataString();

double calculate_bucket_risk(vector<Bond> bonds);
double calculate_bucket_mv(vector<Bond> bonds);
double calculate_bucket_hedge(vector<Bond> bonds);

int get_bucket_amount(vector<Bond> bonds);
double get_bucket_dv01(vector<Bond> bonds);

string yield_data_string;

vector< double > vars;
vector<Bond> openTB_bonds;
vector<Bond> closeTB_bonds;
vector<ChangeBond> change_bonds;

double real_time, user_time, system_time;

double two_year_risk, five_year_risk, ten_year_risk, thirty_year_risk;
double two_year_mv, five_year_mv, ten_year_mv, thirty_year_mv;
double two_year_hedge, five_year_hedge, ten_year_hedge, thirty_year_hedge;


double var_open;
double var_close;
double spread_var_open;
double spread_var_close;
double yield_var_open;
double yield_var_close;


#endif /* main_h */
