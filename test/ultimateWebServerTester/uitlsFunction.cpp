#include "uitlsFunction.hpp"

#include <filesystem>
#include <iostream>

bool createFile(std::string fileName, std::string content) {
    std::ofstream outputFile(fileName);  // Create or open the file

    if (!outputFile) {
        FAIL("Error creating the file!" << fileName);
    }

    outputFile << content;

    outputFile.close();  // Close the file after writing
    return (true);
}

bool createDirectory(std::string dirPath) {
    // Check if the directory already exists
    if (fs::exists(dirPath)) {
        CAPTURE("Error creating the file!");
        return (true);
    }

    // Create the directory
    try {
        fs::create_directory(dirPath);
        return (true);
    } catch (const std::filesystem::filesystem_error &e) {
        FAIL("Error creating the directory: " << e.what());
    }
    return (true);
}
std::string ReadFileContent(const std::string &filepath) {
    std::ifstream file(filepath);
    if (!file) {
        FAIL("Failed to open file: " << filepath);
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool removeDirectory(std::string directoryPath) {
    try {
        // Use std::filesystem::remove_all to recursively remove the directory
        // and its contents
        std::filesystem::remove_all(directoryPath);
        // //std::cout << "Directory removed: " << directoryPath << std::endl;
        return true;
    } catch (const std::exception &ex) {
        std::cerr << "Failed to remove directory: " << directoryPath
                  << std::endl;
        std::cerr << "Error: " << ex.what() << std::endl;
        return false;
    }
}

bool removeFile(const std::string &filePath) {
    if (std::remove(filePath.c_str()) == 0) {
        // File removal successful
        return true;
    } else {
        // File removal failed
        return false;
    }
}