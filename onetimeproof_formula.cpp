#include <iostream>
#include <vector>
#include <cmath>

// Structure to hold our belief about a factory's carbon emissions
struct FactoryState {
    std::string id;
    double mean_belief;  // Current best guess of emissions (mu_0)
    double variance_belief; // How unsure we are (sigma_0^2)
};

// Structure for incoming sensor data
struct SensorReading {
    std::string factory_id;
    double value;         // The actual reading (x)
    double sensor_noise;  // The variance of the sensor itself (sigma^2)
};

// The Core Bayesian Math Function
// Takes current state and a new reading, returns updated state.
void update_belief(FactoryState& state, const SensorReading& reading) {
    // Standard Bayesian Conjugate Prior (Normal-Normal) update formulas
    // New Mean = ( (mu_0 / sigma_0^2) + (x / sigma^2) ) / ( (1 / sigma_0^2) + (1 / sigma^2) )
    // New Variance = 1 / ( (1 / sigma_0^2) + (1 / sigma^2) )

    double precision_belief = 1.0 / state.variance_belief;
    double precision_sensor = 1.0 / reading.sensor_noise;
    double new_precision = precision_belief + precision_sensor;

    state.variance_belief = 1.0 / new_precision;

    state.mean_belief = ( (precision_belief * state.mean_belief) + (precision_sensor * reading.value) ) / new_precision;
}

int main() {
    // 1. SIMULATION: Initialize a factory with a prior belief
    // "We think Factory A emits 100 tons, but variance is high (15)"
    FactoryState factory_A = {"A123", 100.0, 15.0};

    std::cout << "PRIOR BELIEF -> Mean: " << factory_A.mean_belief
              << ", Variance: " << factory_A.variance_belief << std::endl;

    // 2. INCOMING DATA: A sensor sends a reading
    // "Sensor says 110 tons. This sensor is decent quality (noise variance = 5)"
    SensorReading new_data = {"A123", 110.0, 5.0};

    // 3. UPDATE: Run the Bayesian update
    update_belief(factory_A, new_data);

    std::cout << "POSTERIOR BELIEF -> Mean: " << factory_A.mean_belief
              << ", Variance: " << factory_A.variance_belief << std::endl;

    return 0;
}
