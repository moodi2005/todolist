#include <crow.h>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>
#include <nlohmann/json.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

// Use nlohmann::json directly for working with JSON
using namespace std;
using namespace crow;

int main() {
    SimpleApp app;

    app.loglevel(LogLevel::Error);

    // JSON array to store the tasks
    nlohmann::json tasks = nlohmann::json::array();

    // Try loading tasks from a file if it exists
    try {
        filesystem::path current_path = filesystem::current_path();
        ifstream file(current_path / "tasks.db");

        if (file.is_open()) {
            file >> tasks; // Load the tasks JSON array from the file
            file.close();
        }
    }
    catch (const exception &e) {
        cerr << "Failed to load tasks: " << e.what() << endl; // Log error if file loading fails
    }

    // Route to add a new task (POST request)
    app.route_dynamic("/add")
        .methods("POST"_method)([&tasks](const request &req) {
            try {
                // Parse the incoming JSON request body
                nlohmann::json json_data = nlohmann::json::parse(req.body);

                // Ensure that the 'name' key is present in the JSON
                if (!json_data.contains("name")) {
                    return response(400, "Invalid JSON format: missing 'name'");
                }

                // Generate a new unique ID for the task using boost's UUID generator
                boost::uuids::uuid uuid = boost::uuids::random_generator()();

                // Add the new task to the tasks array
                tasks.push_back({{"name", json_data["name"]}, {"done", false}, {"_id", to_string(uuid)}});

                // Save the updated tasks array to the file
                filesystem::path current_path = filesystem::current_path();
                ofstream Wfile(current_path / "tasks.db");
                Wfile << tasks.dump(4) << endl; // Save tasks in a nicely formatted JSON
                Wfile.close();

                return response(200, "Data stored successfully");
            }
            catch (const std::exception &e) {
                return response(500, string("Error: ") + e.what()); // Handle any exceptions
            }
        });

    // Route to fetch all tasks (GET request)
    app.route_dynamic("/tasks").methods("GET"_method)([&tasks](const request &req) {
        return response(200, tasks.dump(4)); // Return tasks as JSON
    });

    // Route to delete a task by ID (DELETE request)
    app.route_dynamic("/task/<string>").methods("DELETE"_method)([&tasks](const request &req, string id) {
        auto finded = find_if(tasks.begin(), tasks.end(), [&id](nlohmann::json &task) { return task["_id"] == id; });

        if (finded != tasks.end()) {
            tasks.erase(finded); // Erase task from the array
            return response(200, "task removed");
        }
        else {
            return response(404, "task not found by _id");
        }
    });

    // Route to update an existing task (POST request)
    app.route_dynamic("/update").methods("POST"_method)([&tasks](const request &req) {
        try {
            // Parse the incoming JSON request body
            nlohmann::json json_data = nlohmann::json::parse(req.body);

            // Ensure that 'name', 'done' and '_id' keys are present in the JSON
            if (!json_data.contains("name") || !json_data.contains("done") || !json_data.contains("_id")) {
                return response(400, "Invalid JSON format: missing 'name', 'done' or '_id'");
            }

            // Find the task by _id
            auto finded = find_if(tasks.begin(), tasks.end(), [&json_data](nlohmann::json &task) {
                return task["_id"] == json_data["_id"];
            });

            if (finded != tasks.end()) {
                (*finded)["name"] = json_data["name"];
                (*finded)["done"] = json_data["done"];

                // Save the updated tasks to the file
                filesystem::path current_path = filesystem::current_path();
                ofstream Wfile(current_path / "tasks.db");
                Wfile << tasks.dump(4) << endl; // Save tasks as formatted JSON
                Wfile.close();

                return response(200, tasks.dump(4));  // Return the updated tasks list
            }
            else {
                return response(404, "Task Not Found By Id");
            }
        }
        catch (const std::exception &e) {
            return response(500, string("Error: ") + e.what()); // Handle any exceptions
        }
    });

    // Route to serve the main HTML file (GET request)
    app.route_dynamic("/").methods("GET"_method)([](const request &req, response &res) {
        filesystem::path current_path = filesystem::current_path();
        filesystem::path file_path = current_path / "./ui/index.html";

        ifstream file(file_path);
        if (file.is_open()) {
            stringstream buffer;
            buffer << file.rdbuf(); // Read the file content into the buffer
            res.code = 200; // Set HTTP status to OK
            res.write(buffer.str()); // Send the file content as response
        }
        else {
            res.code = 404; // Set HTTP status to Not Found if file doesn't exist
            res.write("File not found: "); // Send error message
        }
        res.end(); // End the response
    });

    // Route to serve any other static files (like CSS, JS) in the /ui directory
    CROW_ROUTE(app, "/<string>")
        .methods("GET"_method)([](const request &req, response &res, string fileName) {
            if (fileName == "") fileName = "index.html";

            filesystem::path current_path = filesystem::current_path();
            filesystem::path file_path = current_path / "./ui" / fileName;

            ifstream file(file_path);
            if (file.is_open()) {
                stringstream buffer;
                buffer << file.rdbuf(); // Read the file content into the buffer
                res.code = 200; // Set HTTP status to OK
                res.write(buffer.str()); // Send the file content as response
            }
            else {
                res.code = 404; // Set HTTP status to Not Found if file doesn't exist
                res.write("File not found: "); // Send error message
            }
            res.end(); // End the response
        });

    // Start the app on port 8080 and enable multi-threading
    thread server_thread([&]() {
        app.port(8080).multithreaded().run();
        cout << "App is Rannig";
    });


    #if defined(_WIN32) || defined(_WIN64)
        // For Windows
        system("start http://localhost:8080");
    #elif defined(__linux__)
        // For Linux
        system("xdg-open http://localhost:8080");
    #elif defined(__APPLE__) && defined(__MACH__)
        // For macOS
        system("open http://localhost:8080");
    #else
        // For other systems
        std::cerr << "Unsupported OS, unable to open the browser automatically.\n";
    #endif

    server_thread.join();


    return 0; // End of main function
}
