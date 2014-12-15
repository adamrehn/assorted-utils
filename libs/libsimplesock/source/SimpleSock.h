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

//When compiling under Windows, be sure to add -lwsock32 -lws2_32
#ifdef _WIN32
	//Under MinGW, the default targeted version of Windows may be too old for WinSock2
	#ifdef _WIN32_WINNT
		#undef _WIN32_WINNT
	#endif
	#ifdef WINVER
		#undef WINVER
	#endif
	#define _WIN32_WINNT 0x0501
	#define WINVER _WIN32_WINNT
	
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

class SimpleSock
{
	public:
		static void Init(bool throwExceptions = true);
		static void Cleanup();
		
		static int CreateSocket(int addressFamily, int type, int protocol);
		static int CreateSocketTcp(bool IPV6 = false);
		static int CreateSocketUdp(bool IPV6 = false);
		
		static void CloseSocket(int sock);
		
		static bool SetBlocking(int sock);
		static bool SetNonBlocking(int sock);
		
		static int PerformNonBlockingReceive(int sock, char* buffer, int bufSize, int& bytesRead);
		static int PerformNonBlockingReceiveUdp(int sock, char* buffer, int bufSize, int& bytesRead, sockaddr_storage* receivedFrom, socklen_t* receivedFromLength);
		
		static bool SetReceiveTimeout(int sock, unsigned int seconds, unsigned int microseconds);
		
		//These are for TCP sockets
		static bool Bind(int sock, const char* address, int port);
		static bool Connect(int sock, const char* address, int port);
		
		//For UDP sockets instead of sendto (resolves remote address for you)
		static int UdpSendTo(int udpSock, const char* address, int port, const char* buffer, int bufSize, bool IPV6);
		
		//Joining and leaving multicast groups
		static bool JoinMulticastGroup(int udpSock, const char* address, int port, bool IPV6 = false);
		static bool LeaveMulticastGroup(int udpSock, const char* address, int port, bool IPV6 = false);
		
		//Getting local info from sockets
		static std::string GetLocalAddressFromSock(int sock);
		static int GetLocalPortFromSock(int sock);
		
		//Getting peer info - from sockets
		static std::string GetPeerAddressFromSock(int sock);
		static int GetPeerPortFromSock(int sock);
		
		//Getting peer info - from sockaddr structs
		static std::string GetPeerAddressFromStruct(sockaddr* peerDetails);
		static int GetPeerPortFromStruct(sockaddr* peerDetails);
		
		#ifndef _WIN32
		
		//Network interface information
		static std::vector<std::string> GetNetworkInterfaces();
		static std::string GetAddressForInterface(const std::string& interfaceToCheck);
		
		#endif
		
	private:
		static bool throwExceptions;
		
		static int GetSocketType(int sock);
		
		static bool IsIPV6(sockaddr_storage* details);
};

#endif
