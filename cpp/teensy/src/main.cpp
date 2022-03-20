#include <Arduino.h>
#include "arm_math.h"
#include "stdio.h"
#include "algorithm"
#include "../lib/multilateration/multilateration.h"


int main(void) {
    Serial.begin(9600); 
    while(!Serial){}
    Serial.println("Serial connected"); 

    float32_t** A = init_A_matrix(); 
    float32_t* B = init_B_matrix(); 

    while(true){}
}
