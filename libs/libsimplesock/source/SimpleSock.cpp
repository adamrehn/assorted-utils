/*
//  SimpleSock Socket Abstraction Library
//  Copyright (c) 2011-2013, Adam Rehn
//
//  This library is a conventient little abstraction to help with using
//  BSD sockets under Unix-like OSes and WinSock under Windows.
//
//  ---
//
//  This file is part of SimpleSock.
//
//  SimpleSock is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with SimpleSock. If not, see <http://www.gnu.org/licenses/>.
*/
#include "SimpleSock.h"

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