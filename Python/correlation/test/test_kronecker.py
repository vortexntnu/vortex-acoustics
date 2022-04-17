import correlation.kronecker as kr
import numpy as np



def test_initialize():
    arr1 = [3.0, 5.2, 5.2, 6.7, 8.9, 2.0, 6.7, 6.7, 5.7, 0.0, 0.0]
    arr2 = [0.0, 0.0, 3.0, 5.2, 5.2, 6.7, 8.9, 2.0, 6.7, 6.7, 5.7]

    kr.Kronecker(arr1, arr2)

    




