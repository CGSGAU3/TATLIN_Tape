#ifndef _TAPE_H
#define _TAPE_H

#include <string>
#include <fstream>

/* Structure for delay configuration */
struct DelayConfig
{
  int readWrite;
  int rewind;
  int move;
};

/* Class for emulating the tape */
class Tape
{
private:

  int *data;                 // Tape data
  int length;                // Length of the tape
  int pos;                   // Current position of tape
  bool readFlag;             // Shows if we have read the data

  int maxElements;           // Maximum elements we can keep in RAM
  int boundMin;              // Left bound of the visible tape
  int boundMax;              // Right bound of the visible tape
  int firstElement;          // Pointer to a first element of the visible tape

  DelayConfig config;        // Configuration for the delay

  std::ifstream tape;        // File with the data of the tape

  friend class TapeSorter;
public:

  /* Constructor that read the tape (we have a file) */
  Tape( const std::string &fileName, const std::string &configFileName );

  /* Copy constructor - don't need */
  Tape( const Tape &other ) = delete;
  Tape & operator =( const Tape &other ) = delete;

  /* Move constructor and assignment */
  Tape( Tape &&other ) noexcept;
  Tape & operator =( Tape &&other ) noexcept;

  /* Methods for read & write data */
  int read( void );
  bool readMove( int &ref );
  void write( int value );

  /* Methods for move the tape */
  void moveForward( void );
  void moveBackward( void );
  void rewind( void );

  /* Some methods to control iteration */
  Tape & operator ++( void );
  Tape & operator --( void );
  bool isEnd( void ) const;
  bool isBegin( void ) const;

  /* Clear methods */
  void clear( void );

  ~Tape( void );
};

#endif // _TAPE_H
