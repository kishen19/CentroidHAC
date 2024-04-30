// This code is part of the Problem Based Benchmark Suite (PBBS)
// Copyright (c) 2011 Guy Blelloch and the PBBS team
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights (to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <iostream>
#include <algorithm>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "parlay/parallel.h"
#include "parlay/primitives.h"

#include "Centroid/utils/parse_command_line.h"
#include "Centroid/utils/time_loop.h"
#include "Centroid/utils/euclidean_point.h"
#include "Centroid/utils/mips_point.h"
#include "Centroid/utils/point_range.h"
#include "Centroid/utils/graph.h"

#include "centroid.h"
#include "nn_types.h"

// *************************************************************
//  TIMING
// *************************************************************

using uint = unsigned int;

template<typename PointRange, typename indexType>
void Centroid_main(char* iFile, bool test, int rounds=1){
  PointRange Points;
  time_loop(rounds, 0,
    [&] () {
      Points = PointRange(iFile, test);
    },
    [&] () {
      CentroidHAC_Exact<PointRange, uint>(Points);
    },
    [&] () {}
  );
}

int main(int argc, char* argv[]) {
  commandLine P(argc,argv);

  // Get input files
  char* iFile = P.getOptionValue("-base_path");
  bool test = P.getOption("-test");

  // Get input params
  int rounds = P.getOptionIntValue("-rounds", 1);
  std::string tp = std::string(P.getOptionValue("-data_type"));
  std::string df = std::string(P.getOptionValue("-dist_func"));
  if((tp != "uint8") && (tp != "int8") && (tp != "float")){
    std::cout << "Error: vector type not specified correctly, specify int8, uint8, or float" << std::endl;
    abort();
  }
  if(df != "Euclidean" && df != "mips"){
    std::cout << "Error: specify distance type Euclidean or mips" << std::endl;
    abort();
  }

  if(tp == "float"){
    if(df == "Euclidean"){
      Centroid_main<PointRange<float, Euclidean_Point<float>>, uint>(iFile, test, rounds);
    } else if(df == "mips"){
      Centroid_main<PointRange<float, Mips_Point<float>>, uint>(iFile, test, rounds);
    }
  } else if(tp == "uint8"){
    if(df == "Euclidean"){
      Centroid_main<PointRange<uint8_t, Euclidean_Point<uint8_t>>, uint>(iFile, test, rounds);
    } else if(df == "mips"){
      Centroid_main<PointRange<uint8_t, Mips_Point<uint8_t>>, uint>(iFile, test, rounds);
    }
  } else if(tp == "int8"){
    if(df == "Euclidean"){
      Centroid_main<PointRange<int8_t, Euclidean_Point<int8_t>>, uint>(iFile, test, rounds);
    } else if(df == "mips"){
      Centroid_main<PointRange<int8_t, Mips_Point<int8_t>>, uint>(iFile, test, rounds);
    }
  }
  return 0;
}
