CC = c++ # Replace with your desired C++ compiler
CFLAGS =  -std=c++17  -I/Users/aboudarga/.brew/Cellar/catch2/3.4.0/include \
			-L/Users/aboudarga/.brew/Cellar/catch2/3.4.0/lib -lCatch2 -lCatch2Main \
			-L/Users/aboudarga/.brew/Cellar/curl/8.2.0/lib -lcurl\
			-I/Users/aboudarga/.brew/opt/curl/include

# Add any additional compiler flags you need
# For example, to enable warnings:
# CFLAGS += -Wall -Wextra

# Your target and source file names
TARGET = test_web_server 
SOURCE = test_web_server.cpp uitlsFunction.cpp curlConfig.cpp

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

.PHONY: clean
clean:
	rm -f $(TARGET)
