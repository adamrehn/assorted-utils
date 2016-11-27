/*
//  SimpleSock Socket Abstraction Library
//  Copyright (c) 2011-2016, Adam Rehn
//
//  This library is a conventient little abstraction to help with using
//  BSD sockets under Unix-like OSes and WinSock under Windows.
//
//  ---
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//  
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//  
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.
*/
#ifndef _SIMPLESOCK_H
#define _SIMPLESOCK_H

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <stdexcept>

class SimpleSockException : public std::runtime_error
{
	public:
		SimpleSockException(const std::string& what_arg) : std::runtime_error(what_arg) {}
};

namespace NBReadStatus
{
	const static int ReadSuccessful = 0;
	const static int NoData         = 1;
	const static int Disconnected   = 2;
}

#ifdef _WIN32
	
	//When compiling under MinGW, be sure to add -lwsock32 -lws2_32 to the linker flags
	//Under MSVC, we use pragmas to add the libraries automatically
	#ifdef _MSC_VER
		
		#pragma comment(lib, "wsock32.lib")
		#pragma comment(lib, "ws2_32.lib")
		
	#else
		
		//Under MinGW, the default targeted version of Windows may be too old for WinSock2
		#ifdef _WIN32_WINNT
			#undef _WIN32_WINNT
		#endif
		#ifdef WINVER
			#undef WINVER
		#endif
		#define _WIN32_WINNT 0x0501
		#define WINVER _WIN32_WINNT
		
	#endif
	
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#define socklen_t int
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <ifaddrs.h>
#endif

#ifndef _WIN32
	#include <sys/time.h>
#endif

#ifndef _MSC_VER
	#include <unistd.h>
#endif

#include <fcntl.h>
#include <errno.h>
using std::memset;

template <typename SocketTy>
class SimpleSockImp
{
	public:
		static void Init(bool throwExceptions = true)
		{
			#ifdef _WIN32
				WSADATA wsaData;
				if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
					throw SimpleSockException("Error initialising Winsock");
				}
			#endif
			
