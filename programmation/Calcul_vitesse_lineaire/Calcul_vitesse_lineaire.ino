double linear_speed(float v, float wheel_diam, float bracket){
    float wheel_size = 2.54*wheel_diam/100.0; //conversion centimetre --> metre
    float v_rd = v*bracket*3.14/30.0;
    float s = v_rd*wheel_size*3.6; // vitesse du cycliste en km/h
    return s;
}
