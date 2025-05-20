#include <vector>
#include <algorithm>
#include <filesystem>

#include "tape_sorter.h"

void TapeSorter::split( void )
{
  int mergeCnt = 0;
  int elemCnt = 0;
  int tmpLen = 100000;
  std::vector<int> tmpData;

  try
  {
    while (true)
    {
      if (elemCnt < tmpLen)
      {
        if (elemCnt == 0)
        {
          int remain = input.length - mergeCnt * input.maxElements;
          tmpLen = remain >= input.maxElements ? input.maxElements : remain;

          // N = M for temporary files
          tmpData.push_back(tmpLen);
          tmpData.push_back(tmpLen * sizeof(int));
        }
        tmpData.push_back(input.read());
        elemCnt++;

        if (!input.isEnd())
          input.moveForward();
        else
          break;
      }
      else
      {
        std::ofstream tmpTape("tmp/tape" + std::to_string(mergeCnt++) + ".bin", std::ofstream::binary);

        std::sort(tmpData.begin() + 2, tmpData.end());
        tmpTape.write((char *)tmpData.data(), sizeof(int) * tmpData.size());
        tmpData.clear();
        elemCnt = 0;
      }
    }

    if (elemCnt != 0)
    {
      std::ofstream tmpTape("tmp/tape" + std::to_string(mergeCnt++) + ".bin", std::ofstream::binary);

      std::sort(tmpData.begin() + 2, tmpData.end());
      tmpTape.write((char *)tmpData.data(), sizeof(int) * tmpData.size());
    }
  }
  catch ( const std::exception &e )
  {
    throw e;
  }
  catch (...)
  {
    throw std::runtime_error("Somethins goes wrong!");
  }
}

void TapeSorter::mergePair( const std::string &tape1, const std::string &tape2, const std::string &dest )
{
  Tape t1(tape1, "config.ini");
  Tape t2(tape2, "config.ini");
  std::ofstream res(dest, std::ofstream::binary);

  int tmp = t1.length + t2.length;

  res.write((char *)&tmp, sizeof(int));
  tmp = std::max(t1.maxElements, t2.maxElements) * 4;
  res.write((char *)&tmp, sizeof(int));

  int x, y;

  bool has[2] = {t1.readMove(x), t2.readMove(y)};

  while (has[0] && has[1])
  {
    if (x < y)
    {
      res.write((char *)&x, sizeof(int));
      has[0] = t1.readMove(x);
    }
    else
    {
      res.write((char *)&y, sizeof(int));
      has[1] = t2.readMove(y);
    }
  }

  while (has[0])
  {
    res.write((char *)&x, sizeof(int));
    has[0] = t1.readMove(x);
  }

  while (has[1])
  {
    res.write((char *)&y, sizeof(int));
    has[1] = t2.readMove(y);
  }

  res.close();
}

std::string TapeSorter::merge( void )
{
  std::vector<std::string> tapes;

  for (const auto &entry : std::filesystem::directory_iterator("tmp"))
  {
    if (entry.path().string() != "tmp\\.gitkeep")
      tapes.push_back(entry.path().string());
  }

  int iter = 0;

  while (tapes.size() > 1)
  {
    std::vector<std::string> newTapes;

    for (int i = 0; i < (int)tapes.size() - 1; i += 2)
    {
      std::string merged = "tmp/merge_" + std::to_string(iter) + "_" + std::to_string(i / 2) + ".bin";

      mergePair(tapes[i], tapes[i + 1], merged);
      newTapes.push_back(merged);
    }

    if (tapes.size() % 2 == 1)
      newTapes.push_back(tapes.back());

    tapes = newTapes;
    iter++;
  }
  return tapes[0];
}

Tape TapeSorter::sort( void )
{
  split();

  Tape res = Tape(merge(), "config.ini");

  for (const auto &entry : std::filesystem::directory_iterator("tmp"))
  {
    if (entry.path().string() != "tmp\\.gitkeep")
      std::remove(entry.path().string().c_str());
  }

  return res;
}