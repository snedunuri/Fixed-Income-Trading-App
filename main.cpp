//
//  main.cpp
//  Midterm Submission
//
//  Created by  Satya Nedunuri on 10/24/15.
//  Copyright © 2015  Satya Nedunuri. All rights reserved.
//

#include <iostream>
#include "SBB_io.h"
#include "TradingBook.hpp"
#include "YieldCurve.hpp"
#include "SBB_util.h"
#include "main.hpp"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "sbb_socket.h"
//#include "SBB_util.h"
#include "IntraDayStats.hpp"
#include "VarCalculator.hpp"
#include <sstream>
#include <regex>


int main(int argc, const char * argv[]) {
   
    YieldCurve yield_curve = getNewYieldCurve();
    processTradingBooks(yield_curve);
    
    string str = getAllDataString();
    
    run_server();
    
    return 0;
}

void
run_server(){
    /*
     * get an internet domain socket
     */
    int sd;
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }
    
    /*
     * set up the socket structure
     */
    struct sockaddr_in	sock_addr;
    
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    
    //#ifdef SBB_ANY
    /* set to INADDR_ANY if want server to be open to any client on any machine */
    sock_addr.sin_addr.s_addr = INADDR_ANY;
    
    /*
     #else
     char	hostname[128];
     
     //we'll default to this host and call a section 3 func to get this host
     
     if( gethostname(hostname,sizeof(hostname)) ){
     fprintf(stderr," SBB gethostname(...) failed errno: %d\n", errno);
     exit(1);
     }
     printf("SBB gethostname() local hostname: \"%s\"\n", hostname);
     
     
     //set up socket structure for our host machine
     
     struct hostent *hp;
     if ((hp = gethostbyname(hostname)) == 0) {
     fprintf(stderr,"SBB gethostbyname(...) failed errno: %d exiting...\n", errno);
     exit(1);
     }
     sock_addr.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
     #endif
     */
    
    sock_addr.sin_port = htons(PORT); //4321
    
    /*
     * bind the socket to the port number
     */
    bind(sd, (struct sockaddr *) &sock_addr, sizeof(sock_addr)); //{
    //    perror("bind");
        //exit(1);
    //}
    
    /*
     * advertise we are available on this socket/port
     */
    if (listen(sd, 5) == -1) {
        perror("listen");
        exit(1);
    }
    
    /*
     * wait for a client to connect
     */
    struct sockaddr_in	sock_addr_from_client;
    socklen_t addrlen = sizeof(sock_addr_from_client);
    int sd_current;
    if ((sd_current = accept(sd, (struct sockaddr *)  &sock_addr_from_client, &addrlen)) == -1) {
        fprintf(stderr,"SBB accept(...) failed errno: %d  exiting...\n", errno);
        exit(1);
    }
    printf("SBB client ip address: %s port: %x\n",
           inet_ntoa(sock_addr_from_client.sin_addr),
           PORT);
    
    //		ntohs(sock_addr_from_client.sin_port));
    
    /*
     * block on socket waiting for client message
     */
    double ret = 0.0;
    char	msg[MSGSIZE];
    fprintf(stderr," SBB: sizeof msg: %lu\n", sizeof(msg));
    memset(&msg[0], 0, sizeof(msg));
    
    while ( ( ret = recv(sd_current, msg, sizeof(msg), 0) ) > 0 ) {
        
        printf("The server is running...\n");
        
        //get message from client
        printf("SBB server received msg: [%s] from client\n", msg);
        
        
        //respond to message
        if(strcmp(msg, "get trading book data") == 0){
           
            //do all calculations here
            YieldCurve yield_curve = getNewYieldCurve();
            processTradingBooks(yield_curve);
            calculateIntradayChanges();
            
            //convert bond data to string
            string buffer = bondToString(openTB_bonds);
            buffer.append("%");
            string buffer1 = bondToString(closeTB_bonds);
            buffer1.append("%");
            string buffer2 = changeBondToString(change_bonds);
            buffer2.append("%");
            buffer2.append("~");
            
            //concatenate strings
            string buffer3 = buffer.append(buffer1).append(buffer2);
            
            //const char* buffer2 = "asdasdas~";
            
            if (send(sd_current, buffer3.c_str(), strlen(buffer3.c_str()), 0) == -1){
                fprintf(stderr,"SBB send(...) failed errno: %d exiting...\n", errno);
                exit(1);
            }
            
            printf("Done with first instruction");
        }
        
        
        if (strcmp(msg, "get var") == 0){
            stringstream ss;
            
            ss << "Spread VaR" << "/" << spread_var_open << "/" << spread_var_close << "/" << spread_var_close - spread_var_open << "/" << "@" << "Yield VaR" << "/" << yield_var_open << "/" << yield_var_close << "/" << yield_var_close - yield_var_open << "/" << "@" << "Total VaR" << "/" << var_open << "/" << var_close << "/" << var_close - var_open << "/" << "@" << "~";
            
            string buffer = ss.str();
            
            if (send(sd_current, buffer.c_str(), strlen(buffer.c_str()), 0) == -1){
                fprintf(stderr,"SBB send(...) failed errno: %d exiting...\n", errno);
                exit(1);
            }
            
        }
        
        if (strcmp(msg, "get time") == 0){
            
            stringstream ss;
            
            ss << "Server Side" << "/" << real_time << "/" << user_time << "/" << system_time << "/" << "~";
            
            string buffer = ss.str();
            
            if (send(sd_current, buffer.c_str(), strlen(buffer.c_str()), 0) == -1){
                fprintf(stderr,"SBB send(...) failed errno: %d exiting...\n", errno);
                exit(1);
            }
            
        }
        
        if ( strcmp(msg, "get risk") == 0){
            
            stringstream ss;
            
            ss << "Closing Book" << "/" << two_year_risk << "/" << two_year_mv << "/" << five_year_risk << "/" << five_year_mv << "/" << ten_year_risk << "/" << ten_year_mv << "/" << thirty_year_risk << "/" << thirty_year_mv << "/" << "@";
            
            //include the hedge information
            stringstream ss1;
            
            ss1 << "2 Year Hedge" << "/" << "" << "/" << two_year_hedge << "/" << "" << "/" << five_year_hedge << "/" << "" << "/" << ten_year_hedge << "/" << "" << "/" << thirty_year_hedge << "/" << "~";
            
            string temp = ss1.str();
            
            string buffer = ss.str();
            buffer.append(temp);
            
            if (send(sd_current, buffer.c_str(), strlen(buffer.c_str()), 0) == -1){
                fprintf(stderr,"SBB send(...) failed errno: %d exiting...\n", errno);
                exit(1);
            }
            
        }
        
        if ( strcmp( msg, "get yield") == 0){
           
            if (send(sd_current, yield_data_string.append("~").c_str(), strlen(yield_data_string.c_str()), 0) == -1){
                fprintf(stderr,"SBB send(...) failed errno: %d exiting...\n", errno);
                exit(1);
            }
            
        }
    
        
        if (strcmp(msg, "up") == 0){
            
            YieldCurve newYield = getModifiedYieldCurve( 1.5 );
            processTradingBooks(newYield);
            
            string buffer = getAllDataString();
            
            if (send(sd_current, buffer.c_str(), strlen(buffer.c_str()), 0) == -1){
                fprintf(stderr,"SBB send(...) failed errno: %d exiting...\n", errno);
                exit(1);
            }
            
        }
        
        if (strcmp(msg, "down") == 0){
            
            YieldCurve newYield = getModifiedYieldCurve( .5 );
            processTradingBooks(newYield);
            
            string buffer = getAllDataString();
            
            if (send(sd_current, buffer.c_str(), strlen(buffer.c_str()), 0) == -1){
                fprintf(stderr,"SBB send(...) failed errno: %d exiting...\n", errno);
                exit(1);
            }
            
        }
        
        if (strcmp(msg, "default") == 0){
            
            YieldCurve newYield = getNewYieldCurve();
            processTradingBooks(newYield);
            
            string buffer = getAllDataString();
            
            if (send(sd_current, buffer.c_str(), strlen(buffer.c_str()), 0) == -1){
                fprintf(stderr,"SBB send(...) failed errno: %d exiting...\n", errno);
                exit(1);
            }
            
        }
        
        if (strcmp(msg, "calculate") == 0){
            
            char buff[MSGSIZE];
            recv(sd_current, buff, sizeof(buff), 0);
            
            const string string_to_split(buff);
            
            //split string
            std::regex rgx("/");
            std::sregex_token_iterator iter(string_to_split.begin(),
                                            string_to_split.end(),
                                            rgx,
                                            -1);
            std::sregex_token_iterator end;
            
            vector<double> tokens;
            
            for ( ; iter != end; ++iter)
                tokens.push_back( stod(*iter) );
            
            
            main_yield_curve.shift_curve( tokens );
            
            processTradingBooks( main_yield_curve );
            
            string buffer = getAllDataString();
            
            if (send(sd_current, buffer.c_str(), strlen(buffer.c_str()), 0) == -1){
                fprintf(stderr,"SBB send(...) failed errno: %d exiting...\n", errno);
                exit(1);
            }
            
        }

        
        if (strcmp(msg, "histogram") == 0){
    
            stringstream vv;
            
            for(int i=0;i<vars.size();i++){
                
                    std::ostringstream strs;
                    
                    strs << vars[i];
                    
                    std::string var = strs.str();
                    
                    vv << var << "/";
                
            }
            
            vv << "~";
            
            string buff = vv.str();
            
            if (send(sd_current, buff.c_str(), strlen(buff.c_str()), 0) == -1){
                
                fprintf(stderr,"SBB send(...) failed errno: %d exiting...\n", errno);
                
                exit(1);
                
            }
            
        }


        memset(&msg[0], 0, sizeof(msg));
    }
    
    if( 0 == ret ) {
        printf("SBB client exited...\n");
        /* For TCP sockets
         * the return value 0 means the peer has closed its half side of the connection
         */
    }
    else if( -1 == ret ) {
        fprintf(stderr,"SBB recv(...) returned failed - errno: %d exiting...\n", errno);
        exit(1);
    }
    
    close(sd_current);
    close(sd);
}

