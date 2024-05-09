dt='float'
base_path='/ssd2/kishen/centroidHAC/basic/'
datasets=('iris' 'breast_cancer' 'digits' 'wine')

for dataset in "${datasets[@]}"
do
    echo "Running for ${dataset}"
    bazel run //src:centroid_exact_main -- -data_type $dt -base_path ${base_path}${dataset}/${dataset}.txt -test -dendrogram_outfile ${base_path}${dataset}/${dataset}_dend_exact.txt
done