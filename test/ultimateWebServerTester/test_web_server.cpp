#include "curlConfig.hpp"
#include <catch2/catch_test_macros.hpp>
#include "uitlsFunction.hpp"
#include <unistd.h>
#include <thread>
#include <chrono>
#include <signal.h> // For kill
#include <sys/types.h> // For pid_t
#include <fcntl.h>
#include <string>

class MockHttpServer {
public:
    std::string command;
    pid_t serverPid;
    std::string workingDir;

    MockHttpServer(std::string& configFile) {
        command = "/Users/aboudarga/1337/webServer_2/build/bin/webserver /tmp/webServer/file.conf";
        workingDir = "/tmp/webServer";

        createDirectory(workingDir);
        createFile("/tmp/webServer/file.conf", configFile);

        // Start the web server process in a new process
        serverPid = fork();
        if (serverPid == 0) {
            // This is the child process
            // Execute the web server command
            // Redirect output to /dev/null to suppress any output
            int devNull = open("webServer.log", O_WRONLY);
            dup2(devNull, STDOUT_FILENO);
            dup2(devNull, STDERR_FILENO);
            close(devNull);
            execl("/bin/sh", "sh", "-c", command.c_str(), nullptr);
            _exit(EXIT_FAILURE); 
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    ~MockHttpServer() {
        stopWebServer();
        // removeDirectory(workingDir);
    }

    void stopWebServer() {
        // Send a shutdown request to the web server process
        if (serverPid > 0) {
            kill(serverPid, SIGTERM); // Send the SIGTERM signal to gracefully terminate the process
            int status;
            waitpid(serverPid, &status, 0); // Wait for the child process to exit
            serverPid = 0; // Reset the serverPid to indicate that the server has been stopped
        }
    }
};

TEST_CASE("Test HTTP GET Request", "[http]") {
    std::string configFile = R"(
        server {
            listen 8080;
            host 127.0.0.1;

            location /{
                root /tmp/webServer;
                index index.html index.php;
            }
            location /indexNotFound{
                root /tmp/webServer/indexNotFound;
                index index.html;
            }
            location /tryindexOff{
                root /tmp/webServer/indexOff;
            }
            location /autoIndexingOn{
                root /tmp/webServer/listIndex;
                index index.html;
                autoindex on;
            }
            location /autoIndexingOff{
                root /tmp/webServer/listIndex;
                index index.html;
                autoindex off;
            }

            location .php$ {
            #fastcgi_pass unix:/var/run/php/php{version}-fpm.sock;
            #fastcgi_index index.php;
            #include fastcgi_params;
            #fastcgi_param SCRIPT_FILENAME $document_root$fastcgi_script_name;
            }
        }
    )";
    MockHttpServer mock(configFile);

    SECTION("Test successful GET request") {
        std::string url = "http://localhost:8080";
        std::string expectedContent = R"(
            <html>
                <body>
                    <h1>Hello, World! this index</h1>
                </body>
            </html>
        )";
        std::string expectedFilePath = "/tmp/webServer/index.html";

        createFile(expectedFilePath, expectedContent);
        ResponseData response = PerformHttpGet(url);
        REQUIRE_FALSE(response.body.empty());

        // std::string expectedContent = ReadFileContent(expectedFilePath);

        // Compare the response with the content of the expected file
        REQUIRE(response.body == expectedContent);
    }

    SECTION("Test index not found") {
        std::string url = "http://localhost:8080/indexNotFound/";
        long expectedRespondCode = 403;
        createDirectory("/tmp/webServer/indexNotFound");
        ResponseData response = PerformHttpGet(url);

        REQUIRE_FALSE(response.body.empty());
        REQUIRE(response.code == expectedRespondCode);
        // removeDirectory("/tmp/webServer/indexNOtFound");
    }

    SECTION("Test index off"){
        std::string url = "http://localhost:8080/tryindexOff/";
        long expectedRespondCode = 403;

        createDirectory("/tmp/webServer/indexOff");
        ResponseData response = PerformHttpGet(url);
        REQUIRE_FALSE(response.body.empty());
        REQUIRE(response.code == expectedRespondCode);
        // removeDirectory("/tmp/webServer/indexOff");
    }

    // SECTION("Test invalid URL") {
    //     std::string url = "http://localhost:8055";

    //     ResponseData response = PerformHttpGet(url);
    //     REQUIRE_FALSE(response.body.empty());
    // }

    

  

