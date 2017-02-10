#ifndef SBB_IO_H
#define SBB_IO_H

#include <stdio.h>
#include <stdexcept>

#include <string.h>

//
// these are the string values used in the data files!
//
#define YIELD_STRING "YIELD"
#define SPREAD_STRING "SPREAD"
#define BMARK_STRING_T2 "T2"
#define BMARK_STRING_T5 "T5"
#define BMARK_STRING_T10 "T10"
#define BMARK_STRING_T30 "T30"

//
// lengths of strings in data files and derived from data files
//
#define SBB_MAX_FIELD_LENGTH		32
#define SBB_INSTRUMENT_TCM_LENGTH	64

#define SBB_LINE_BUFFER_LENGTH		256

// wide open class for loading data per instrument
class SBB_instrument_fields {
    
public:
    SBB_instrument_fields(){
        _SecurityID[0] = 0;
        _Ticker[0] = 0;
        _SettlementDate = 0;
        _Coupon = 0.0;
        _MaturityDate = 0;
        _Frequency = 0;
        _ValType[0] = 0;
        _Yield = 0.0;
        _Spread = 0.0;
        _Quality[0] = 0;
        _Amount = 0;
        _TCM[0] = 0;
        _Line_buffer[0] = 0;
    }
    ~SBB_instrument_fields() {};
    
    const		char* SecurityID() const { return _SecurityID; }
    const		char* Ticker() const { return _Ticker; }
    int		SettlementDate() const { return _SettlementDate; }
    double		Coupon() const { return _Coupon; }
    int		MaturityDate() const { return _MaturityDate; }
    short		Frequency() const { return _Frequency; }
    const char*	ValType() const { return _ValType; }
    double		Yield() const { return _Yield; }
    double		Spread() const { return _Spread; }
    const char* 	Quality() const { return _Quality; }
    int		Amount() const { return _Amount; }
    const char* 	TCM() const { return _TCM; }
    const char* 	Line_buffer() const { return _Line_buffer; }
    
    //
    // keep yield and spread separate data items:
    // if spread is loaded, I'll calculated yield on a second pass after combining with the yield curve
    // - this I'll be writing back into this object AFTER the initial laod...
    //
    
    //
    // the values in the file are checked here!
    //
    bool 		is_spread_priced() const { return !strcmp(ValType(),SPREAD_STRING); }
    bool 		is_yield_priced() { return !strcmp(ValType(),YIELD_STRING); }
    
    void SecurityID(const char* SecurityID) { strcpy(_SecurityID,SecurityID); }
    void Ticker(const char* Ticker) { strcpy(_Ticker,Ticker); }
    void SettlementDate(int SettlementDate) { _SettlementDate = SettlementDate; }
    void Coupon(double Coupon) { _Coupon = Coupon; }
    void MaturityDate(int MaturityDate) { _MaturityDate = MaturityDate; }
    void Frequency(short Frequency) { _Frequency = Frequency; }
    void ValType(const char* ValType) { strcpy(_ValType, ValType); }
    void Yield(double Yield) { _Yield = Yield; }
    void Spread(double Spread) { _Spread = Spread; }
    void Quality(const char* Quality) { strcpy(_Quality, Quality); }
    void Amount(int Amount) { _Amount = Amount; }
    
    void set_TCM(){
        sprintf(_TCM,"%s %.2f %d",
                _Ticker,
                _Coupon,
                _MaturityDate);
    }
    
    void 	Line_buffer(const char* buf) { strcpy(_Line_buffer, buf); }
    
    void show() const {
        printf("\tBond Static - ID: \"%s\" TCM: \"%s\" Ticker (issuer): \"%s\" \n\t\tSettleDate: %d\n\t\tCoupon: %.2f \n\t\tMatDate: %d \n\t\tFrequency: %d \n\t\tValType: %s Yield: %.3f Spread: %.3f\n\t\tQuality: \"%s\" \n\t\tAmount: %d\n",
               _SecurityID,
               _TCM,
               _Ticker,
               _SettlementDate,
               _Coupon,
               _MaturityDate,
               _Frequency,
               _ValType,
               _Yield,
               _Spread,
               _Quality,
               _Amount);
    }
    
    
private:
    char	_SecurityID[SBB_MAX_FIELD_LENGTH];	// cusip (made up)
    char	_Ticker[SBB_MAX_FIELD_LENGTH];	// ticker "e.g., "GM"
    int	_SettlementDate;
    double	_Coupon;
    int	_MaturityDate;
    short	_Frequency;
    char	_ValType[SBB_MAX_FIELD_LENGTH];
    double	_Yield;
    double	_Spread;
    char	_Quality[SBB_MAX_FIELD_LENGTH];
    
