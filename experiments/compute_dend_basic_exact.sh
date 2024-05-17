dt='float'
# base_path='/ssd2/kishen/centroidHAC/basic/'
base_path='/home/kishen/CentroidHAC/data1/basic/'
datasets=('iris' 'wine' 'breast_cancer' 'digits' 'faces')

for dataset in "${datasets[@]}"
do
    echo "Running for ${dataset}"
    bazel run //src:centroid_exact_main -- -data_type $dt -base_path ${base_path}${dataset}/${dataset}.txt -test -dendrogram_outfile ${base_path}${dataset}/dend/${dataset}_dend_centroid_exact.txt
done