    SECTION("Test autoindex on") {
        std::string url = "http://localhost:8080/autoIndexingOn/";
        std::string expectedAutoIndexContent = "fileA fileB fileC fileD"; // Replace with expected autoindex content

        // Create an autoindex page in the expected directory
        createDirectory("/tmp/webServer/listIndex");
        createFile("/tmp/webServer/listIndex/fileA", "this file A");
        createFile("/tmp/webServer/listIndex/fileB", "this file B");
        createFile("/tmp/webServer/listIndex/fileC", "this file C");
        createFile("/tmp/webServer/listIndex/fileD", "this file D");

        // Test with autoindex on
        ResponseData response = PerformHttpGet(url);
        REQUIRE_FALSE(response.body.empty());
        REQUIRE(response.body.find("fileA") != std::string::npos);
        REQUIRE(response.body.find("fileB") != std::string::npos);
        REQUIRE(response.body.find("fileC") != std::string::npos);
        REQUIRE(response.body.find("fileD") != std::string::npos);


        // removeDirectory("/tmp/webServer/listIndex");
    }
   
     SECTION("Test autoindex off") {
        std::string url = "http://localhost:8080/autoIndexingOff/";
        int expectedCode = 403;
        std::string expectedBody = R"(<!DOCTYPE html><html><head><title>403 Forbidden</title></head><body><h1><center>403 Forbidden<hr></center></h1><p><center>webServer/0.01</center></p></body></html>)";

        createDirectory("/tmp/webServer/listIndex");
        // Test with autoindex off
        ResponseData response = PerformHttpGet(url);
        REQUIRE_FALSE(response.body.empty());
        REQUIRE(response.body == expectedBody);
        REQUIRE(response.code == expectedCode);

        // removeDirectory("/tmp/webServer/listIndex");
    }
    // Add more test cases as needed...

      SECTION("Test large response") {
        std::string url = "http://localhost:8080/largeResponse.html";
        std::string expectedContent = ""; // Initialize expectedContent as an empty string

        // Generate a large response (e.g., 10 MB)
        const int megabytes = 10;
        const int bufferSize = 1024 * 1024; // 1 MB buffer size
        std::string largeContent;
        largeContent.reserve(megabytes * bufferSize);
        for (int i = 0; i < megabytes; ++i) {
            largeContent += std::string(bufferSize, 'A');
        }

        createFile("/tmp/webServer/largeResponse.html", largeContent);

        // Test large response
        ResponseData response = PerformHttpGet(url);
        REQUIRE_FALSE(response.body.empty());
        REQUIRE(response.body == largeContent);

        // removeFile("/tmp/webServer/largeResponse.html");
    }

    SECTION("Test HTML calling CSS files") {
        std::string url = "http://localhost:8080/cssFiles/index.html";
        std::string expectedHtmlContent = R"(
            <!DOCTYPE html>
            <html>
            <head>
                <link rel="stylesheet" href="style1.css">
                <link rel="stylesheet" href="style2.css">
            </head>
            <body>
                <h1>Hello, World!</h1>
            </body>
            </html>
        )";

        std::string expectedStyle1Content = R"(
            h1 {
                color: red;
            }
        )";

        std::string expectedStyle2Content = R"(
            body {
                background-color: lightblue;
            }
        )";

        // Create the HTML file and CSS files
        createDirectory("/tmp/webServer/cssFiles");
        createFile("/tmp/webServer/cssFiles/index.html", expectedHtmlContent);
        createFile("/tmp/webServer/cssFiles/style1.css", expectedStyle1Content);
        createFile("/tmp/webServer/cssFiles/style2.css", expectedStyle2Content);

        // Test HTML calling CSS files
        ResponseData response = PerformHttpGet(url);
        REQUIRE_FALSE(response.body.empty());
        REQUIRE(response.body == expectedHtmlContent);

        // removeFile("/tmp/webServer/cssFiles/index.html");
        // removeFile("/tmp/webServer/cssFiles/style1.css");
        // removeFile("/tmp/webServer/cssFiles/style2.css");
    }

    SECTION("Test non-existent URL") {
    std::string url = "http://localhost:8080/non_existent_page";

    ResponseData response = PerformHttpGet(url);
    REQUIRE(response.code == 404); // Expecting a Not Found status code
    }

    SECTION("Test server concurrency and performance") {
    // Perform multiple HTTP GET requests in parallel using multiple threads
    const int numThreads = 10;
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
            threads.emplace_back([&]() {
                // Make a GET request to the server
                std::string url = "http://localhost:8080/";
                ResponseData response = PerformHttpGet(url);
                REQUIRE_FALSE(response.body.empty());
    });
    }

    // Wait for all threads to finish
    for (auto& thread : threads) {
            thread.join();
    }
    }

    // removeDirectory(mock.workingDir);
    
}