void
processTradingBooks(YieldCurve yield_curve){
    
    
    SBB_instrument_input_file opening_data("tradingbook_opening.txt");
    SBB_instrument_input_file closing_data("tradingbook_closing.txt");
    
    int line_count = 0;
    int line_count2 = 0;
    int line_count3 = (int) yield_curve.get_yield_rates().size();
    
    SBB_instrument_fields *opening_data_ptr;
    SBB_instrument_fields *closing_data_ptr;
    
    opening_data_ptr = opening_data.records(line_count);
    closing_data_ptr = closing_data.records(line_count2);
    
    TradingBook openTB(opening_data_ptr, line_count);
    TradingBook closeTB(closing_data_ptr, line_count2);
    
    //process opening and closing books
    openTB.process_bonds(yield_curve);
    closeTB.process_bonds(yield_curve);
    
    //calculate VaR
    TradingBook yield_curve2(yield_curve.get_benchmark_data(), line_count3);
    
    yield_curve2.process_bonds(yield_curve);
    
    VarCalculator var_calc_open = VarCalculator(openTB, yield_curve2);
    VarCalculator var_calc_close = VarCalculator(closeTB, yield_curve2);
  
    var_calc_open.process_historical_data();
    var_calc_close.process_historical_data();
    
    
    vars = var_calc_close.book_pnl_vector;
    
    //Total VaR for both opening and closing books
    var_open = var_calc_open.get_var(99);
    var_close = var_calc_close.get_var(99);
    
    //spread VaR for both opening and closing books
    spread_var_open = var_calc_open.get_spread_var(99);
    spread_var_close = var_calc_close.get_spread_var(99);
    
    //caclculate the yield VaR
    yield_var_open = var_open - spread_var_open;
    yield_var_close = var_close - spread_var_close;
    
    //set the bond vectors
    openTB_bonds = openTB.get_bond_collection();
    closeTB_bonds = closeTB.get_bond_collection();
    
    //get the risk information
    two_year_risk = calculate_bucket_risk(closeTB.two_yr_bucket);
    five_year_risk = calculate_bucket_risk(closeTB.five_yr_bucket);
    ten_year_risk = calculate_bucket_risk(closeTB.ten_yr_bucket);
    thirty_year_risk = calculate_bucket_risk(closeTB.thirty_yr_bucket);
    
    //get the market value information
    two_year_mv = calculate_bucket_mv(closeTB.two_yr_bucket);
    five_year_mv = calculate_bucket_mv(closeTB.five_yr_bucket);
    ten_year_mv = calculate_bucket_mv(closeTB.ten_yr_bucket);
    thirty_year_mv = calculate_bucket_mv(closeTB.thirty_yr_bucket);
    
    //get the two year hedge information
    double dv01_mult = yield_curve.get_dv01_2yr() *-1;
    
    /*
    double two_year_amt = get_bucket_amount(closeTB.two_yr_bucket);
    double five_year_amt = get_bucket_amount(closeTB.five_yr_bucket);
    double ten_year_amt = get_bucket_amount(closeTB.ten_yr_bucket);
    double thirty_year_amt = get_bucket_amount(closeTB.thirty_yr_bucket);
    
    int two_year_dv01 = get_bucket_dv01(closeTB.two_yr_bucket);
    double five_year_dv01 = get_bucket_dv01(closeTB.five_yr_bucket);
    double ten_year_dv01 = get_bucket_dv01(closeTB.ten_yr_bucket);
    double thirty_year_dv01 = get_bucket_dv01(closeTB.thirty_yr_bucket);
    
    two_year_hedge = two_year_amt * two_year_dv01 / dv01_mult;
    five_year_hedge = five_year_amt * five_year_dv01 / dv01_mult;
    ten_year_hedge = ten_year_amt * ten_year_dv01 / dv01_mult;
    thirty_year_hedge = thirty_year_amt * thirty_year_dv01 / dv01_mult;
     */
    
    two_year_hedge = two_year_risk / dv01_mult;
    five_year_hedge = five_year_risk / dv01_mult;
    ten_year_hedge = ten_year_risk / dv01_mult;
    thirty_year_hedge = thirty_year_risk / dv01_mult;
    
    //get the yield information
    stringstream ss;
    
    ss << "Yield Curve" << "/" << yield_curve.yield_rates[1] << "/" <<  yield_curve.yield_rates[2] << "/" << yield_curve.yield_rates[3] << "/" << yield_curve.yield_rates[0] << "/" << "~";
    
    yield_data_string = ss.str();
    
    
}

