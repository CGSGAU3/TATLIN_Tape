#ifndef _TAPE_SORTER_H
#define _TAPE_SORTER_H

#include "tape.h"

/* Tape sorter class */
class TapeSorter
{
private:

  Tape &input;

  void split( void );
  std::string merge( void );
  void mergePair( const std::string &tape1, const std::string &tape2, const std::string &dest );

public:

  TapeSorter( Tape &tape ) : input(tape)
  {
    input.rewind();
  }

  Tape sort( void );
};

#endif // _TAPE_SORTER_H
