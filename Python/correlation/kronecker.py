import numpy as np

"""
TODO 
- errors: 
    blæææ: R1 must have dim PL1 because you will add it to I with dim (PL1, PL1)
    dimentions of h2_u is wrong it seems
-wired things
    - ignoring the expectance value
    - the result is only dependent on one value of x
- find D
- find delta - levenberg marquat
- test
- port to cpp
- understadn why it works! and why it is better then correlation.
    look at eq 6, 10 and 12
    what is a korn product? what happend with P = 1, and P = Li
"""

class Kronecker:

    def __init__(
        self, 
        x: np.array,
        y: np.array
        ):

        self.L1 = 6
        self.L2 = 7
        self.P = 3
        self.D = 4
        self.n_iterations = 4
        self.delta = 0.1 

        self.x = x
        self.x_D = x[len(y) - self.D] 
        self.y = y

        assert  len(y) == self.L1*self.L2 

        self.h2_u = np.zeros([self.P, self.L2])
        self.h1_u = np.zeros([self.P, self.L1])

        self.H2_p = np.zeros([self.L1*self.L2, self.L1])
        self.H1_p = np.zeros([self.L1*self.L2, self.L2])

        self.y2_u = np.zeros([self.P, self.L1])
        self.y1_u = np.zeros([self.P, self.L2])

        self.R2 = np.zeros([self.P, self.P])
        self.R1 = np.zeros([self.P, self.P])

        self.r2 = np.zeros([self.P, self.L1])
        self.r1 = np.zeros([self.P, self.L1])

        self.initzialization()

    def initzialization(self):
        for p in range (self.P):
            self.h2_u[p][0] = 0.1

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
        realtive_impulse_response = np.empty(self.L1*self.L2)
        for p in range (self.P):
            realtive_impulse_response_p = np.empty(self.L1*self.L2)
            h1_p = np.transpose(self.h1_u[p])
            h2_p = np.transpose(self.h2_u[p])

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
        for p in range(self.P):
            self.compute_H2_p(p)
            y2_p = np.matmul(np.transpose(self.H2_p), self.y)
            self.y2_u[p] = np.transpose(y2_p)

    def compute_y1_u(self):
        for p in range(self.P):
            self.compute_H1_p(p)
            y1_p = np.matmul(np.transpose(self.H1_p), self.y)
            self.y1_u[p] = np.transpose(y1_p)

    def compute_H2_p(self, p):  
        for i in range (self.L1):
            for j in range (0, self.L2):
                self.H2_p[i+j*self.L1][i] = self.h2_u[p][j] 

    def compute_H1_p(self, p):
        for i in range (self.L2):
            for j in range (self.L1):
                self.H1_p[i*self.L1 + j][i] = self.h1_u[p][j]

    def compute_h2(self):
        self.h2_u = np.matmul(np.linalg.inv(self.R1 + self.delta*np.identity(self.P)), self.r1)

    def compute_h1(self):
        self.h1_u = np.matmul(np.linalg.inv(self.R2 + self.delta*np.identity(self.P)), self.r2)











    
