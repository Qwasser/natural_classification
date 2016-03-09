%module wrapper
%{
#include "wrapper/data_wrapper.h"
#include "wrapper/rules_wrapper.h"
%}

%include "std_vector.i"
%include "std_string.i"

// Instantiate templates used by example
namespace std {
   %template(DoubleVector) vector<double>;
   %template(IntVector) vector<int>;
   %template(StringVector) vector<string>;
   %template(StringStringVector) vector< vector<string> >;
}

// Include the header file with above prototypes

%include "wrapper/data_wrapper.h"
%include "wrapper/rules_wrapper.h"
