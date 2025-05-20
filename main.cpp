#include <iostream>

#include "tape.h"

int main( void )
{
  Tape tape("tape1.bin", "config.ini");

  try
  {
    for (tape.rewind(); !tape.isEnd(); ++tape)
    {
      std::cout << tape.read() << std::endl;
    }
    std::cout << tape.read() << std::endl;
  }
  catch ( const std::exception &e )
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}