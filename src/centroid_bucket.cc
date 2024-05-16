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

#include "utils/parse_command_line.h"
#include "utils/euclidean_point.h"
#include "utils/point_range.h"
#include "utils/graph.h"
#include "utils/nn_types.h"

#include "centroid_bucket.h"

// *************************************************************
//  TIMING
// *************************************************************

using uint = unsigned int;

//file order: {PointsFile, IndexFile, SaveIndexFile, DendrogramFile}
template<typename PointRange, typename indexType>
void Centroid_main(parlay::sequence<char*> files, bool test, bool exact, 
    double eps, long R, long L, double alpha, bool pass, 
    long num_clusters, long cluster_size, long MST_deg, double delta, size_t rounds = 1){
  PointRange Points;
  double total_time = 0.0;
  for (size_t r=0; r<rounds; r++){
    Points = PointRange(files[0], test);
    if (exact){
      auto NN = nn_exact<PointRange, indexType>();
      total_time += CentroidHAC_Bucket<PointRange, indexType, nn_exact<PointRange, indexType>>(Points, NN, eps, files[2], files[3]);
    } else {
      auto NN = nn_knn<PointRange, indexType>(files[1], R, L, alpha, pass, Points.size());
      total_time += CentroidHAC_Bucket<PointRange, indexType, nn_knn<PointRange, indexType>>(Points, NN, eps, files[2], files[3]);
    }
  }
  std::cout << "# time per iter: " << total_time/rounds << std::endl;
}

int main(int argc, char* argv[]) {
  commandLine P(argc,argv);

  // Get input files
  char* PointsFile = P.getOptionValue("-base_path");
  char* SaveIndexFile = P.getOptionValue("-graph_outfile");
  char* IndexFile = P.getOptionValue("-graph_path");
  char* DendrogramFile = P.getOptionValue("-dendrogram_outfile");
  bool test = P.getOption("-test");

  // Get input params
  double eps = P.getOptionDoubleValue("-eps", 0.2); if(eps<0) P.badArgument();
  size_t rounds = P.getOptionIntValue("-rounds", 1);
  std::string tp = std::string(P.getOptionValue("-data_type"));
  if((tp != "uint8") && (tp != "int8") && (tp != "float")){
    std::cout << "Error: vector type not specified correctly, specify int8, uint8, or float" << std::endl;
    abort();
  }

  // If using Exact NNS
  bool exact = P.getOption("-exact");
  // O/w, get ANN related params
  long R = P.getOptionIntValue("-R", 0); if(R<0) P.badArgument();
  long L = P.getOptionIntValue("-L", 0); if(L<0) P.badArgument();
  long MST_deg = P.getOptionIntValue("-mst_deg", 0);
  if(MST_deg < 0) P.badArgument();
  long num_clusters = P.getOptionIntValue("-num_clusters", 0);
  if(num_clusters<0) P.badArgument();
  long cluster_size = P.getOptionIntValue("-cluster_size", 0);
  if(cluster_size<0) P.badArgument();
  double alpha = P.getOptionDoubleValue("-alpha", 1.0);
  int two_pass = P.getOptionIntValue("-two_pass", 0);
  if(two_pass > 1 || two_pass < 0) P.badArgument();
  bool pass = (two_pass == 1);
  double delta = P.getOptionDoubleValue("-delta", 0); if(delta<0) P.badArgument();
  
  parlay::sequence<char*> files = {PointsFile, IndexFile, SaveIndexFile, DendrogramFile};

  if(tp == "float"){
    Centroid_main<PointRange<float, Euclidean_Point<float>>, uint>(files, test, exact, eps, R, L, alpha, pass, num_clusters, cluster_size, MST_deg, delta, rounds);
  } else if(tp == "uint8"){
    Centroid_main<PointRange<uint8_t, Euclidean_Point<uint8_t>>, uint>(files, test, exact, eps, R, L, alpha, pass, num_clusters, cluster_size, MST_deg, delta, rounds);
  } else if(tp == "int8"){
    Centroid_main<PointRange<int8_t, Euclidean_Point<int8_t>>, uint>(files, test, exact, eps, R, L, alpha, pass, num_clusters, cluster_size, MST_deg, delta, rounds);
  }
  return 0;
}
