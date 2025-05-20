#include <iostream>
#include <filesystem>

#include "tape_sorter.h"

int main( int argC, char *argV[] )
{
  if (argC == 1)
  {
    /* Generate tape */
    int len, view; // 100 elements, 7 visible
    std::ofstream test("tapes/test.bin", std::ofstream::binary);

    std::cout << "Input tape size in elements: ";
    std::cin >> len;

    std::cout << "Input visible size in BYTES (1 element = 4 bytes): ";
    std::cin >> view;

    test.write((char *)&len, sizeof(int));
    test.write((char *)&view, sizeof(int));

    srand(clock());
    for (int i = 0; i < len; i++)
    {
      int x = rand() * (rand() % 30);

      test.write((char *)&x, sizeof(int));
    }
    test.close();

    /* Test generated tape */
    try
    {
      Tape tape("tapes/test.bin", "config.ini");

      std::cout << "Input tape:" << std::endl;

      for (tape.rewind(); !tape.isEnd(); ++tape)
        std::cout << tape.read() << std::endl;
      std::cout << std::endl;

      Tape res = TapeSorter(tape).sort();

      std::cout << "Output tape:" << std::endl;
      for (res.rewind(); !res.isEnd(); ++res)
        std::cout << res.read() << std::endl;

      res.clear();

      for (const auto &entry : std::filesystem::directory_iterator("tmp"))
        std::remove(entry.path().string().c_str());

      return EXIT_SUCCESS;
    }
    catch ( const std::exception &e )
    {
      std::cerr << e.what() << std::endl;
      return EXIT_FAILURE;
    }
    catch (...)
    {
      std::cerr << "Something goes bad..." << std::endl;
      return EXIT_FAILURE;
    }
  }
  else if (argC == 3)
  {
    try
    {
      Tape tape(argV[1], "config.ini");

      if (std::string(argV[2]) == "-view")
      {
        for (tape.rewind(); !tape.isEnd(); ++tape)
          std::cout << tape.read() << std::endl;
        std::cout << std::endl;

        return EXIT_SUCCESS;
      }

      TapeSorter(tape).sort();

      for (const auto &entry : std::filesystem::directory_iterator("tmp"))
        std::rename(entry.path().string().c_str(), argV[2]);
    }
    catch ( const std::exception &e )
    {
      std::cerr << e.what() << std::endl;
      return EXIT_FAILURE;
    }
    catch (...)
    {
      std::cerr << "Something goes bad..." << std::endl;
      return EXIT_FAILURE;
    }
  }
  else
  {
    std::cout << "Incorrect usage!" << std::endl;
    std::cout << "Usage: tape <input file> <output file>" << std::endl;
    std::cout << "Or: tape (if u want to do test for yourself)" << std::endl;
    std::cout << "Or: tape <input file> -view" << std::endl;
    return EXIT_FAILURE;
  }
}