#include "SBB_io.h"
#include "SBB_util.h"
#include <stdlib.h>
#include <errno.h>


#define SBB_LINE_BUFFER_LENGTH		256
static char Line_buffer[SBB_LINE_BUFFER_LENGTH+1];

char*
runtime_error(char* msg)
{
    printf(msg);
    exit(1);
}

static int
get_row_count_minus_comments(int line_length, char line_buffer[], FILE* fp)
{
    // not including comments (first char is #) that is...
    // Comments interspersed in data lines is allowed
    int line_count_minus_comments = 0;
    int comment_count = 0;
    while(fgets(line_buffer,line_length,fp)){
        if('#' == line_buffer[0]) {
            comment_count++;
            continue; // skip through comments
        }
        ++line_count_minus_comments;
    }
    //printf("hit %d comment lines in file\n", comment_count);
    
    rewind(fp);
    //fseek(fp, 0L, SEEK_SET);
    
    return line_count_minus_comments;
}

int
get_row_count_minus_comments(char* filename)
{
    // FILE* file = std::fopen(filename, "r"); // open read-only
    FILE* file = fopen(filename, "r"); // open read-only
    if (!file){
        fprintf(stderr,"ERROR couldn't open: %s\n", filename);
        //throw std::runtime_error("file open failure");
        throw runtime_error("file open failure");
    }
    
    int total_length = get_row_count_minus_comments(SBB_LINE_BUFFER_LENGTH, Line_buffer, file);
    
    //if (std::fclose(file)) {
    if (fclose(file)) {
        fprintf(stderr,"flose failed on file errno: %d\n", errno);
    }
    return total_length;
}

SBB_instrument_input_file::SBB_instrument_input_file(const char * filename)
{
    _file = fopen(filename, "r"); // open read-only
    //_file = std::fopen(filename, "r"); // open read-only
    if (!_file){
        fprintf(stderr,"ERROR couldn't open: %s\n", filename);
        //throw std::runtime_error("file open failure");
        throw runtime_error("file open failure");
    }
}

SBB_instrument_input_file::~SBB_instrument_input_file()
{
    //if (std::fclose(_file)) {
    if (fclose(_file)) {
        fprintf(stderr,"fclose failed on file errno: %d\n", errno);
    }
}

void
SBB_instrument_input_file::free_records()
{
    delete[] _fields_array;
}