			SimpleSockImp<SocketTy>::throwExceptions = throwExceptions;
		}
		
		static void Cleanup()
		{
			#ifdef _WIN32
				WSACleanup();
			#endif
		}
		
		static SocketTy CreateSocket(int addressFamily, int type, int protocol)
		{
			SocketTy sock = socket(addressFamily, type, protocol);
			if (sock == -1 && SimpleSockImp<SocketTy>::throwExceptions) {
				throw SimpleSockException("Could not create socket");
			}
			return sock;
		}
		
		static SocketTy CreateSocketTcp(bool IPV6 = false)
		{
			//AF_UNSPEC was not working under Linux
			return SimpleSockImp<SocketTy>::CreateSocket((IPV6) ? AF_INET6 : AF_INET, SOCK_STREAM, IPPROTO_TCP);
		}
		
		static SocketTy CreateSocketUdp(bool IPV6 = false)
		{
			return SimpleSockImp<SocketTy>::CreateSocket((IPV6) ? AF_INET6 : AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		}
		
		static void CloseSocket(SocketTy sock)
		{
			#ifdef _WIN32
				closesocket(sock);
			#else
				close(sock);
			#endif
		}
		
		static bool SetBlocking(SocketTy sock)
		{
			#ifdef _WIN32
				u_long mode = 0; //Blocking
				return (ioctlsocket(sock, FIONBIO, &mode) == 0);
			#else
				//Get the current flags
				int flags = fcntl(sock, F_GETFL);
				if (flags == -1) {
					return false;
				}
				
				//Set the new flags
				flags = flags & (~O_NONBLOCK);
				if (fcntl(sock, F_SETFL, flags) == -1) {
					return false;
				}
				
				return true;
			#endif
		}
		
		static bool SetNonBlocking(SocketTy sock)
		{
			#ifdef _WIN32
				u_long mode = 1; //Non-blocking
				return (ioctlsocket(sock, FIONBIO, &mode) == 0);
			#else
				//Get the current flags (USE F_GETFL, NOT F_GETFD!)
				int flags = fcntl(sock, F_GETFL);
				if (flags == -1) {
					return false;
				}
				
				//Set the new flags
				flags = flags | O_NONBLOCK;
				if (fcntl(sock, F_SETFL, flags) == -1) {
					return false;
				}
				
				return true;
			#endif
		}
		
		static int PerformNonBlockingReceive(SocketTy sock, char* buffer, int bufSize, int& bytesRead)
		{
			//Set non-blocking mode
			if (!SimpleSockImp<SocketTy>::SetNonBlocking(sock) && SimpleSockImp<SocketTy>::throwExceptions) {
				throw SimpleSockException("Failed to set non-blocking mode on socket");
			}
			
			//Perform the receive call
			errno = 0;
			bytesRead = recv(sock, buffer, bufSize, 0);
			
			//Set back to blocking mode
			if (!SimpleSockImp<SocketTy>::SetBlocking(sock) && SimpleSockImp<SocketTy>::throwExceptions) {
				throw SimpleSockException("Failed to set blocking mode on socket");
			}
			
			//Let the caller know of the outcome
			if (bytesRead > 0) {
				return NBReadStatus::ReadSuccessful;
			}
			else if (bytesRead == 0)
			{
				//The client disconnected cleanly
				return NBReadStatus::Disconnected;
			}
			else
			{
				//Determine the nature of the error
				#ifdef _WIN32
				int errorCode = WSAGetLastError();
				if (errorCode == WSAEWOULDBLOCK || errorCode == 0)
				#else
				if (errno == EAGAIN || errno == EWOULDBLOCK || errno == 0)
				#endif
				{
					//No data was available for reading
					return NBReadStatus::NoData;
				}
				else
				{
					//The connection failed in one way or another
					return NBReadStatus::Disconnected;
				}
			}
		}
		
		static int PerformNonBlockingReceiveUdp(SocketTy sock, char* buffer, int bufSize, int& bytesRead, sockaddr_storage* receivedFrom, socklen_t* receivedFromLength)
		{
			//Only UDP sockets should utilise this function
			if (SimpleSockImp<SocketTy>::throwExceptions && SimpleSockImp<SocketTy>::GetSocketType(sock) != SOCK_DGRAM) {
				throw SimpleSockException("Called PerformNonBlockingReceiveUdp with a non-UDP socket");
			}
			
			//Set non-blocking mode
			if (!SimpleSockImp<SocketTy>::SetNonBlocking(sock) && SimpleSockImp<SocketTy>::throwExceptions) {
				throw SimpleSockException("Failed to set non-blocking mode on socket");
			}
			
			//Perform the receive from call
			errno = 0;
			bytesRead = recvfrom(sock, buffer, bufSize, 0, (sockaddr*)receivedFrom, receivedFromLength);
			
			//Set back to blocking mode
			if (!SimpleSockImp<SocketTy>::SetBlocking(sock) && SimpleSockImp<SocketTy>::throwExceptions) {
				throw SimpleSockException("Failed to set blocking mode on socket");
			}
			
			//Let the caller know of the outcome
			if (bytesRead > 0) {
				return NBReadStatus::ReadSuccessful;
			}
			else if (bytesRead == 0)
			{
				//The client disconnected cleanly
				return NBReadStatus::Disconnected;
			}
			else
			{
				//Determine the nature of the error
				#ifdef _WIN32
				int errorCode = WSAGetLastError();
				if (errorCode == WSAEWOULDBLOCK || errorCode == 0)
				#else
				if (errno == EAGAIN || errno == EWOULDBLOCK || errno == 0)
				#endif
				{
					//No data was available for reading
					return NBReadStatus::NoData;
				}
				else
				{
					//The connection failed in one way or another
					return NBReadStatus::Disconnected;
				}
			}
		}
		
		static bool SetReceiveTimeout(SocketTy sock, unsigned int seconds, unsigned int microseconds)
		{
			#ifdef _WIN32
				
				DWORD milliseconds = (seconds * 1000) + (microseconds / 1000);
				return (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&milliseconds, sizeof(milliseconds)) == 0);
				
			#else
				
				struct timeval tv;
				tv.tv_sec = seconds;
				tv.tv_usec = microseconds; 
				return (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval)) == 0);
				
			#endif
		}
		
		//These are for TCP sockets
		
		//Based off the info from <http://beej.us/guide/bgnet/output/html/multipage/getaddrinfoman.html>
		static bool Bind(SocketTy sock, const char* address, int port)
		{
			//Build and populate the structure to be passed to getaddrinfo
			struct addrinfo hints, *results;
			memset(&hints, 0, sizeof hints);
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SimpleSockImp<SocketTy>::GetSocketType(sock);
			
			//A NULL address means we bind to the local address
			if (address == NULL) {
				hints.ai_flags = AI_PASSIVE;
			}
			
			//Convert the port to a string representation
			char portString[256];
			sprintf(portString, "%d", port);
			
			//Attempt to resolve the address information
			if (getaddrinfo(address, portString, &hints, &results) != -1)
			{
				//Loop through all the results and bind to the first we can
				struct addrinfo *p;
				for (p = results; p != NULL; p = p->ai_next)
				{
					if (bind(sock, p->ai_addr, p->ai_addrlen) != -1)
					{
						freeaddrinfo(results);
						return true;
					}
				}
				
				//If we get to this point, we failed to bind to any of the results
				freeaddrinfo(results);
				if (SimpleSockImp<SocketTy>::throwExceptions) {
					throw SimpleSockException("Failed to bind to address and port");
				}
			}
			else
			{
				if (SimpleSockImp<SocketTy>::throwExceptions) {
					throw SimpleSockException("getaddrinfo failed");
				}
			}
			
			return false;
		}
		
		//Based off the info from <http://beej.us/guide/bgnet/output/html/multipage/getaddrinfoman.html>
		static bool Connect(SocketTy sock, const char* address, int port)
		{
			//Build and populate the structure to be passed to getaddrinfo
			struct addrinfo hints, *results;
			memset(&hints, 0, sizeof hints);
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SimpleSockImp<SocketTy>::GetSocketType(sock);
			
			//Convert the port to a string representation
			char portString[256];
			sprintf(portString, "%d", port);
			
			//Attempt to resolve the address information
			if (getaddrinfo(address, portString, &hints, &results) != -1)
			{
				//Loop through all the results and connect to the first we can
				struct addrinfo *p;
				for (p = results; p != NULL; p = p->ai_next)
				{
					if (connect(sock, p->ai_addr, p->ai_addrlen) != -1)
					{
						freeaddrinfo(results);
						return true;
					}
				}
				
				//If we get to this point, we failed to connect to any of the results
				freeaddrinfo(results);
				if (SimpleSockImp<SocketTy>::throwExceptions) {
					throw SimpleSockException("Failed to connect to address and port");
				}
			}
			else
			{
				if (SimpleSockImp<SocketTy>::throwExceptions) {
					throw SimpleSockException("getaddrinfo failed");
				}
			}
			
			return false;
		}
		
		//For UDP sockets instead of sendto (resolves remote address for you)
		static int UdpSendTo(SocketTy udpSock, const char* address, int port, const char* buffer, int bufSize, bool IPV6)
		{
			//Build and populate the structure to be passed to getaddrinfo
			struct addrinfo hints, *results;
			memset(&hints, 0, sizeof hints);
			hints.ai_family = (IPV6) ? AF_INET6 : AF_INET;
			hints.ai_socktype = SOCK_DGRAM;
			
			//Convert the port to a string representation
			char portString[256];
			sprintf(portString, "%d", port);
			
			//Attempt to resolve the address information
			if (getaddrinfo(address, portString, &hints, &results) != -1)
			{
				//Loop through all the results and send to the first we can
				struct addrinfo *p;
				for (p = results; p != NULL; p = p->ai_next)
				{
					int bytesSent = 0;
					if ((bytesSent = sendto(udpSock, buffer, bufSize, 0, p->ai_addr, p->ai_addrlen)) > 0)
					{
						freeaddrinfo(results);
						return bytesSent;
					}
				}
				
				//If we get to this point, we failed to send to any of the results
				freeaddrinfo(results);
				if (SimpleSockImp<SocketTy>::throwExceptions) {
					throw SimpleSockException("Failed to send to any of the matches");
				}
			}
			else
			{
				if (SimpleSockImp<SocketTy>::throwExceptions) {
					throw SimpleSockException("getaddrinfo failed");
				}
			}
			
			return -1;
		}
		
		//Joining and leaving multicast groups
		static bool JoinMulticastGroup(SocketTy udpSock, const char* address, int port, bool IPV6 = false)
		{
			//Set SO_REUSEADDR for the socket so that other sockets may bind to the same source multicast address
			//(SO_REUSEADDR and SO_REUSEPORT are treated identically for multicast addresses, according to
			//<http://stackoverflow.com/questions/14388706/socket-options-so-reuseaddr-and-so-reuseport-how-do-they-differ-do-they-mean-t/14388707#14388707>
			//so we use SO_REUSEADDR for maximum portability...)
			int reuse = 1;
			#ifdef _WIN32
			setsockopt(udpSock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse));
			#else
			setsockopt(udpSock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
			#endif
			
			//...Except that under Mac OS X, we actually need to set both SO_REUSEADDR and SO_REUSEPORT to 1:
			//<https://github.com/robovm/robovm/issues/77>
			#if defined __APPLE__ && defined __MACH__
			setsockopt(udpSock, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
			#endif
			
			//Bind the socket to the desired port on 0.0.0.0
			struct sockaddr_in localEndpoint;
			localEndpoint.sin_family      = (IPV6) ? AF_INET6 : AF_INET;
			localEndpoint.sin_addr.s_addr = htonl(INADDR_ANY);
			localEndpoint.sin_port        = htons(port);
			if (bind(udpSock, (struct sockaddr*)&localEndpoint, sizeof(localEndpoint)) == -1) {
				return false;
			}
			
			struct ip_mreq mreq;
			mreq.imr_multiaddr.s_addr = inet_addr(address);
			mreq.imr_interface.s_addr = INADDR_ANY;
			
			#ifdef _WIN32
			return (setsockopt(udpSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&mreq, sizeof(mreq)) != -1);
			#else
			return (setsockopt(udpSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) != -1);
			#endif
			
			//Set multicast loopback to enabled, to ensure consistency regardless of the platform default
			unsigned char loop = 1;
			#ifdef _WIN32
			return (setsockopt(udpSock, IPPROTO_IP, IP_MULTICAST_LOOP, (const char*)&loop, sizeof(loop)) != -1);
			#else
			return (setsockopt(udpSock, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)) != -1);
			#endif
		}
		
		static bool LeaveMulticastGroup(SocketTy udpSock, const char* address, int port, bool IPV6 = false)
		{
			struct ip_mreq mreq;
			mreq.imr_multiaddr.s_addr = inet_addr(address);
			mreq.imr_interface.s_addr = INADDR_ANY;
			
			#ifdef _WIN32
			return (setsockopt(udpSock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (const char*)&mreq, sizeof(mreq)) != -1);
			#else
			return (setsockopt(udpSock, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq)) != -1);
			#endif
		}
		
		//Getting local info from sockets
		static std::string GetLocalAddressFromSock(SocketTy sock)
		{
			//Create a std::string to hold the human-readable result
			std::string localIP = "";
			
			//Create the structure to hold the socket's connection details
			struct sockaddr_storage localDetails;
			memset(&localDetails, 0, sizeof(localDetails));
			socklen_t localDetailsSize = sizeof(localDetails);
			
			//Attempt to populate the details structure
			if (getsockname(sock, (struct sockaddr*)&localDetails, &localDetailsSize) != -1) {
				return SimpleSockImp<SocketTy>::GetPeerAddressFromStruct((struct sockaddr*)&localDetails);
			}
			else
			{
				if (SimpleSockImp<SocketTy>::throwExceptions) {
					throw SimpleSockException("Could not determine local address");
				}
			}
			
			return localIP;
		}
		
		static int GetLocalPortFromSock(SocketTy sock)
		{
			//Create the structure to hold the socket's connection details
			struct sockaddr_storage localDetails;
			memset(&localDetails, 0, sizeof(localDetails));
			socklen_t localDetailsSize = sizeof(localDetails);
			
			//Attempt to populate the details structure
			if (getsockname(sock, (struct sockaddr*)&localDetails, &localDetailsSize) != -1) {
				return SimpleSockImp<SocketTy>::GetPeerPortFromStruct((sockaddr*)&localDetails);
			}
			
			return 0;
		}
		
		//Getting peer info - from sockets
		static std::string GetPeerAddressFromSock(SocketTy sock)
		{
			//Create a std::string to hold the human-readable result
			std::string peerIP = "";
			
			//Create the structure to hold the socket's connection details
			struct sockaddr_storage peerDetails;
			memset(&peerDetails, 0, sizeof(peerDetails));
			socklen_t peerDetailsSize = sizeof(peerDetails);
			
			//Attempt to populate the details structure
			if (getpeername(sock, (struct sockaddr*)&peerDetails, &peerDetailsSize) != -1) {
				return SimpleSockImp<SocketTy>::GetPeerAddressFromStruct((struct sockaddr*)&peerDetails);
			}
			else
			{
				if (SimpleSockImp<SocketTy>::throwExceptions) {
					throw SimpleSockException("Could not determine peer address");
				}
			}
			
			return peerIP;
		}
		
		//Adapted from code from <http://stackoverflow.com/questions/2371910/how-to-get-the-ip-address-and-port-number-from-addrinfo-in-unix-c>
		static int GetPeerPortFromSock(SocketTy sock)
		{
			//Create the structure to hold the socket's connection details
			struct sockaddr_storage peerDetails;
			memset(&peerDetails, 0, sizeof(peerDetails));
			socklen_t peerDetailsSize = sizeof(peerDetails);
			
			//Attempt to populate the details structure
			if (getpeername(sock, (struct sockaddr*)&peerDetails, &peerDetailsSize) != -1) {
				return SimpleSockImp<SocketTy>::GetPeerPortFromStruct((sockaddr*)&peerDetails);
			}
			
			return 0;
		}
		
		//Getting peer info - from sockaddr structs
		static std::string GetPeerAddressFromStruct(sockaddr* peerDetails)
		{
			//Create a std::string to hold the human-readable result
			std::string peerIP = "";
			
			char ipTextBuf[INET6_ADDRSTRLEN];
			const char *remoteIP = NULL;
			
			#ifdef _WIN32
				int ipTextBufSize = sizeof(ipTextBuf);
				if (WSAAddressToString((struct sockaddr*)peerDetails, sizeof(*peerDetails), NULL, ipTextBuf, (DWORD*)&ipTextBufSize) == 0)
				{
					remoteIP = ipTextBuf;
					
					//WSAAddressToString generates a string like this - 192.168.1.1:8080
					//We need to strip the port number
					if (!SimpleSockImp<SocketTy>::IsIPV6((sockaddr_storage*)peerDetails))
					{
						std::string generatedIP = std::string(remoteIP);
						size_t colonPos = generatedIP.find(":");
						if (colonPos != std::string::npos) {
							((char*)remoteIP)[colonPos] = 0;
						}
					}
				}
				else
				{
					/*
					if (SimpleSockImp<SocketTy>::throwExceptions) {
						throw SimpleSockException("Could not determine peer address (WSAAddressToString returned nonzero)");
					}
					*/
					return "0.0.0.0";
				}
			#else
				if (SimpleSockImp<SocketTy>::IsIPV6((sockaddr_storage*)peerDetails)) {
					remoteIP = inet_ntop(AF_INET6, &(((struct sockaddr_in6*)peerDetails)->sin6_addr), ipTextBuf, INET6_ADDRSTRLEN);
				}
				else {
					remoteIP = inet_ntop(AF_INET, &(((struct sockaddr_in*)peerDetails)->sin_addr), ipTextBuf, INET6_ADDRSTRLEN);
				}
			#endif
			
			//Populate the std::string with the contents of the character buffer
			peerIP = std::string(remoteIP);
			return peerIP;
		}
		
		static int GetPeerPortFromStruct(sockaddr* peerDetails)
		{
			if (SimpleSockImp<SocketTy>::IsIPV6((sockaddr_storage*)peerDetails)) {
				return ntohs(((struct sockaddr_in6*)peerDetails)->sin6_port);
			}
			
			return ntohs(((struct sockaddr_in*)peerDetails)->sin_port);
		}
		
		#ifndef _WIN32
		
		//Network interface information
		static std::vector<std::string> GetNetworkInterfaces()
		{
			std::vector<std::string> networkInterfaces;
			
			struct ifaddrs *interfaces = NULL;
			if (getifaddrs(&interfaces) == 0)
			{
				//Iterate through the linked list of network interfaces
				struct ifaddrs *currInterface = interfaces;
				while (currInterface != NULL)
				{
					//Only include IPv4 and IPv6 connections
					if (currInterface->ifa_addr->sa_family == AF_INET || currInterface->ifa_addr->sa_family == AF_INET6) {
						networkInterfaces.push_back( std::string(currInterface->ifa_name) );
					}
					
					currInterface = currInterface->ifa_next;
				}
			}
			
			freeifaddrs(interfaces);
			return networkInterfaces;
		}
		
		static std::string GetAddressForInterface(const std::string& interfaceToCheck)
		{
			std::string identifiedAddress = "";
			
			struct ifaddrs *interfaces = NULL;
			if (getifaddrs(&interfaces) == 0)
			{
				//Iterate through the linked list of network interfaces
				struct ifaddrs *currInterface = interfaces;
				while (currInterface != NULL)
				{
					//Only include IPv4 and IPv6 connections
					if (currInterface->ifa_addr->sa_family == AF_INET || currInterface->ifa_addr->sa_family == AF_INET6)
					{
						if (std::string(currInterface->ifa_name) == interfaceToCheck) {
							identifiedAddress = std::string(inet_ntoa(((struct sockaddr_in *)currInterface->ifa_addr)->sin_addr));               
						}
					}
					
					currInterface = currInterface->ifa_next;
				}
			}
			
			freeifaddrs(interfaces);
			return identifiedAddress;
		}
		
		#endif
		
	private:
		static bool throwExceptions;
		
		//Adapted from code from <http://stackoverflow.com/questions/3217650/how-can-i-find-the-socket-type-from-the-socket-descriptor>
		static int GetSocketType(SocketTy sock)
		{
			int type;
			socklen_t length = sizeof(int);
			
			#ifdef _WIN32
			if (getsockopt(sock, SOL_SOCKET, SO_TYPE, (char*)&type, &length) != -1) {
			#else
			if (getsockopt(sock, SOL_SOCKET, SO_TYPE, &type, &length) != -1) {
			#endif
				return type;
			}
			else
			{
				return -1;
			}
		}
		
		static bool IsIPV6(sockaddr_storage* details)
		{
			#ifdef _WIN32
			if (details->ss_family == AF_INET6) {
			#else
			if (((sockaddr*)details)->sa_family == AF_INET6) {
			#endif
				return true;
			}
			
			return false;
		}
};

template <typename SocketTy>
bool SimpleSockImp<SocketTy>::throwExceptions = false;

//Use the SimpleSock typedef instead of using SimpleSockImp directly
typedef SimpleSockImp<int> SimpleSock;

#endif