double get_bucket_dv01(vector<Bond> bonds){
    
    double total_dv01 = 0.0;
    for(int i = 0; i < bonds.size(); i++){
        total_dv01 += bonds.at(i).get_current_dv01();
    }
    return total_dv01;
}

int get_bucket_amount(vector<Bond> bonds){
    
    int total_amount = 0;
    for(int i = 0; i < bonds.size(); i++){
        SBB_instrument_fields* bond_ptr;
        bond_ptr = bonds.at(i).get_bond_ptr();
        total_amount += bond_ptr->Amount();
    }
    
    return total_amount;
}


YieldCurve
getModifiedYieldCurve( double yield_shift ){
    
    //add the yield shift
    for(int i = 0; i < main_yield_curve.yield_rates.size(); i++){
        main_yield_curve.yield_rates[i] *= yield_shift;
        //double current_yield = yield_data_ptr->Yield();
        //yield_data_ptr->Yield(current_yield + yield_shift);
    }

    return main_yield_curve;
}

YieldCurve
getNewYieldCurve(){
    
    SBB_instrument_input_file yield_data("yieldcurve.txt");
    
    int line_count = 0;
    
    SBB_instrument_fields *yield_data_ptr;
    
    yield_data_ptr = yield_data.records(line_count);
    
    YieldCurve yield_curve(yield_data_ptr, line_count);
    
    //process spread data
    yield_curve.process_spread_data();
    
    main_yield_curve = yield_curve;
    
    return yield_curve;
    
}

