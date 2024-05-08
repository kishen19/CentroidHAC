# Iris dataset
bazel run //src:centroid_main -- -R 32 -L 64 -alpha 1.2 -two_pass 0 -data_type float -base_path /ssd2/kishen/centroidHAC/basic/iris/iris.txt -test -dendrogram_outfile /ssd2/kishen/centroidHAC/basic/iris/iris_dend_0.1.txt -eps 0.1

# breast_cancer
bazel run //src:centroid_main -- -R 32 -L 64 -alpha 1.2 -two_pass 0 -data_type float -dist_func Euclidean -base_path /ssd2/kishen/centroidHAC/basic/breast_cancer/breast_cancer.txt -test -dendrogram_outfile /ssd2/kishen/centroidHAC/basic/breast_cancer/breast_cancer_dend_0.1.txt -eps 0.1

# california_housing
bazel run //src:centroid_main -- -R 32 -L 64 -alpha 1.2 -two_pass 0 -data_type float -dist_func Euclidean -base_path /ssd2/kishen/centroidHAC/basic/california_housing/california_housing.txt -test -dendrogram_outfile /ssd2/kishen/centroidHAC/basic/california_housing/california_housing_dend_0.1.txt -eps 0.1

# diabetes
bazel run //src:centroid_main -- -R 32 -L 64 -alpha 1.2 -two_pass 0 -data_type float -dist_func Euclidean -base_path /ssd2/kishen/centroidHAC/basic/diabetes/diabetes.txt -test -dendrogram_outfile /ssd2/kishen/centroidHAC/basic/diabetes/diabetes_dend_0.1.txt -eps 0.1 

# digits
bazel run //src:centroid_main -- -R 32 -L 64 -alpha 1.2 -two_pass 0 -data_type float -dist_func Euclidean -base_path /ssd2/kishen/centroidHAC/basic/digits/digits.txt -test -dendrogram_outfile /ssd2/kishen/centroidHAC/basic/digits/digits_dend_0.1.txt -eps 0.1

# linnerud
bazel run //src:centroid_main -- -R 32 -L 64 -alpha 1.2 -two_pass 0 -data_type float -dist_func Euclidean -base_path /ssd2/kishen/centroidHAC/basic/linnerud/linnerud.txt -test -dendrogram_outfile /ssd2/kishen/centroidHAC/basic/linnerud/linnerud_dend_0.1.txt -eps 0.1

# wine
bazel run //src:centroid_main -- -R 32 -L 64 -alpha 1.2 -two_pass 0 -data_type float -dist_func Euclidean -base_path /ssd2/kishen/centroidHAC/basic/wine/wine.txt -test -dendrogram_outfile /ssd2/kishen/centroidHAC/basic/wine/wine_dend_0.1.txt -eps 0.1
