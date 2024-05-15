eps=$1
R=32
L=64
a=1.2
dt='float'
# base_path='/ssd2/kishen/centroidHAC/basic/'
base_path='/home/kishen/CentroidHAC/data1/basic/'
datasets=('iris' 'wine' 'breast_cancer' 'digits' 'faces')

for dataset in "${datasets[@]}"
do
    echo "Running for ${dataset} ${eps}"
    bazel run //src:centroid_main -- -R $R -L $L -alpha $a -data_type $dt -base_path ${base_path}${dataset}/${dataset}.txt -test -dendrogram_outfile ${base_path}${dataset}/${dataset}_dend_${eps}.txt -eps ${eps} -rounds 3
done