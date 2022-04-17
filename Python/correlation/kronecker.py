import numpy as np
"""
Q
    - decide val of input
        input signals? 
        start with L1 = 25, L2 = 20 (just because it is what they did)
        same for P = 5
    - what is delta (first step in iteration) 

TODO 
- check what features are available in the CMSIS lib befreo you start implementing the alg
- compute y2_underscore as a func of y(k) and h2_underscore
- compute R2_underscore as a func of y(k) and h2_underscore
"""

class Kronecker:

    def __init__(
        self, 
        x: np.array,
        y: np.array
        ):

        self.D = 5 #find acctual number
        self.L1 = 25
        self.L2 = 20
        self.P = 5

        self.h2_u = np.ones([self.P*self.L2, 1])
        self.h1_u = np.ones([self.P*self.L1, 1])
        self.H2 = np.zeros([self.L1*self.L2, self.L1])
        self.H1 = np.zeros([self.L1*self.L2, self.L2])
        self.y2_u = None
        self.y1_u = None
        self.R2 = np.zeros([self.P*self.L1, self.P*self.L1])
        self.R1 = np.zeros([self.P*self.L2, self.P*self.L2])
        self.r2 = np.zeros([self.P*self.L1, self.L1*self.L2])
        self.r1 = np.zeros([self.P*self.L2, self.L1*self.L2])

        self.compute_R2_and_r2(x, y)


    def compute_R2_and_r2(self, x: np.array , y: np.array):

        R2_vals = np.zeros([self.P*self.L1, self.P*self.L1, len(y)])
        r2_vals = np.zeros([self.P*self.L1, self.L1*self.L2, len(y)])

        for k, y_k in enumerate(y):
            self.compute_y2_u(y_k)

            y2_u_trans = np.transpose(self.y2_u)
            R2_vals_k = np.matmul(self.y2_u, y2_u_trans)
            for i in range(self.P*self.L1):
                for j in range(self.P*self.L1):
                    R2_vals[i][j][k] = R2_vals_k[i][j] 

            if k < self.D:
                r2_vals_k = self.y2_u * 0                   #assuming x = 0 for t < 0 
            else:   
                r2_vals_k = self.y2_u * x[k-self.D] 
            for i in range(self.P*self.L1):
                for j in range(self.L1*self.L2):
                    r2_vals[i][j][k] = r2_vals_k[i][j]  
 
        for i in range(self.P*self.L1):
            for j in range(self.P*self.L1):
                self.R2[i][j] = np.mean(R2_vals[i][j])

        for i in range(self.P*self.L1):
            for j in range(self.L1*self.L2):
                self.r2[i][j] = np.mean(r2_vals[i][j])


    def compute_H2(self):
        for i in range (self.L1):
            for j in range (self.L1):
                if i == j:
                    for k in range (self.L2):
                        self.H2[i+ k*self.L1][j] = 1*self.h2_u[k]


    def compute_y2_u(self, y_k: float):
        y2_u_trans = np.zeros([self.L1*self.L2, self.P*self.L1]) 
        self.compute_H2()
        for i in range (self.P):
            for j in range (self.L1*self.L2):
                y2_u_trans[j][i*self.L1:(i+1)*self.L1] = y_k*self.H2[j]
        self.y2_u = np.transpose(y2_u_trans)










    
