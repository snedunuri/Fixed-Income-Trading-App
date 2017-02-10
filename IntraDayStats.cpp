//
//  IntraDayStats.cpp
//  Homework 7
//
//  Created by  Satya Nedunuri on 11/22/15.
//  Copyright © 2015  Satya Nedunuri. All rights reserved.
//

#include "IntraDayStats.hpp"

double
IntraDayStats::total_position_change(){
    double opening_position = opening_book.get_total_position();
    double closing_position = closing_book.get_total_position();
    return closing_position - opening_position;
}

double
IntraDayStats::total_LGD_change(){
    double opening_LGD = opening_book.get_total_LGD();
    double closing_LGD = closing_book.get_total_LGD();
    return closing_LGD - opening_LGD;
}