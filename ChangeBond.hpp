//
//  ChangeBond.hpp
//  Homework 9
//
//  Created by  Satya Nedunuri on 12/10/15.
//  Copyright © 2015  Satya Nedunuri. All rights reserved.
//

#ifndef ChangeBond_hpp
#define ChangeBond_hpp

#include <stdio.h>

class ChangeBond {
    
public:
    ChangeBond(int amount, double risk, double lgd, const char* ticker, const char* quality){
        this->amount = amount;
        this->risk = risk;
        this->lgd = lgd;
        this->ticker = ticker;
        this->quality = quality;
    }
    
    int get_amount() { return amount; }
    double get_risk() { return risk; }
    double get_lgd() { return lgd; }
    const char* get_ticker() { return ticker; }
    const char* get_quality() { return quality; }
    
    
private:
    int amount;
    double risk;
    double lgd;
    const char* ticker;
    const char* quality;
    
};

#endif /* ChangeBond_hpp */
