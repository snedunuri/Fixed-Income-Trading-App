//
//  IntraDayStats.hpp
//  Homework 7
//
//  Created by  Satya Nedunuri on 11/22/15.
//  Copyright © 2015  Satya Nedunuri. All rights reserved.
//

#ifndef IntraDayStats_hpp
#define IntraDayStats_hpp

#include <stdio.h>
#include "TradingBook.hpp"

class IntraDayStats{
    
public:
    
    IntraDayStats(TradingBook opening_book, TradingBook closing_book):opening_book(opening_book), closing_book(closing_book){
    }
    
    ~IntraDayStats(){}
    
    double total_position_change();
    
    double total_LGD_change();
    
private:
    TradingBook opening_book;
    TradingBook closing_book;
};

#endif /* IntraDayStats_hpp */
