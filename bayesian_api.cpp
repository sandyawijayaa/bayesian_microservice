#include "crow_all.h" 
#include <iostream> 
#include <vector>
#include <cmath>
#include <unordered_map> // this is dict equiv in python
#include <mutex>

// --- CORE MATH STRUCTS ---
struct FactoryState {
    std::string id;
    double mean_belief;
    double variance_belief;
};

struct SensorReading {
    std::string factory_id;
    double value;
    double sensor_noise;
};

// --- GLOBAL STATE (Simulated Database) ---
// In a real app, this would be a Redis or Postgres DB.
// We use a mutex to make it thread-safe since APIs handle multiple requests at once.
std::unordered_map<std::string, FactoryState> factory_db;
std::mutex db_mutex;

// --- CORE MATH FUNCTION ---
void update_belief(FactoryState& state, const SensorReading& reading) {
    double precision_belief = 1.0 / state.variance_belief;
    double precision_sensor = 1.0 / reading.sensor_noise;
    double new_precision = precision_belief + precision_sensor;

    state.variance_belief = 1.0 / new_precision;
    state.mean_belief = ( (precision_belief * state.mean_belief) + (precision_sensor * reading.value) ) / new_precision;
}

int main() {
    crow::SimpleApp app;

    // SEED DATA: Let's pretend we already know about Factory A
    {
        std::lock_guard<std::mutex> lock(db_mutex);
        factory_db["A123"] = {"A123", 100.0, 15.0};
    }

    // --- ENDPOINT 1: GET current state of a factory ---
    // Example: GET /factory/A123
    CROW_ROUTE(app, "/factory/<string>")([](std::string factory_id){
        std::lock_guard<std::mutex> lock(db_mutex); // Lock access to DB (so threads don't corrupt it)

        if (factory_db.find(factory_id) == factory_db.end()) {
            return crow::response(404, "Factory not found");
        }

        FactoryState& state = factory_db[factory_id];
        crow::json::wvalue json_response;
        json_response["factory_id"] = state.id;
        json_response["mean_emissions"] = state.mean_belief;
        json_response["variance"] = state.variance_belief;

        return crow::response(json_response);
    });

    // --- ENDPOINT 2: POST new sensor data to update belief ---
    // Accepts JSON: { "factory_id": "A123", "reading": 110.0, "sensor_noise": 5.0 }
    CROW_ROUTE(app, "/ingest").methods(crow::HTTPMethod::POST)([](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400, "Invalid JSON");

        std::string f_id = x["factory_id"].s();
        double reading_val = x["reading"].d();
        double noise = x["sensor_noise"].d();

        SensorReading reading = {f_id, reading_val, noise};

        std::lock_guard<std::mutex> lock(db_mutex);

        // If factory is new, initialize it with a default "uninformed" prior
        if (factory_db.find(f_id) == factory_db.end()) {
             factory_db[f_id] = {f_id, 100.0, 100.0}; // High variance = we don't know much
        }

        // Run the math
        update_belief(factory_db[f_id], reading);

        // Return the new state
        crow::json::wvalue json_response;
        json_response["status"] = "updated";
        json_response["new_mean"] = factory_db[f_id].mean_belief;
        json_response["new_variance"] = factory_db[f_id].variance_belief;

        return crow::response(json_response);
    });

    // Run the API on port 8080; port is hardcoded here
    app.port(8080).multithreaded().run();
}