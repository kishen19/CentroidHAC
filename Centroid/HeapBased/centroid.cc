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
#include "Centroid/utils/NSGDist.h"
#include "Centroid/utils/euclidean_point.h"
#include "Centroid/utils/point_range.h"
#include "Centroid/utils/mips_point.h"
#include "Centroid/utils/graph.h"
#include "Centroid/utils/aspen_graph.h"
#include "Centroid/utils/aspen_flat_graph.h"

#include "centroid.h"

// *************************************************************
//  TIMING
// *************************************************************

using uint = unsigned int;

//file order: {base, query, graph, groundtruth, graph_outfile, res_file}
template<typename PointRange, typename GraphType>
void Centroid_main(parlay::sequence<char*> files, long k, BuildParams &BP, bool test){
  using indexType = typename GraphType::iT;
  using Point = typename PointRange::pT;

  //load points (filename, bool: double the size)
  PointRange Points = PointRange(files[0], test);
  size_t n = Points.size();

  //load graph
  long maxDeg = BP.max_degree();
  bool graph_built = (files[2] != NULL);
  std::cout << graph_built << std::endl;
  GraphType Graph;
  if(graph_built){
    std::cout << "Loading Graph..." << std::endl;
    Graph = GraphType(files[2]);
  } else{
    std::cout << "Building Graph..." << std::endl;
    Graph = GraphType(maxDeg, n);
  }
  std::cout << "Centroid..." << std::endl;
  Centroid<Point, PointRange, indexType, GraphType>(Graph, k, BP, files[4], graph_built, Points);
}

