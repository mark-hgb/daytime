Simple DAYTIME client/server implementation
===========================================

This is a simple client and server implementation of RFC 867 Daytime Protocol (https://tools.ietf.org/html/rfc867).

# Basics
The implementation is offering a TCP version (daytime_server.c and daytime_client.c). A UDP version (RFC867 is allowing both transport protocols) will be added later.

# Build
Build with the following prequisites is tested on Debian 10.6 and Ubuntu 20.04. Code should be build- and runable out of the box on any modern Linux distribution but not on Windows (the Socket API is slightly different there).

Use the following command to install the prequisites for building:
```
sudo apt install build-essential git
```
Now checkout the Github repository for the project to a local directory on your system:
```
git clone https://github.com/mark-hgb/daytime.git
```
To build the code for server and client just type:
```
make
```
# Usage
Usage help for the server:
```
./daytime_server -h
Usage: ./daytime_server [-i IP] [-p PORT]

```
Without any parameter the server listen on port 13 on any local IP address. Keep in mind that you need Superuser rights to start the server on port 13 (well-known port). You can select any local listen IP with option -i and any local listen port with option -p

Usage help for the client:
```
./daytime_client -h
Usage: ./daytime_client [-i IP] [-p PORT]
```
Without any parameter the client connects to port 13 on localhost (127.0.0.1). You can connect to any server IP with option -i and any server port with option -p.
# Credits
t.b.d.
