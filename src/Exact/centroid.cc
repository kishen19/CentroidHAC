#include "parlay/parallel.h"
#include "parlay/primitives.h"

#include "src/utils/parse_command_line.h"
#include "src/utils/euclidean_point.h"
#include "src/utils/point_range.h"

#include "centroid.h"

// *************************************************************
//  TIMING
// *************************************************************

using uint = unsigned int;

//file order: {PointsFile, DendrogramFile}
template<typename PointRange, typename indexType>
void Centroid_main(parlay::sequence<char*> files, bool test, size_t rounds=1){
  PointRange Points;
  double total_time = 0.0;
  for (size_t r=0; r<rounds; r++){
    Points = PointRange(files[0], test);
    total_time += CentroidHAC_Exact<PointRange, uint>(Points, files[1]);
  }
  std::cout << "# time per iter: " << total_time/rounds << std::endl;
}

int main(int argc, char* argv[]) {
  commandLine P(argc,argv);

  // Get input files
  char* PointsFile = P.getOptionValue("-base_path");
  char* DendrogramFile = P.getOptionValue("-dendrogram_outfile");
  bool test = P.getOption("-test");

  // Get input params
  size_t rounds = P.getOptionIntValue("-rounds", 1);
  std::string tp = std::string(P.getOptionValue("-data_type"));
  if((tp != "uint8") && (tp != "int8") && (tp != "float")){
    std::cout << "Error: vector type not specified correctly, specify int8, uint8, or float" << std::endl;
    abort();
  }

  parlay::sequence<char*> files = {PointsFile, DendrogramFile};

  if(tp == "float"){
    Centroid_main<PointRange<float, Euclidean_Point<float>>, uint>(files, test, rounds);
  } else if(tp == "uint8"){
    Centroid_main<PointRange<uint8_t, Euclidean_Point<uint8_t>>, uint>(files, test, rounds);
  } else if(tp == "int8"){
    Centroid_main<PointRange<int8_t, Euclidean_Point<int8_t>>, uint>(files, test, rounds);
  }
  return 0;
}
