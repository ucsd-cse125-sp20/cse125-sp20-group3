# cse125-sp20-group3

 Client compile: g++ client.cpp -o client -lws2_32\
 Running client: ./client localhost (if you are running client and server on your local machine, 
 otherwise use ip address or website link as argument)\
 Refer to [Winsock](https://docs.microsoft.com/en-us/windows/win32/winsock/finished-server-and-client-code)\
 Server compile: g++ server_main.cpp Server.cpp -o server -lws2_32\
 Running server: ./server

# Client

The backbone of the client uses [The Forge](https://github.com/ConfettiFX/The-Forge). To build the project, use Visual Studio 2017/2019 with Windows SDK 10.0.17763.0 installed. I haven't worked out non-IDE build methods as of yet

The-Forge/edit.bat in should open visual studio.

# Notes

Since there are a multitude of file headers, in order to compile Client.h correctly, WIN32_LEAN_AND_MEAN must be defined in the preprocessor. to do this, go to Project Properties => C/C++ => Preprocessor => Definitions => WIN32_LEAN_AND_MEAN for the corresponding configuration.