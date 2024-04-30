bazel build //Centroid/HeapBased:centroid_main &> log.txt
bazel run //Centroid/HeapBased:centroid_main -- -R 32 -L 64 -alpha 1.2 -two_pass 0 -data_type float -dist_func Euclidean -base_path /ssd2/kishen/anndata/sift/sift_learn.fbin -graph_path /ssd2/kishen/anndata/sift/sift_learn_32_64

# To build index (and save) from scratch
# bazel run //Centroid/HeapBased:centroid_main -- -R 32 -L 64 -alpha 1.2 -two_pass 1 -data_type float -dist_func Euclidean -base_path /ssd2/kishen/anndata/sift/sift_learn.fbin -graph_outfile /ssd2/kishen/anndata/sift/sift_learn_32_64

# Small dataset testing
# bazel run //Centroid/HeapBased:centroid_main -- -R 32 -L 64 -alpha 1.2 -two_pass 1 -data_type float -dist_func Euclidean -base_path /ssd2/kishen/anndata/test/2d_line.txt -test

# Use flag -exact to get exact distances. However, this will still be (1+eps) approx due to the else if block in centroid.h.