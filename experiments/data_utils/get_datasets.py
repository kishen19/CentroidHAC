from sklearn import datasets
import numpy as np

base_path = "/ssd2/kishen/centroidHAC/basic/"

def get_iris_dataset(save = True):
    iris = datasets.load_iris()
    data, labels = iris.data, iris.target
    if save:
        np.savetxt(base_path + "iris/iris.txt", data, fmt="%.5f", delimiter=" ", header = "%i %i"%(data.shape[0],data.shape[1]),comments='')
        np.savetxt(base_path + "iris/iris.gt", labels, fmt="%i")
    return data, labels


def get_digits_dataset(save = True):
    digits = datasets.load_digits()
    data, labels = digits.data, digits.target
    if save:
        np.savetxt(base_path + "digits/digits.txt", data, fmt="%.5f", delimiter=" ", header = "%i %i"%(data.shape[0],data.shape[1]),comments='')
        np.savetxt(base_path + "digits/digits.gt", labels, fmt="%i")
    return data, labels

def get_wine_dataset(save = True):
    wine = datasets.load_wine()
    data, labels = wine.data, wine.target
    if save:
        np.savetxt(base_path + "wine/wine.txt", data, fmt="%.5f", delimiter=" ", header = "%i %i"%(data.shape[0],data.shape[1]),comments='')
        np.savetxt(base_path + "wine/wine.gt", labels, fmt="%i")
    return data, labels

def get_breast_cancer_dataset(save = True):
    breast_cancer = datasets.load_breast_cancer()
    data, labels = breast_cancer.data, breast_cancer.target
    if save:
        np.savetxt(base_path + "breast_cancer/breast_cancer.txt", data, fmt="%.5f", delimiter=" ", header = "%i %i"%(data.shape[0],data.shape[1]),comments='')
        np.savetxt(base_path + "breast_cancer/breast_cancer.gt", labels, fmt="%i")
    return data, labels

def get_california_housing_dataset(save = True):
    california_housing = datasets.fetch_california_housing()
    data, labels = california_housing.data, california_housing.target
    if save:
        np.savetxt(base_path + "california_housing/california_housing.txt", data, fmt="%.5f", delimiter=" ", header = "%i %i"%(data.shape[0],data.shape[1]),comments='')
        np.savetxt(base_path + "california_housing/california_housing.gt", labels, fmt="%.5f")
    return data, labels

def get_diabetes_dataset(save = True):
    diabetes = datasets.load_diabetes()
    data, labels = diabetes.data, diabetes.target
    if save:
        np.savetxt(base_path + "diabetes/diabetes.txt", data, fmt="%.5f", delimiter=" ", header = "%i %i"%(data.shape[0],data.shape[1]),comments='')
        np.savetxt(base_path + "diabetes/diabetes.gt", labels, fmt="%.5f")
    return data, labels

def get_linnerud_dataset(save = True):
    linnerud = datasets.load_linnerud()
    data, labels = linnerud.data, linnerud.target
    if save:
        np.savetxt(base_path + "linnerud/linnerud.txt", data, fmt="%.5f", delimiter=" ", header = "%i %i"%(data.shape[0],data.shape[1]),comments='')
        np.savetxt(base_path + "linnerud/linnerud.gt", labels, fmt="%.5f")
    return data, labels

if __name__ == "__main__":
    get_iris_dataset()
    get_digits_dataset()
    get_wine_dataset()
    get_breast_cancer_dataset()
    get_california_housing_dataset()
    get_diabetes_dataset()
    get_linnerud_dataset()