import correlation.kronecker as kr
import numpy as np

def test_init_H2():
    L1 = 3
    L2 = 4
    h = np.array([2,3,4,5])
    H2 = kr.compute_H2(L1, L2, h)

    print("\n", H2)

"""

def test_initialize():
    L1 = 4
    L2 = 5
    P = 2
    y_0 = 6

    h, y = kr.initialize(L1, L2, P, y_0)

    print("\n h: \n", h, "\n y \n", y)
    """




