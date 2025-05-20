#include <thread>

#include "tape.h"

Tape::Tape( Tape &&other ) noexcept :
  data(other.data), length(other.length), pos(other.pos), maxElements(other.maxElements),
  tape(std::move(other.tape)), config(other.config),
  boundMin(other.boundMin), boundMax(other.boundMax), firstElement(other.firstElement)
{
  other.data = nullptr;
}

Tape & Tape::operator =( Tape &&other ) noexcept
{
  if (this != &other)
  {
    data = other.data;
    length = other.length;
    pos = other.pos;
    maxElements = other.maxElements;
    config = other.config;
    tape = std::move(other.tape);
    boundMin = other.boundMin;
    boundMax = other.boundMax;
    firstElement = other.firstElement;

    other.data = nullptr;
  }

  return *this;
}

Tape::Tape( const std::string &fileName, const std::string &configFileName ) : 
  pos(0), tape(fileName, std::ifstream::binary)
{
  std::ifstream configFile(configFileName);

  /* Check if files are opened */
  if (!tape)
    throw std::runtime_error("Can't open the file with the tape!");
  else if (!configFile)
    throw std::runtime_error("Can't open the file with configuration!");

  /* Read config and check the correctness */
  configFile >> config.readWrite >> config.rewind >> config.move;

  if (configFile.bad() && !configFile.eof())
    throw std::runtime_error("Configuration file is invalid!");
  configFile.close();

  /* Read the tape */
  tape.read((char *)&length, 4);
  tape.read((char *)&maxElements, 4);

  maxElements /= sizeof(int);

  if (tape.bad() || length <= 0 || maxElements <= 0)
    throw std::runtime_error("Tape is invalid!");

  data = new int[maxElements];

  for (int i = 0; i < maxElements; i++)
  {
    tape.read((char *)&data[i], sizeof(int));

    if (tape.eof() && i != maxElements - 1)
      throw std::runtime_error("Unexpected end of tape!");
  }

  boundMin = 0;
  boundMax = maxElements - 1;
  firstElement = 0;
}

int Tape::read( void ) const
{
  int index = (firstElement + pos - boundMin) % maxElements;

  // Config delay
  std::this_thread::sleep_for(std::chrono::milliseconds(config.readWrite));

  return data[index];
}

void Tape::write( int value )
{
  int index = (firstElement + pos - boundMin) % maxElements;

  data[index] = value;

  // Config delay
  std::this_thread::sleep_for(std::chrono::milliseconds(config.readWrite));
}

void Tape::moveBackward( void )
{
  if (pos - 1 < 0)
    throw std::invalid_argument("Illegal backward move!");

  pos--;

  if (pos < boundMin)
  {
    int lastElement = (firstElement - 1 + maxElements) % maxElements;

    // Data paging from tape (we still storage only M bytes)
    tape.seekg(-maxElements * sizeof(int) - sizeof(int), std::ifstream::cur);
    tape.read((char *)&data[lastElement], sizeof(int));
    tape.seekg(maxElements * sizeof(int) - sizeof(int), std::ifstream::cur);

    boundMin--;
    boundMax--;
    firstElement = lastElement;
  }
}

Tape & Tape::operator--( void )
{
  moveBackward();
  return *this;
}

void Tape::moveForward( void )
{
  if (pos + 1 >= length)
    throw std::invalid_argument("Illegal forward move!");

  pos++;

  if (pos > boundMax)
  {
    tape.read((char *)&data[firstElement], sizeof(int));

    if (tape.eof())
      throw std::runtime_error("Error in tape file!");

    boundMin++;
    boundMax++;
    firstElement = (firstElement + 1) % maxElements;
  }

  // Config delay
  std::this_thread::sleep_for(std::chrono::milliseconds(config.move));
}

Tape & Tape::operator++( void )
{
  moveForward();
  return *this;
}

bool Tape::isBegin( void ) const
{
  return pos == 0;
}

bool Tape::isEnd( void ) const
{
  return pos == length - 1;
}

void Tape::rewind( void )
{
  pos = 0;
  boundMin = 0;
  firstElement = 0;
  boundMax = maxElements - 1;

  tape.clear();
  tape.seekg(8, std::ifstream::beg); // skip length and M

  for (int i = 0; i < maxElements; i++)
    tape.read((char *)&data[i], sizeof(int));

  // Config delay
  std::this_thread::sleep_for(std::chrono::milliseconds(config.rewind));
}

Tape::~Tape( void )
{
  if (data != nullptr)
    delete data, data = nullptr;
  tape.close();
}
