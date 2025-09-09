#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "src/maths.h"

int main() {
    double angle = 0;

    while(angle < 2*M_PI) {
        double cos_val = cos(angle);
        double sin_val = sin(angle);

        double my_sin, my_cos;
        sincos(angle, &my_sin, &my_cos);

        printf("Angle: %lf SIN_ERR: %lf COS_ERR: %lf\n", angle, sin_val-my_sin, cos_val-my_cos);
        angle += 0.1;
    }

    return 0;
}