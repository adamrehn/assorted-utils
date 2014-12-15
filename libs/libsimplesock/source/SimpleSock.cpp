/*
//  SimpleSock Socket Abstraction Library
//  Copyright (c) 2011-2014, Adam Rehn
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
#include "SimpleSock.h"

#ifndef _WIN32
	#include <sys/time.h>
#endif

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
using std::memset;

void SimpleSock::Init(bool throwExceptions)
{
	#ifdef _WIN32
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			throw SimpleSockException("Error initialising Winsock");
		}
	#endif
	
	SimpleSock::throwExceptions = throwExceptions;
}

void SimpleSock::Cleanup()
{
	#ifdef _WIN32
		WSACleanup();
	#endif
}

int SimpleSock::CreateSocket(int addressFamily, int type, int protocol)
{
	int sock = socket(addressFamily, type, protocol);
	if (sock == -1 && SimpleSock::throwExceptions) {
		throw SimpleSockException("Could not create socket");
	}
	return sock;
}

int SimpleSock::CreateSocketTcp(bool IPV6)
{
	//AF_UNSPEC was not working under Linux
	return SimpleSock::CreateSocket((IPV6) ? AF_INET6 : AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

int SimpleSock::CreateSocketUdp(bool IPV6)
{
	return SimpleSock::CreateSocket((IPV6) ? AF_INET6 : AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

bool SimpleSock::SetBlocking(int sock)
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

bool SimpleSock::SetNonBlocking(int sock)
{
	#ifdef _WIN32
		u_long mode = 1; //Non-blocking
		return (ioctlsocket(sock, FIONBIO, &mode) == 0);
	#else
		//Get the current flags (USE F_GETFL, NOT F_GETFD, THIS SET ME BACK QUITE SOME TIME!)
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

int SimpleSock::PerformNonBlockingReceive(int sock, char* buffer, int bufSize, int& bytesRead)
{
	//Set non-blocking mode
	if (!SimpleSock::SetNonBlocking(sock) && SimpleSock::throwExceptions) {
		throw SimpleSockException("Failed to set non-blocking mode on socket");
	}
	
	//Perform the receive call
	errno = 0;
	bytesRead = recv(sock, buffer, bufSize, 0);
	
	//Set back to blocking mode
	if (!SimpleSock::SetBlocking(sock) && SimpleSock::throwExceptions) {
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

int SimpleSock::PerformNonBlockingReceiveUdp(int sock, char* buffer, int bufSize, int& bytesRead, sockaddr_storage* receivedFrom, socklen_t* receivedFromLength)
{
	//Only UDP sockets should utilise this function
	if (SimpleSock::throwExceptions && SimpleSock::GetSocketType(sock) != SOCK_DGRAM) {
		throw SimpleSockException("Called PerformNonBlockingReceiveUdp with a non-UDP socket");
	}
	
	//Set non-blocking mode
	if (!SimpleSock::SetNonBlocking(sock) && SimpleSock::throwExceptions) {
		throw SimpleSockException("Failed to set non-blocking mode on socket");
	}
	
	//Perform the receive from call
	errno = 0;
	bytesRead = recvfrom(sock, buffer, bufSize, 0, (sockaddr*)receivedFrom, receivedFromLength);
	
	//Set back to blocking mode
	if (!SimpleSock::SetBlocking(sock) && SimpleSock::throwExceptions) {
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

bool SimpleSock::SetReceiveTimeout(int sock, unsigned int seconds, unsigned int microseconds)
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

//Based off the info from <http://beej.us/guide/bgnet/output/html/multipage/getaddrinfoman.html>
bool SimpleSock::Bind(int sock, const char* address, int port)
{
	//Build and populate the structure to be passed to getaddrinfo
	struct addrinfo hints, *results;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SimpleSock::GetSocketType(sock);
	
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
		if (SimpleSock::throwExceptions) {
			throw SimpleSockException("Failed to bind to address and port");
		}
	}
	else
	{
		if (SimpleSock::throwExceptions) {
			throw SimpleSockException("getaddrinfo failed");
		}
	}
	
	return false;
}

//For UDP sockets instead of sendto
int SimpleSock::UdpSendTo(int udpSock, const char* address, int port, const char* buffer, int bufSize, bool IPV6)
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
		if (SimpleSock::throwExceptions) {
			throw SimpleSockException("Failed to send to any of the matches");
		}
	}
	else
	{
		if (SimpleSock::throwExceptions) {
			throw SimpleSockException("getaddrinfo failed");
		}
	}
	
	return -1;
}

//Based off the info from <http://beej.us/guide/bgnet/output/html/multipage/getaddrinfoman.html>
bool SimpleSock::Connect(int sock, const char* address, int port)
{
	//Build and populate the structure to be passed to getaddrinfo
	struct addrinfo hints, *results;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SimpleSock::GetSocketType(sock);
	
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
		if (SimpleSock::throwExceptions) {
			throw SimpleSockException("Failed to connect to address and port");
		}
	}
	else
	{
		if (SimpleSock::throwExceptions) {
			throw SimpleSockException("getaddrinfo failed");
		}
	}
	
	return false;
}

bool SimpleSock::JoinMulticastGroup(int udpSock, const char* address, int port, bool IPV6)
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
}

bool SimpleSock::LeaveMulticastGroup(int udpSock, const char* address, int port, bool IPV6)
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

std::string SimpleSock::GetLocalAddressFromSock(int sock)
{
	//Create a std::string to hold the human-readable result
	std::string localIP = "";
	
	//Create the structure to hold the socket's connection details
	struct sockaddr_storage localDetails;
	memset(&localDetails, 0, sizeof(localDetails));
	socklen_t localDetailsSize = sizeof(localDetails);
	
	//Attempt to populate the details structure
	if (getsockname(sock, (struct sockaddr*)&localDetails, &localDetailsSize) != -1) 
	{
		return SimpleSock::GetPeerAddressFromStruct((struct sockaddr*)&localDetails);
	}
	else
	{
		if (SimpleSock::throwExceptions) {
			throw SimpleSockException("Could not determine local address");
		}
	}
	
	return localIP;
}

int SimpleSock::GetLocalPortFromSock(int sock)
{
	//Create the structure to hold the socket's connection details
	struct sockaddr_storage localDetails;
	memset(&localDetails, 0, sizeof(localDetails));
	socklen_t localDetailsSize = sizeof(localDetails);
	
	//Attempt to populate the details structure
	if (getsockname(sock, (struct sockaddr*)&localDetails, &localDetailsSize) != -1) 
	{
		return SimpleSock::GetPeerPortFromStruct((sockaddr*)&localDetails);
	}
	
	return 0;
}

std::string SimpleSock::GetPeerAddressFromSock(int sock)
{
	//Create a std::string to hold the human-readable result
	std::string peerIP = "";
	
	//Create the structure to hold the socket's connection details
	struct sockaddr_storage peerDetails;
	memset(&peerDetails, 0, sizeof(peerDetails));
	socklen_t peerDetailsSize = sizeof(peerDetails);
	
	//Attempt to populate the details structure
	if (getpeername(sock, (struct sockaddr*)&peerDetails, &peerDetailsSize) != -1) 
	{
		return SimpleSock::GetPeerAddressFromStruct((struct sockaddr*)&peerDetails);
	}
	else
	{
		if (SimpleSock::throwExceptions) {
			throw SimpleSockException("Could not determine peer address");
		}
	}
	
	return peerIP;
}

//Adapted from code from <http://stackoverflow.com/questions/2371910/how-to-get-the-ip-address-and-port-number-from-addrinfo-in-unix-c>
int SimpleSock::GetPeerPortFromSock(int sock)
{
	//Create the structure to hold the socket's connection details
	struct sockaddr_storage peerDetails;
	memset(&peerDetails, 0, sizeof(peerDetails));
	socklen_t peerDetailsSize = sizeof(peerDetails);
	
	//Attempt to populate the details structure
	if (getpeername(sock, (struct sockaddr*)&peerDetails, &peerDetailsSize) != -1) 
	{
		return SimpleSock::GetPeerPortFromStruct((sockaddr*)&peerDetails);
	}
	
	return 0;
}

std::string SimpleSock::GetPeerAddressFromStruct(sockaddr* peerDetails)
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
			if (!SimpleSock::IsIPV6((sockaddr_storage*)peerDetails))
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
			if (SimpleSock::throwExceptions) {
				throw SimpleSockException("Could not determine peer address (WSAAddressToString returned nonzero)");
			}
			*/
			return "0.0.0.0";
		}
	#else
		if (SimpleSock::IsIPV6((sockaddr_storage*)peerDetails))
		{
			remoteIP = inet_ntop(AF_INET6, &(((struct sockaddr_in6*)peerDetails)->sin6_addr), ipTextBuf, INET6_ADDRSTRLEN);
		}
		else
		{
			remoteIP = inet_ntop(AF_INET, &(((struct sockaddr_in*)peerDetails)->sin_addr), ipTextBuf, INET6_ADDRSTRLEN);
		}
	#endif
	
	//Populate the std::string with the contents of the character buffer
	peerIP = std::string(remoteIP);
	return peerIP;
}