int main(int argc, char* argv[]) {
    commandLine P(argc,argv,
    "[-a <alpha>] [-d <delta>] [-R <deg>]"
        "[-L <bm>] [-k <k> ]  [-gt_path <g>] [-query_path <qF>]"
        "[-graph_path <gF>] [-graph_outfile <oF>] [-res_path <rF>]"
        "[-memory_flag <algoOpt>] [-mst_deg <q>] [num_clusters <nc>] [cluster_size <cs>]"
        "[-data_type <tp>] [-dist_func <df>][-base_path <b>] <inFile>");

  // Get all input params
  char* iFile = P.getOptionValue("-base_path");
  char* oFile = P.getOptionValue("-graph_outfile");
  char* gFile = P.getOptionValue("-graph_path");
  char* qFile = P.getOptionValue("-query_path");
  char* cFile = P.getOptionValue("-gt_path");
  char* rFile = P.getOptionValue("-res_path");
  char* vectype = P.getOptionValue("-data_type");
  std::string tp = std::string(vectype);
  long R = P.getOptionIntValue("-R", 0);
  if(R<0) P.badArgument();
  long L = P.getOptionIntValue("-L", 0);
  if(L<0) P.badArgument();

  long MST_deg = P.getOptionIntValue("-mst_deg", 0);
  if(MST_deg < 0) P.badArgument();
  long num_clusters = P.getOptionIntValue("-num_clusters", 0);
  if(num_clusters<0) P.badArgument();
  long cluster_size = P.getOptionIntValue("-cluster_size", 0);
  if(cluster_size<0) P.badArgument();
  long k = P.getOptionIntValue("-k", 0);
  if (k > 1000 || k < 0) P.badArgument();
  double alpha = P.getOptionDoubleValue("-alpha", 1.0);
  int two_pass = P.getOptionIntValue("-two_pass", 0);
  if(two_pass > 1 | two_pass < 0) P.badArgument();
  bool pass = (two_pass == 1);
  double delta = P.getOptionDoubleValue("-delta", 0);
  if(delta<0) P.badArgument();
  char* dfc = P.getOptionValue("-dist_func");
  std::string df = std::string(dfc);
  char* gt = P.getOptionValue("-graph_type");
  std::string graph_type = std::string(gt);
  bool test = (P.getOptionIntValue("-test", 0)==1);

  if((tp != "uint8") && (tp != "int8") && (tp != "float")){
    std::cout << "Error: vector type not specified correctly, specify int8, uint8, or float" << std::endl;
    abort();
  }
  if(df != "Euclidian" && df != "mips"){
    std::cout << "Error: specify distance type Euclidian or mips" << std::endl;
    abort();
  }
  if(graph_type != "flat" && graph_type != "aspen" && graph_type != "aspen_flat"){
    std::cout << "Error: specify graph type flat or aspen or aspen_flat" << std::endl;
    abort();
  }

  BuildParams BP = BuildParams(R, L, alpha, pass, num_clusters, cluster_size, MST_deg, delta);
  long maxDeg = BP.max_degree();
  parlay::sequence<char*> files = {iFile, qFile, gFile, cFile, oFile, rFile};

  std::cout << "Starting..." << std::endl;
  if(graph_type == "flat"){
    if(tp == "float"){
      if(df == "Euclidian"){
        Centroid_main<PointRange<float, Euclidian_Point<float>>, Flat_Graph<uint>>(files, k, BP, test);
      } else if(df == "mips"){
        Centroid_main<PointRange<float, Mips_Point<float>>, Flat_Graph<uint>>(files, k, BP, test);
      }
    } else if(tp == "uint8"){
      if(df == "Euclidian"){
        Centroid_main<PointRange<uint8_t, Euclidian_Point<uint8_t>>, Flat_Graph<uint>>(files, k, BP, test);
      } else if(df == "mips"){
        Centroid_main<PointRange<uint8_t, Mips_Point<uint8_t>>, Flat_Graph<uint>>(files, k, BP, test);
      }
    } else if(tp == "int8"){
      if(df == "Euclidian"){
        Centroid_main<PointRange<int8_t, Euclidian_Point<int8_t>>, Flat_Graph<uint>>(files, k, BP, test);
      } else if(df == "mips"){
        Centroid_main<PointRange<int8_t, Mips_Point<int8_t>>, Flat_Graph<uint>>(files, k, BP, test);
      }
    }
  } else if(graph_type == "aspen"){
    if(tp == "float"){
      if(df == "Euclidian"){
        Centroid_main<PointRange<float, Euclidian_Point<float>>, Aspen_Graph<uint>>(files, k, BP, test);
      } else if(df == "mips"){
        Centroid_main<PointRange<float, Mips_Point<float>>, Aspen_Graph<uint>>(files, k, BP, test);
      }
    } else if(tp == "uint8"){
      if(df == "Euclidian"){
        Centroid_main<PointRange<uint8_t, Euclidian_Point<uint8_t>>, Aspen_Graph<uint>>(files, k, BP, test);
      } else if(df == "mips"){
        Centroid_main<PointRange<uint8_t, Mips_Point<uint8_t>>, Aspen_Graph<uint>>(files, k, BP, test);
      }
    } else if(tp == "int8"){
      if(df == "Euclidian"){
        Centroid_main<PointRange<int8_t, Euclidian_Point<int8_t>>, Aspen_Graph<uint>>(files, k, BP, test);
      } else if(df == "mips"){
        Centroid_main<PointRange<int8_t, Mips_Point<int8_t>>, Aspen_Graph<uint>>(files, k, BP, test);
      }
    }
  } else if(graph_type == "aspen_flat"){
    if(tp == "float"){
      if(df == "Euclidian"){
        Centroid_main<PointRange<float, Euclidian_Point<float>>, Aspen_Flat_Graph<uint>>(files, k, BP, test);
      } else if(df == "mips"){
        Centroid_main<PointRange<float, Mips_Point<float>>, Aspen_Flat_Graph<uint>>(files, k, BP, test);
      }
    } else if(tp == "uint8"){
      if(df == "Euclidian"){
        Centroid_main<PointRange<uint8_t, Euclidian_Point<uint8_t>>, Aspen_Flat_Graph<uint>>(files, k, BP, test);
      } else if(df == "mips"){
        Centroid_main<PointRange<uint8_t, Mips_Point<uint8_t>>, Aspen_Flat_Graph<uint>>(files, k, BP, test);
      }
    } else if(tp == "int8"){
      if(df == "Euclidian"){
        Centroid_main<PointRange<int8_t, Euclidian_Point<int8_t>>, Aspen_Flat_Graph<uint>>(files, k, BP, test);
      } else if(df == "mips"){
        Centroid_main<PointRange<int8_t, Mips_Point<int8_t>>, Aspen_Flat_Graph<uint>>(files, k, BP, test);
      }
    }
  }
  return 0;
}