string
getAllDataString(){
    
    string buffer = bondToString(openTB_bonds);
    buffer.append("%");
    string buffer1 = bondToString(closeTB_bonds);
    buffer1.append("%");
    string buffer2 = changeBondToString(change_bonds);
    buffer2.append("%");
    
    //concatenate strings
    string buffer3 = buffer.append(buffer1).append(buffer2).append(">>>");
    
    //var data
    stringstream ss;
    
    ss << "Spread VaR" << "/" << spread_var_open << "/" << spread_var_close << "/" << spread_var_close - spread_var_open << "/" << "@" << "Yield VaR" << "/" << yield_var_open << "/" << yield_var_close << "/" << yield_var_close - yield_var_open << "/" << "@" << "Total VaR" << "/" << var_open << "/" << var_close << "/" << var_close - var_open << "/" << ">>>";
    
    string varString = ss.str();
    
    //time data
    stringstream ss1;
    
    ss1 << "Server Side" << "/" << real_time << "/" << user_time << "/" << system_time << "/" << ">>>";
    
    string timeString = ss1.str();
    
    //risk data
    stringstream ss2;
    
    ss2 << "Closing Book" << "/" << two_year_risk << "/" << two_year_mv << "/" << five_year_risk << "/" << five_year_mv << "/" << ten_year_risk << "/" << ten_year_mv << "/" << thirty_year_risk << "/" << thirty_year_mv << "/" << "@" ;
    
    //include the hedge information
    stringstream ss3;
    
    ss3 << "2 Year Hedge" << "/" << "" << "/" << two_year_hedge << "/" << "" << "/" << five_year_hedge << "/" << "" << "/" << ten_year_hedge << "/" << "" << "/" << thirty_year_hedge << "/" << ">>>";
    
    string temp = ss3.str();
    
    string riskString = ss2.str();
    riskString.append(temp);
    
    //concatenate all string
    string final = buffer3.append(varString).append(timeString).append(riskString).append(yield_data_string);
    
    return final;
}

