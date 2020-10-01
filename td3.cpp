#include <iostream>
#include "td3.hpp"
#include "support.hpp"
#include <stdlib.h>
#include <math.h>       // sin, cos

using namespace std;

using namespace support;

double* extend_array(double* array, int length, int new_size) {
    double* array_new = new double[new_size];
    for (int i = 0; i < length; i++) array_new[i] = array[i];
    delete[] array;
    return array_new;
}

double* shrink_array(double* array, int length, int new_size) {
    double* array_new = new double[new_size];
    for (int i = 0; i < new_size; i++) array_new[i] = array[i];
    delete[] array;
    return array_new;
}

double* append_to_array(double element,
                        double* array,
                        int &current_size,
                        int &max_size) {
    if (current_size < max_size) {
        array[current_size] = element;
        current_size++;
        return array;
    } else {
        max_size += 5;
        array = extend_array(array, current_size, max_size);
        array[current_size] = element;
        current_size++;
        return array;
    }
}

double* remove_from_array(double* array,
                          int &current_size,
                          int &max_size) {
  if (current_size == 0) {
      return array;
  }

  current_size -= 1;
  if (max_size - current_size >= 5) {
      array = shrink_array(array, current_size, current_size);
      max_size = current_size;
  }
  return array;
}

bool simulate_projectile(const double magnitude, const double angle,
                         const double simulation_interval,
                         double *targets, int &tot_targets,
                         int *obstacles, int tot_obstacles,
                         double* &telemetry,
                         int &telemetry_current_size,
                         int &telemetry_max_size) {
  // YOU CAN MODIFY THIS FUNCTION TO RECORD THE TELEMETRY

  bool hit_target, hit_obstacle;
  double v0_x, v0_y, x, y, t;
  double PI = 3.14159265;
  double g = 9.8;

  v0_x = magnitude * cos(angle * PI / 180);
  v0_y = magnitude * sin(angle * PI / 180);

  t = 0;
  x = 0;
  y = 0;

  hit_target = false;
  hit_obstacle = false;
  while (y >= 0 && (! hit_target) && (! hit_obstacle)) {
    telemetry = append_to_array(t, telemetry, telemetry_current_size, telemetry_max_size);
    telemetry = append_to_array(x, telemetry, telemetry_current_size, telemetry_max_size);
    telemetry = append_to_array(y, telemetry, telemetry_current_size, telemetry_max_size);

    double * target_coordinates = find_collision(x, y, targets, tot_targets);
    if (target_coordinates != NULL) {
      remove_target(targets, tot_targets, target_coordinates);
      hit_target = true;
    } else if (find_collision(x, y, obstacles, tot_obstacles) != NULL) {
      hit_obstacle = true;
    } else {
      t = t + simulation_interval;
      y = v0_y * t  - 0.5 * g * t * t;
      x = v0_x * t;
    }
  }

  return hit_target;
}

void merge_telemetry(double **telemetries,
                     int tot_telemetries,
                     int *telemetries_sizes,
                     double* &telemetry,
                     int &telemetry_current_size,
                     int &telemetry_max_size) {
    int i = 0;
    bool appended = true;
    while (appended) {
        appended = false;
        for (int t = 0; t < tot_telemetries; t++) {
            if (i < telemetries_sizes[t]) {
                appended = true;
                for (int j = i; j < i + 3; j++) telemetry = append_to_array(telemetries[t][j], telemetry, telemetry_current_size, telemetry_max_size);
            }
        }
        i+=3;
    }

    // now we sort telemetry

    for (int i = 0; i < telemetry_current_size - 3; i = i + 3) {
        bool swapped = false;
        for (int j = 0; j < telemetry_current_size - 3; j = j + 3) {
            if (telemetry[j] > telemetry[j+3]) {
                swapped = true;
                double tempt = telemetry[j+3];
                double tempx = telemetry[j+4];
                double tempy = telemetry[j+5];

                telemetry[j+3] = telemetry[j];
                telemetry[j+4] = telemetry[j+1];
                telemetry[j+5] = telemetry[j+2];

                telemetry[j] = tempt;
                telemetry[j+1] = tempx;
                telemetry[j+2] = tempy;
            }
        }
        if (!swapped) break;
    }
}
