#pragma once

namespace example
{

/**
  @class ExampleClass
  @author A. Author Name
  @date 02/07/2017

  @brief A class that only lets you accumulate a number

  A longer description can go here.

  @ingroup module1
*/
class ExampleClass
{
public:
    /** Constructor */
    ExampleClass() : private_var_(0) {}

    /** Accumulate onto private_var_ and return result */
    int addTo(int i);

    /** Get private_var_ */
    int getFrom();

private:
    int private_var_;
};
}