void
calculateIntradayChanges(){
    
    for(int i = 0; i < openTB_bonds.size(); i++){
        
        //calculate the change in amount
        SBB_instrument_fields* ptr1 = openTB_bonds.at(i).get_bond_ptr();
        SBB_instrument_fields* ptr2 = closeTB_bonds.at(i).get_bond_ptr();
        int amount_open = ptr1->Amount();
        int amount_close = ptr2->Amount();
        int amount_change = amount_close - amount_open;
        
        //caculate the change in risk
        double risk_open = openTB_bonds.at(i).get_current_risk();
        double risk_close = closeTB_bonds.at(i).get_current_risk();
        double risk_change = risk_close - risk_open;
        
        //calculate the change in lgd
        double lgd_open = openTB_bonds.at(i).get_lgd();
        double lgd_close = closeTB_bonds.at(i).get_lgd();
        double lgd_change = lgd_close - lgd_open;
        
        //create new change bond and push it to vector
        ChangeBond cBond(amount_change, risk_change, lgd_change, ptr1->Ticker(), ptr1->Quality());
        change_bonds.push_back(cBond);
        
    }
    
}

double
calculate_bucket_risk(vector<Bond> bonds){
    
    double total_risk = 0;
    for(int i = 0; i < bonds.size(); i++){
        total_risk += bonds[i].get_current_risk();
    }
    
    return total_risk;
}

double
calculate_bucket_mv(vector<Bond> bonds){
    
    double total_mv = 0;
    for(int i = 0; i < bonds.size(); i++){
        total_mv += bonds[i].get_current_market_value();
    }
    
    return total_mv * 1000;
}


string
bondToString(vector<Bond> bond_collection){
    
    stringstream ss;
    
    for(int i = 0; i < bond_collection.size(); i++){
        
        Bond current = bond_collection.at(i);
        SBB_instrument_fields *ptr = current.get_bond_ptr();
        
        ss << ptr->Ticker() << "#" << ptr->Quality() << "#" << ptr->Amount() << "#" << current.get_current_risk() << "#" << current.get_lgd() << "/";
    }
    const std::string tmp = ss.str();
    return tmp;
}

string
changeBondToString(vector<ChangeBond> bond_collection){
    
    stringstream ss;
    
    for(int i = 0; i < bond_collection.size(); i++){
        
        ChangeBond current = bond_collection.at(i);
        
        ss << current.get_ticker() << "#" <<  current.get_quality() << "#" << current.get_amount() << "#" << current.get_risk() << "#" << current.get_lgd() << "/";
    }
    const std::string tmp = ss.str();
    return tmp;
}


