
#include <stdlib.h>
#include <math.h>


//Simulación Sensores




// Acelerometro

float floatrand(float min, float max){
    return min + (float)rand()/(float)(RAND_MAX/(max-min));
}

int int_rand(int lower,int upper){
     int num = (rand() %(upper - lower + 1)) + lower;
     return num;
}


float * acceloremeter_sensor_x(){
    float* arr = malloc(2000 * sizeof(float));
    for(int i = 0; i < 2000; i++){
        arr[i] = 2*sin(2*3.14*0.001*i);
    }
   
    return arr;    
}
float * acceloremeter_sensor_y(){
    float* arr = malloc(2000 * sizeof(float));
    for(int i = 0; i < 2000;i++){
        arr[i] = 2*cos(3*3.14*0.001*i);
    }
       
    return arr;

}
float * acceloremeter_sensor_z(){
    float* arr = malloc(2000 * sizeof(float));
    for(int i = 0; i < 2000;i++){
        arr[i] = 2*sin(10*3.14*0.001*i);
    }   
    return arr; 
}

//THCP_Sensor

char thcp_temp_sensor(){
    char n = (char) (5 + (rand() % 26));
    return n;
}

char thcp_hum_sensor(){
    char n = (char) (30 + (rand() % 51));
    return n;
}

float thcp_pres_sensor(){
    float n = floatrand(1000,1200);
    return n;
}

float thcp_CO_sensor(){
    float n = floatrand(30,2000);
    return n;
}

char batt_sensor() {
   

    char n = (char) (1 + (rand() % 100));
    return n;
}


float accelerometer_kpi_amp_x() {
    return floatrand(0.0059,0.12);
}

float accelerometer_kpi_frec_x() {
    return floatrand(29.0,31.0);
}

float accelerometer_kpi_amp_y() {
    return floatrand(0.0041,0.12);
}

float accelerometer_kpi_frec_y() {
    return floatrand(59.0,61.0);
}

float accelerometer_kpi_amp_z() {
    return floatrand(0.008,0.15);
}

float accelerometer_kpi_frec_z() {
    return floatrand(89.0,91.0);
}


float rms(){
    float sq = pow(accelerometer_kpi_amp_x(),2)+ pow(accelerometer_kpi_amp_y(),2)+pow(accelerometer_kpi_amp_z(),2);
    return pow(sq,0.5); 
}







//Crear paquete







