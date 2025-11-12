# C++ Bayesian Microservice with Docker

This is a high-performance C++ microservice that performs real-time Bayesian updating on streaming data. It's designed to simulate a high-throughput system for tracking supply chain metrics (e.g., carbon emissions) where data is noisy and uncertain. The service is built with the **Crow** C++ micro-framework and is fully containerized with **Docker** using a multi-stage build. 

## Tech Stack

- **C++17**
- **Crow (C++ Framework):** To create the REST API.
- **Bayesian Statistics:** Uses a Normal-Normal conjugate prior.
- **Docker**

## How to Run This Project

**Prerequisites:**

- [Docker Desktop](https://www.docker.com/products/docker-desktop/) must be installed and running.

**Step 1: Build the Docker Image**

From the project's root directory, run the `docker build` command. This will download all dependencies, compile the C++ code, and create a final, optimized image named `bayesian-service`.

```
docker build -t bayesian-service .

```

**Step 2: Run the Container**

This command starts the service and maps your computer's port 8080 to the container's port 8080.

```
docker run -p 8080:8080 bayesian-service

```

You will see a log message from Crow that the server is running on port 8080.

**Step 3: Test the API**

Open a new terminal and use `curl` to interact with your live service.

1. **Check initial state:** (Get the "prior belief" for factory A123)
    
    ```
    curl http://localhost:8080/factory/A123
    
    ```
    
    *Returns: `{"variance":15,"mean_emissions":100,"factory_id":"A123"}`*
    
2. **Send new data:** (Post a new sensor reading)
    
    ```
    curl -X POST -d '{"factory_id": "A123", "reading": 110.0, "sensor_noise": 5.0}' http://localhost:8080/ingest
    
    ```
    
    *Returns: `{"new_variance":3.75,"new_mean":107.5,"status":"updated"}`*
    
3. **Verify the new state:** (Get the "posterior belief")
    
    ```
    curl http://localhost:8080/factory/A123
    
    ```
    
    *Returns: `{"variance":3.75,"mean_emissions":107.5,"factory_id":"A123"}`*

## Key Files in this repo
- 📄 bayesian_api.cpp (Main service code)
- 📄 bayesian.cpp (Simple test script to get the update formula right)
- 📄 crow_all.h (The Crow library file)
- 📄 Dockerfile (The build instructions)
