server:
	g++ main.cpp SBB_util.cc SBB_date.cc SBB_io.cc SBB_ratings.cc TradingBook.cpp YieldCurve.cpp Bond.cpp SBB_util.h SBB_date.h SBB_io.h SBB_ratings.h TradingBook.hpp Bond.hpp YieldCurve.hpp VarCalculator.cpp VarCalculator.hpp IntraDayStats.cpp IntraDayStats.hpp ChangeBond.hpp

client:
	g++ sbb_client.c -o client.o
	
clean:
	rm *.o
	rm *.gch