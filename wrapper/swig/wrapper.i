%module wrapper
%{
#include "wrapper/data_wrapper.h"
#include "wrapper/rules_wrapper.h"
#include "wrapper/ideal_object_wrapper.h"
#include "wrapper/object_idealizer.h"
%}

%include "std_vector.i"
%include "std_string.i"

// Instantiate templates used by example
namespace std {
   %template(TokenVector) vector<TokenWrapper>;
   %template(DoubleVector) vector<double>;
   %template(IntVector) vector<int>;
   %template(StringVector) vector<string>;
   %template(StringStringVector) vector< vector<string> >;
   %template(BoolVector) vector<bool>;
   %template(BoolBoolVector) vector< vector<bool> >;
}

// Include the header file with above prototypes

%include "wrapper/data_wrapper.h"
%include "wrapper/rules_wrapper.h"
%include "wrapper/ideal_object_wrapper.h"
%include "wrapper/object_idealizer.h"