    int	_Amount; 	// in 000's
    
    //
    // add this field for convenience - concatenation of fields in data file...
    // "ticker coupon maturity"
    //
    char _TCM[SBB_INSTRUMENT_TCM_LENGTH];
    
    //
    // save the line buffer as read in  so I can write out results at end of each input line (I asked students to concat price, dv01, risk, LGD on each line)
    //
    char _Line_buffer[SBB_LINE_BUFFER_LENGTH+1];
};

//
// simple class for interfacing with data file that contains instrument desriptive data
//
class SBB_instrument_input_file {
    
    // Resource (file handle) acquisition is initialization version - "RAII"
    // destructor closes file...
public:
    SBB_instrument_input_file(const char* filename);
    ~SBB_instrument_input_file();
    
    //
    // this reads entire file to first get the total line counts so I can new an array all at once
    //
    //int record_count();
    
    //
    // returns a heap allocated array of file contents
    //
    SBB_instrument_fields* records(int& length);
    
    //
    // As a convenience to the caller memory is allocated inside this class
    // Call this to free it
    //
    void free_records();
    
private:
    FILE* _file;
    SBB_instrument_fields *_fields_array;
    
    //
    // prevent copying
    //
    SBB_instrument_input_file (const SBB_instrument_input_file &);
    
    //
    // prevent assignment
    //
    SBB_instrument_input_file & operator= (const SBB_instrument_input_file &);
    
    //
    // disable the default constructor
    // force construction with filename param only...
    //
    SBB_instrument_input_file();
};


class SBB_yield_curve_input_file {
public:
    SBB_yield_curve_input_file(const char* filename);
    ~SBB_yield_curve_input_file();
    
};


//
// simple class to open a file rw and write lines
// kept separate from instrument file class to gaurd against accidently opening rw and deleting contents
//
class SBB_output_file {
public:
    SBB_output_file(const char* filename);
    ~SBB_output_file();
    
    int write_line(const char* output_line);
    
private:
    FILE* _file;
};

class SBB_var_day_record {
    
public:
    SBB_var_day_record() {
        _date=0;
        _valType[0]='\0';
        _valValue=0;
        _benchmarkID[0]='\0';
    }
    ~SBB_var_day_record() {};
    
    int Date() { return _date; }
    char* ValType() { return _valType; }
    double ValValue() { return _valValue; }
    char* BenchmarkID() { return _benchmarkID; }
    
    void Date(int date) { _date = date; }
    void ValType(char* val) { strcpy(_valType, val); }
    void ValValue(double val) { _valValue = val; }
    void BenchmarkID(char* val) { strcpy(_benchmarkID, val); }
    
    void show() {
        printf("date: %d type: %s value: %.3f bmark: %s",
               _date,
               _valType,
               _valValue,
               _benchmarkID);
        
    }
    
    bool is_spread_priced() { return !strcmp(ValType(),SPREAD_STRING); }
    bool is_yield_priced() { return !strcmp(ValType(),YIELD_STRING); }
    
private:
    // 20101101 YIELD 3.0 .012 NULL
    
    int	_date;	// format: 20091225
    char	_valType[SBB_MAX_FIELD_LENGTH];
    double	_valValue;
    char	_benchmarkID[SBB_MAX_FIELD_LENGTH];	// use to lookup in other histdata file for OTR's
};

class SBB_var_input_file {
    //
    // operations on the file data assume that the file is ordered - first day info row is oldest day, last row is yesterday!
    // the very first row is the security ID!
    //
public:
    SBB_var_input_file() {
        _file = 0;
        _num_records = 0;
        _fields_array = 0;
        _security_id[0] = 0;
    }
    
    void open(const char* filename);
    
    ~SBB_var_input_file();
    
    
    //
    // returns a heap allocated array of file contents
    //
    SBB_var_day_record* records(int& length);
    void show();
    
    void free_records();
    
    // this field is loaded for this object and identifies the security - the file name is also securityID.txt 
    // but rather than use the file name to parse for name, include in contents...
    char* SecurityID() { return _security_id; }
    
    //static int Get_non_comment_row_count(const char * filename);
    
private:
    char _security_id[SBB_MAX_FIELD_LENGTH];
    FILE*		_file;
    int			_num_records;
    SBB_var_day_record*	_fields_array;
};

#endif
