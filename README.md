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
