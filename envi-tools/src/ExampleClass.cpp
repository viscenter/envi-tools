#include "module1/ExampleClass.hpp"

using namespace example;

int ExampleClass::addTo(int i) { return private_var_ += i; }

int ExampleClass::getFrom() { return private_var_; }
