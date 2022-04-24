import numpy as np

"""
TODO 
- wired things
    - h, r are periodic after n iterations. I dont see how they can become non periodic. 
- test
- port to cpp
"""

class Kronecker:

    def __init__(
        self, 
        x: np.array,
        y: np.array
        ):

        self.P = 5
        self.D = 3 
        self.n_iterations = 10
        self.regulation_constant = 0.1  

        self.L1, self.L2 = compute_L1_L2(len(y))
        assert self.P <= self.L2 <= self.L1

        self.y = y[0:self.L1*self.L2]
        self.x_D = x[len(self.y) - self.D]
        assert self.x_D != 0

        self.h2_u = np.zeros([self.P*self.L2, 1])
        self.h1_u = np.zeros([self.P*self.L1, 1])

        self.H2_p = np.zeros([self.L1*self.L2, self.L1])
        self.H1_p = np.zeros([self.L1*self.L2, self.L2]) 

        self.y2_u = np.zeros([self.P*self.L1, 1])
        self.y1_u = np.zeros([self.P*self.L2, 1])

        self.R2 = np.zeros([self.P*self.L1, self.P*self.L1])
        self.R1 = np.zeros([self.P*self.L2, self.P*self.L2])

        self.r2 = np.zeros([self.P*self.L1, 1])
        self.r1 = np.zeros([self.P*self.L2, 1])

        self.initialize()


    def initialize(self):
        for i in range (self.P):
            self.h2_u[i*self.L2] = 0.1 #init value must be between 0 and 1

        self.compute_y2_u()
        self.compute_R2()
        self.compute_r2()


    def iteration(self):
        for n in range (self.n_iterations):
            
            self.compute_h1()
            self.compute_y1_u()
            self.compute_R1()
            self.compute_r1()

            self.compute_h2()
            self.compute_y2_u()
            self.compute_R2()
            self.compute_r2() 

        #print("R2 : \n", self.R2)
        #print("R1 : \n", self.R1)
        #print("r2 : \n", self.r2)
        #print("r1 : \n", self.r1)
        #print("h2 : \n", self.h2_u)
        #print("h1 : \n", self.h1_u)

    def final_computation(self): 
        realtive_impulse_response = np.zeros([self.L1*self.L2, 1])
        for p in range (self.P):
            realtive_impulse_response_p = np.zeros([self.L1*self.L2, 1])
            h2_p = self.h2_u[p*self.L2 : (p+1)*self.L2]
            h1_p = self.h1_u[p*self.L1 : (p+1)*self.L1]

            for j, a in enumerate(h2_p):
                realtive_impulse_response_p[j*self.L1 : (j+1)*self.L1] = a*h1_p

            realtive_impulse_response += realtive_impulse_response_p

        sdoa = np.argmax(np.abs(realtive_impulse_response)) - self.D 
        return sdoa       

    def compute_R2(self):
        self.R2 = np.matmul(self.y2_u, np.transpose(self.y2_u))  

    def compute_R1(self):
        self.R1 = np.matmul(self.y1_u, np.transpose(self.y1_u)) 

    def compute_r2(self):
        self.r2 = self.y2_u*self.x_D

    def compute_r1(self):
        self.r1 = self.y1_u*self.x_D

    def compute_y2_u(self):
        for p in range (self.P):
            self.compute_H2_p(p)
            y2_p = np.matmul(np.transpose(self.H2_p), self.y)
            y2_p_mat = np.reshape(y2_p, (self.L1, 1))
            self.y2_u[p*self.L1:(p+1)*self.L1] = y2_p_mat

    def compute_y1_u(self):
        for p in range (self.P):
            self.compute_H1_p(p)
            y1_p = np.matmul(np.transpose(self.H1_p), self.y)
            y1_p_mat = np.reshape(y1_p, (self.L2, 1))
            self.y1_u[p*self.L2:(p+1)*self.L2] = y1_p_mat

    def compute_H2_p(self, p):  
        for k in range (self.L1):
            for i in range (self.L2):
                self.H2_p[i*self.L1 +k][k] = self.h2_u[p*self.L2 + i] 


    def compute_H1_p(self, p):
        for i in range (self.L2):
            for j in range (self.L1):
                self.H1_p[i*self.L1 + j][i] = self.h1_u[p*self.L1 + j]

    def compute_h2(self):
        self.h2_u = np.matmul(np.linalg.inv(self.R1 + self.regulation_constant*np.identity(self.P*self.L2)), self.r1)

    def compute_h1(self):
        self.h1_u = np.matmul(np.linalg.inv(self.R2 + self.regulation_constant*np.identity(self.P*self.L1)), self.r2)

def compute_L1_L2(L):
    """
    Requirements:
     - L2 < L1 
     - L1 * L2 = L 
     - L1 and L2 should be as close as possible
     - P << L2
    """
    L2 = 1
    while (True):
        L2 += 1
        L1 = L // L2
        if (L2 + 1) >= L1: 
            break

    return L1, L2











    
