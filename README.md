# bayesian_microservice
This is a high-performance C++ microservice that uses Bayesian statistics to update beliefs about factory emissions in real-time. It is fully containerized with Docker.
## Prerequisites

You only need one piece of software installed on your machine:

- [Docker Desktop](https://www.docker.com/products/docker-desktop/)

You do **not** need to install C++, Boost, or any C++ libraries locally.

## How to Run

1. **Clone the repository (or download the files):**
    
    ```
    git clone https://github.com/your-username/bayesian-microservice.git
    cd bayesian-microservice
    
    ```
    
2. Build the Docker Image:
    
    This command reads the Dockerfile, downloads all dependencies inside a container, compiles the C++ code, and packages it into a final, lightweight image named bayesian-service.
    
    ```
    docker build -t bayesian-service .
    
    ```
    
3. Run the Container:
    
    This command starts your newly built image and maps your computer's port 8080 to the container's port 8080.
    
    ```
    docker run -p 8080:8080 bayesian-service
    
    ```
    
    You should see the server's log output, indicating it's running.
    

## Test the API

Open a **new** terminal window (leave the server running) and use the following `curl` commands to interact with your service:

1. **Check the initial state of "Factory A123":**
    
    ```
    curl http://localhost:8080/factory/A123
    
    ```
    
    *Expected Response:* `{"variance":15,"mean_emissions":100,"factory_id":"A123"}`
    
2. **Send a new sensor reading to update the belief:**
    
    ```
    curl -X POST -d '{"factory_id": "A123", "reading": 110.0, "sensor_noise": 5.0}' http://localhost:8080/ingest
    
    ```
    
    *Expected Response:* `{"new_variance":3.75,"new_mean":107.5,"status":"updated"}`
    
3. **Check the state again to see the update:**
    
    ```
    curl http://localhost:8080/factory/A123
    
    ```
    
    *Expected Response:* `{"variance":3.75,"mean_emissions":107.5,"factory_id":"A123"}`
    

## Key Files

- `bayesian_api.cpp`: The C++ source code containing all the API logic and Bayesian math.
- `crow_all.h`: The header file for the Crow C++ web framework.
- `Dockerfile`: The blueprint used by Docker to build, compile, and package the application into a portable container.
