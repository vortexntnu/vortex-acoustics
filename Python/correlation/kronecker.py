import numpy as np

"""
TODO 
- wired things
    - ignoring the expectance value
    - length of y must be L, but this was not the case in the papaer
    - the result is only dependent on one value of x
- find D
- find delta - levenberg marquat
- test
- port to cpp
"""

class Kronecker:

    def __init__(
        self, 
        x: np.array,
        y: np.array
        ):

        self.L1 = 6
        self.L2 = 7
        self.P = 5
        self.D = 5 
        self.n_iterations = 4
        self.delta = 0.1 

        self.x = x
        self.x_D = x[len(y) - self.D] 
        self.y = y

        assert  len(y) == self.L1*self.L2 

        self.h2_u = np.ones([self.P*self.L2, 1])
        self.h1_u = np.ones([self.P*self.L1, 1])

        self.H2_p = np.zeros([self.L1*self.L2, self.L1])
        self.H1_p = np.zeros([self.L1*self.L2, self.L2])

        self.y2_u = np.zeros([self.P*self.L1, 1])
        self.y1_u = np.zeros([self.P*self.L2, 1])

        self.R2 = np.zeros([self.P*self.L1, self.P*self.L1])
        self.R1 = np.zeros([self.P*self.L2, self.P*self.L2])

        self.r2 = np.zeros([self.P*self.L1, 1])
        self.r1 = np.zeros([self.P*self.L2, 1])

        self.compute_y2_u()
        self.compute_R2()
        self.compute_r2()

    def iteration(self):
        for i in range (self.n_iterations):
            self.compute_h1()
            self.compute_y1_u()
            self.compute_R1()
            self.compute_r1()

            self.compute_h2()
            self.compute_y2_u()
            self.compute_R2()
            self.compute_r2()

    def final_computation(self):
        realtive_impulse_response = np.empty([1, self.L1*self.L2])
        for i in range (self.P):
            realtive_impulse_response_p = np.empty([1, self.L1*self.L2])
            h2_p_trans = np.transpose(self.h2_u[i*self.L2 : (i+1)*self.L2])
            h1_p_trans = np.transpose(self.h1_u[i*self.L1 : (i+1)*self.L1])

            for j, a in enumerate(h2_p_trans):
                realtive_impulse_response_p[j*self.L1 : (j+1)*self.L1] = a*h1_p_trans

            realtive_impulse_response += realtive_impulse_response_p

        sdoa = np.argmax(np.abs(realtive_impulse_response)) - self.D 
        return sdoa    


    def compute_R2(self):
        self.R2 = np.matmul(self.y2_u, np.transpose(self.y2_u))  

    def compute_R1(self):
        self.R1 = np.matmul(self.y1_u, np.transpose(self.y1_u)) 

    def compute_r2(self):
        self.r2 = np.matmul(self.y2_u, self.x_D)

    def compute_r1(self):
        self.r1 = np.matmul(self.y1_u, self.x_D)

    def compute_y2_u(self):
        y2_u_trans = np.zeros([1,self.P*self.L1]) 
        for p in range (self.P):
            self.compute_H2_p(p)
            y2_p = np.matmul(np.transpose(self.H2_p), self.y)
            y2_u_trans[p*self.L1:(p+1)*self.L1] = np.transpose(y2_p)
        self.y2_u = np.transpose(y2_u_trans)

    def compute_y1_u(self):
        y1_u_trans = np.zeros([1,self.P*self.L2]) 
        for p in range (self.P):
            self.compute_H1_p(p)
            y1_p = np.matmul(np.transpose(self.H1_p), self.y)
            y1_u_trans[p*self.L2:(p+1)*self.L2] = np.transpose(y1_p)
        self.y1_u = np.transpose(y1_u_trans)

    def compute_H2_p(self, p):  
        for i in range (self.L1):
            for k in range (0, self.L2, self.L1):
                self.H2_p[i+k][i] = self.h2_u[p*self.L2 + k] 

    def compute_H1_p(self, p): 
        H1_p_trans = np.zeros([self.L2, self.L1*self.L2])
        for i in range (self.L2):
            H1_p_trans[i][i*self.L1: (i+1)*self.L1] = np.transpose(self.h1_u[p*self.L1 : (p+1)*self.L1])    
        self.H1_p = np.transpose(H1_p_trans)

    def compute_h2(self):
        self.h2_u = np.linalg.inv(self.R1 + self.delta*np.identity(self.P*self.L2))

    def compute_h1(self):
        self.h1_u = np.linalg.inv(self.R2 + self.delta*np.identity(self.P*self.L1))











    