int SimpleSock::GetPeerPortFromStruct(sockaddr* peerDetails)
{
	if (SimpleSock::IsIPV6((sockaddr_storage*)peerDetails)) {
		return ntohs(((struct sockaddr_in6*)peerDetails)->sin6_port);
	}
	
	return ntohs(((struct sockaddr_in*)peerDetails)->sin_port);
}

void SimpleSock::CloseSocket(int sock)
{
	#ifdef _WIN32
		closesocket(sock);
	#else
		close(sock);
	#endif
}

#ifndef _WIN32

std::vector<std::string> SimpleSock::GetNetworkInterfaces()
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
			if (currInterface->ifa_addr->sa_family == AF_INET || currInterface->ifa_addr->sa_family == AF_INET6)
			{
				networkInterfaces.push_back( std::string(currInterface->ifa_name) );
			}
			
			currInterface = currInterface->ifa_next;
		}
	}
	
	freeifaddrs(interfaces);
	return networkInterfaces;
}

std::string SimpleSock::GetAddressForInterface(const std::string& interfaceToCheck)
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

//Adapted from code from <http://stackoverflow.com/questions/3217650/how-can-i-find-the-socket-type-from-the-socket-descriptor>
int SimpleSock::GetSocketType(int sock)
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

bool SimpleSock::IsIPV6(sockaddr_storage* details)
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

bool SimpleSock::throwExceptions;
