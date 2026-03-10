# ml-api
An experimental C++17 API for TSP solvers, data loading, and reusable utilities.

---
**Note on AI Assistance:**
All code in these projects was written almost entirely by me. AI (GitHub Copilot) was used only sparingly for guidance, tough bug fixes, and documentation — not for generating code in large batches.

## Included components
-----------------

- `Demo.cpp` — small example runner that loads data, builds a solver, and prints results.

	Quick build hint (from the `ml-api` directory):

	```bash
	g++ -O3 -march=native -flto -DNDEBUG -ffast-math -funroll-loops Demo.cpp solvers/sa/SA.cpp solvers/TSPSolver.cpp -o bin/Demo -std=gnu++23
	./bin/Demo
	```

	What it demonstrates:
	- reading TSPLIB-style input files into a `DistanceMatrix`
	- configuring a solver with the builder pattern
	- executing and reporting the best tour and timing

- `solvers/` — solver API and algorithms. It provides:
	- `TSPSolver.hpp/.cpp`: base interface, builder pattern, RNG seeding, logging helpers, and a default nearest-neighbour initializer.
	- `sa/`: Simulated Annealing implementation with configurable hooks (accept neighbour, create neighbour, cool temperature, stop condition) and `SA_Config` presets.
	- `ga/` and `aco/`: will be added in the future.

	Extension idea:
	- add a new solver by inheriting `TSPSolver` and implementing all pure virtual functions.
	- expose solver-specific hyperparameters via a config struct and builder method

- `include/` — shared utilities. It provides:
	- `Utils.hpp`: core types (`City`, `Tour`, `Result`), logging helpers, and distance-matrix construction.
	- `ReadData.hpp`: readers for TSPLIB-style coordinate files and edge-weight matrices; uses the header-only CSV library in `include/csv-library/`.

- `data/` — sample TSP datasets for testing and demos.