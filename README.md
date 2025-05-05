# Pizza Shop Simulator

This project is a **Pizza Shop Simulator**, implemented as part of a coursework assignment for the **Operating Systems** class at the **Athens University of Economics and Business**.

### Description

This simulation aims to model the operations of a pizza shop, including order placement, pizza preparation, cooking, packaging, and delivery. The system is built using **multi-threading** in **C** with **pthread** library, simulating multiple customers, cooks, packers, and delivery people interacting with each other in a shared environment.

### Features:

- Simulates customer orders with random pizza types (plain and special).
- Manages different stages of the process: order placement, preparation, baking, packaging, and delivery.
- Uses mutexes and condition variables to synchronize access to shared resources, such as ovens and packers.
- Provides statistics at the end of the simulation, including:
  - Total revenue
  - Number of successful and failed orders
  - Average customer service time
  - Maximum cooling and preparation times

### Project Details:

- **Language**: C
- **Libraries**: pthread (for multithreading), time.h (for time management)
- **Concurrency**: Uses mutexes and condition variables to manage synchronization between different processes (orders, pizza preparation, packing, and delivery).

### How to Run:

1. Clone the repository to your local machine.
2. Compile the code using a C compiler (e.g., `gcc`).
3. Run the program with two arguments:
   - Number of customers
   - Random seed for order generation
