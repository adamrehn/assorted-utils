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
	//When compiling under Windows, be sure to add -lwsock32 -lws2_32
	#define WINVER 0x0501
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
		
		//These are for TCP sockets
		static bool Bind(int sock, const char* address, int port);
		static bool Connect(int sock, const char* address, int port);
		
		//For UDP sockets instead of sendto (resolves remote address for you)
		static int UdpSendTo(int udpSock, const char* address, int port, const char* buffer, int bufSize, bool IPV6);
		
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