SBB_instrument_fields*
SBB_instrument_input_file::records(int& length)
{
    length = get_row_count_minus_comments(SBB_LINE_BUFFER_LENGTH, Line_buffer, _file);
    printf("FZ: row count of large file: %d\n", length);
    
    _fields_array = new SBB_instrument_fields[length];
    
    int non_comments_line_count_read = 0;
    
    //while(std::fgets(Line_buffer,SBB_LINE_BUFFER_LENGTH,_file)){
    while(fgets(Line_buffer,SBB_LINE_BUFFER_LENGTH,_file)){
        
        if('#' == Line_buffer[0]) {
            continue; // skip through comments
        }
        
        SBB_instrument_fields* record_ptr = &_fields_array[non_comments_line_count_read];
        
        // make a copy of the line for later write out to make sure it's identical to what I read in :)
        char tmp_line[256];
        strcpy(tmp_line,Line_buffer);
        tmp_line[strlen(Line_buffer)-1] = 0;
        record_ptr->Line_buffer(tmp_line);
        
        //
        // fields per line:
        // if data file schema changes update this comment!
        //
        // SecurityID Ticker SettlementDate Coupon MaturityDate Frequency ValType Yield Quality Amount
        //
        
        //
        // SecurityID
        //
        char* token = strtok(Line_buffer," ");
        if(NULL == token) {
            fprintf(stderr,"line parsing failed on buf: %s\n", Line_buffer);
            exit(1);
        }
        //printf("SecurityID: %s\n", token);
        record_ptr->SecurityID(token);
        
        
         //
         // Ticker
         //
         token = strtok(NULL, " ");
         if(NULL == token) {
         fprintf(stderr,"line parsing failed on buf: %s\n", Line_buffer);
         exit(1);
         }
         //printf("Ticker: %s\n", token);
         record_ptr->Ticker(token);
         
        
        //
        // SettlementDate
        //
        token = strtok(NULL," ");
        if(NULL == token) {
            fprintf(stderr,"line parsing failed on buf: %s\n", Line_buffer);
            exit(1);
        }
        //printf("SettlementDate: %s\n", token);
        record_ptr->SettlementDate(atoi(token));
        
        //
        // Coupon
        //
        token = strtok(NULL," ");
        if(NULL == token ) {
            fprintf(stderr,"line parsing failed on buf: %s\n", Line_buffer);
            exit(1);
        }
        //printf("Coupon: %s\n", token);
        record_ptr->Coupon(atof(token));
        
        //
        // MaturityDate
        //
        token = strtok(NULL," ");
        //printf("MaturityDate: %s\n", token);
        record_ptr->MaturityDate(atoi(token));
        
        //
        // Frequency
        //
        token = strtok(NULL," ");
        //printf("Frequency: %s\n", token);
        record_ptr->Frequency(atoi(token));
        
#define VVV_SPREAD
#ifdef VVV_SPREAD
        
        
        //
        // ValType - SPREAD or YIELD
        // (add price later for HY bonds - then will need to add root finder)
        token = strtok(NULL," ");
        //printf("ValType: %s\n", token);
        record_ptr->ValType(token);
        
        
        //
        // Yield or Spread (let other stay 0 if not set)
        //
        token = strtok(NULL," ");
        if(NULL == token ) {
            fprintf(stderr,"line parsing failed on buf: %s\n", Line_buffer);
            exit(1);
        }
        
        if(record_ptr->is_spread_priced()){
            //printf("VVV Spread is: %s\n", token);
            record_ptr->Spread(atof(token));
        }
        else if(record_ptr->is_yield_priced()){
            //printf("VVV Yield is:  %s\n", token);
            record_ptr->Yield(atof(token));
        }
        else
        {
            //SBB_DEBUG();
            fprintf(stderr,"unsupported ValType! %s\n", token);
        }
        
#else
        token = strtok(NULL," ");
        //printf("Yield: %s\n", token);
        record_ptr->Yield(atof(token));
#endif
        
        //
        // Quallty
        //
        token = strtok(NULL," ");
        if(NULL == token ) {
            fprintf(stderr,"line parsing failed on buf: %s\n", Line_buffer);
            exit(1);
        }
        //printf("Quality code is:  %s\n", token);
        record_ptr->Quality(token);
        
        //
        // Amount
        //
        token = strtok(NULL," ");
        if(NULL == token ) {
            fprintf(stderr,"line parsing failed on buf: %s\n", Line_buffer);
            exit(1);
        }
        //printf("Amount is:  %s\n", token);
        record_ptr->Amount(atoi(token));
        
        
        //
        // store ticker coupon maturity for later
        //
        record_ptr->set_TCM();
        
        
        non_comments_line_count_read++;
    }
    
    printf("FZ lines read: %d \n", non_comments_line_count_read);
    
    return _fields_array;
}


 SBB_output_file::SBB_output_file(const char * filename)
 {
	//_file = std::fopen(filename, "w"); // open write-only
	_file = fopen(filename, "w"); // open write-only
	if (!_file){
 fprintf(stderr,"ERROR %s couldn't open: %s\n", __PRETTY_FUNCTION__, filename);
 //throw std::runtime_error("file open failure");
 throw runtime_error("file open failure");
	}
 }
 
 SBB_output_file::~SBB_output_file()
 {
	//if (std::fclose(_file)) {
	if (fclose(_file)) {
 fprintf(stderr,"ERROR %s flose failed on file %s errno: %d\n",
 __PRETTY_FUNCTION__,
 (char*)_file,
 errno);
	}
 }
 
 int
 SBB_output_file::write_line(const char* output_line)
 {
	//int ret_code = std::fputs(output_line, _file);
	int ret_code = fputs(output_line, _file);
	if( EOF == ret_code ) {
 fprintf(stderr,"%s failed - returned EOF: errno: %d\n",
 __PRETTY_FUNCTION__,
 errno);
	}
	// returns non-negative integer on successful completion (old way was 0)
	return ret_code;
 };
 
 
 
 void
 SBB_var_input_file::open(const char* filename)
 {
	//_file = std::fopen(filename, "r"); // open read-only
	_file = fopen(filename, "r"); // open read-only
	if (!_file){
 fprintf(stderr,"ERROR %s couldn't open: %s\n",
 __PRETTY_FUNCTION__,
 filename);
 //throw std::runtime_error("file open failure");
 throw runtime_error("file open failure");
	}
 }
 
 SBB_var_input_file::~SBB_var_input_file()
 {
	//if (std::fclose(_file)) {
	if (fclose(_file)) {
 fprintf(stderr,"ERROR flose %s failed on file %s errno: %d\n",
 __PRETTY_FUNCTION__,
 (char*) _file,
 errno);
	}
 }
 
 
 SBB_var_day_record*
 SBB_var_input_file::records(int& num_records)
 {
	int total_length = get_row_count_minus_comments(SBB_LINE_BUFFER_LENGTH, Line_buffer, _file);
 
	_fields_array = new SBB_var_day_record[total_length];
 
	_num_records = 0;
	int first_non_comment_line = 1;
	//while(std::fgets(Line_buffer,SBB_LINE_BUFFER_LENGTH,_file)){
	while(fgets(Line_buffer,SBB_LINE_BUFFER_LENGTH,_file)){
 
 if('#' == Line_buffer[0]) {
 continue; // skip through comments
 }
 
 SBB_var_day_record* record_ptr = &_fields_array[_num_records];
 
 //
 // Date
 //
 char* token = strtok(Line_buffer," ");
 if(NULL == token) {
 fprintf(stderr,"line parsing failed on buf: %s\n", Line_buffer);
 exit(1);
 }
 //printf("Date: %s\n", token);
 record_ptr->Date(atoi(token));	// TODO need to change this to string
 
 //
 // Yield or Spread tag
 //
 token = strtok(NULL," ");
 //printf("ValType: %s\n", token);
 record_ptr->ValType(token);
 
 //
 // Yield or Spread value
 //
 token = strtok(NULL," ");
 //printf("Val value: %s\n", token);
 if(NULL == token ) {
 fprintf(stderr,"line parsing failed on buf: %s\n", Line_buffer);
 exit(1);
 }
 record_ptr->ValValue(atof(token));
 
 //
 // benchmark token - e.g., T2, T5, T10, T30
 //
 token = strtok(NULL," ");
 //printf("benchmark: %s\n", token);
 if(NULL == token ) {
 fprintf(stderr,"line parsing failed on buf: %s\n", Line_buffer); 
 exit(1); 
 }
 record_ptr->BenchmarkID(token);
 
 _num_records++;
	}
 
	// as a convenience return number of records in param of this MF
	num_records = _num_records;
 
	return _fields_array;
 }
 
 void
 SBB_var_input_file::show()
 {
	printf("num days of data for var calc: %d\n", _num_records);
	for(int i=0;i<_num_records;i++){
 _fields_array[i].show();
	}
 }
 
 void
 SBB_var_input_file::free_records()
 {
	delete[] _fields_array;
 }


#if 0
int
SBB_var_input_file::Get_non_comment_row_count(const char * filename)
{
    //FILE* _file = std::fopen(filename, "r"); // open read-only
    FILE* _file = fopen(filename, "r"); // open read-only
    if (!_file){
        fprintf(stderr,"ERROR couldn't open: %s\n", filename);
        //throw std::runtime_error("file open failure");
        throw runtime_error("file open failure");
    }
    
    int length = get_row_count_minus_comments(SBB_LINE_BUFFER_LENGTH, Line_buffer, _file);
    
    //if (std::fclose(_file)) {
    if (fclose(_file)) {
        fprintf(stderr,"flose failed on file errno: %d\n", errno);
    }
    return length;
}
#endif

