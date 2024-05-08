eps=0.8
R=32
L=64
a=1.2
dt='float'
base_path='/ssd2/kishen/centroidHAC/mnist/'

bazel run //src:centroid_main -- -R $R -L $L -alpha $a -data_type $dt -base_path ${base_path}mnist.txt -test -graph_path ${base_path}mnist_32_64 -dendrogram_outfile ${base_path}mnist_dend_${eps}.txt -eps ${eps}