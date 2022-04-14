import numpy as np
"""
- check what features are available in the CMSIS lib befreo you start implementing the alg

- questions to implementation
    - decide length of input signals
        assumeing the length of x is equal the lengtth of y and h
        start with L1 = 25, L2 = 20 (just because it is what they did)
        same for P = 5
    - how to init h2_0?
        -assume we can init with ones - check if this makes sense
        -assume h underscore 4, means the sum of h from 0 to 4. (ref equation 10)
    - what is delta (first step in iteration) 

TODO 
- lage en classe yeees
- compute y2_underscore as a func of y(k) and h2_underscore
- compute R2_underscore as a func of y(k) and h2_underscore
"""

D = 5 #find acctual number

def compute_H2(L1, L2, h):
    H2 = np.zeros([L1*L2, L1])
    for i in range (L1):
        for j in range (L1):
            if i == j:
                for k in range (L2):
                    H2[i+ k*L1][j] = 1*h[k]

    return H2

def compute_y2_underscore(L1, L2, P, y, h):
    y2_underscore_trans = np.zeros([L1*L2, P*L1]) 
    H2 = compute_H2(L1, L2, h)
    for i in range (P):
        for j in range (L1*L2):
            y2_underscore_trans[j][i*L1:(i+1)*L1] = y*H2[j]
    y2_underscore = np.transpose(y2_underscore_trans)

    return y2_underscore

def compute_R2_and_r2(L1, L2, P, h, y, x):

    R2_vals = np.zeros([P*L1, P*L1, len(y)])
    r2_vals = np.zeros(P*L1, len(y))

    for k, y_k in enumerate(y):
        y2_underscore = compute_y2_underscore(L1, L2, P, y_k, h)

        y2_underscore_trans = np.transpose(y2_underscore)
        R2_vals_k = np.matmul(y2_underscore, y2_underscore_trans)
        for i in range(P*L1):
            for j in range(P*L1):
                R2_vals[i][j][k] = R2_vals_k[i][j] 

        r2_vals[k] =  np.matmul(y2_underscore, x[k-D]) #hmmm

    R2 = np.zeros([P*L1, P*L1])
    for i in range(P*L1):
        for j in range(P*L1):
            R2[i][j] = np.mean(R2_vals[i][j])

    r2 = np.zeros(P*L1)
    for i in range(P*L1):
        r2[i] = np.mean(r2_vals)

    return R2, r2



def initialize(L1, L2, P, y, x):
    h2_underscore = np.ones([P*L1, 1])
    R2, r2= compute_R2_and_r2(L1, L2, P, h2_underscore, y, x)





    
