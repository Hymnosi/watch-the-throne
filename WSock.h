#define DEFAULT_PORT "23"
#define DEFAULT_BUFLEN 512


string Global_Result = "";
char Global_Password[32];
char Global_Request[128];

char* SendRequestToConsole(const char* data)
{
	WSADATA wsaData;
	int iResult;
	char* szResponse;
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(iResult != 0)
	{
		printf("WSAStartup Failed: %d\n", iResult);
		return NULL;
	}
	struct addrinfo *result = NULL,
                *ptr = NULL,
                hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	//iResult = getaddrinfo("76.183.8.30", DEFAULT_PORT, &hints, &result);
	iResult = getaddrinfo("216.245.221.50", DEFAULT_PORT, &hints, &result);
	if(iResult != 0)
	{
		printf("WSAStartup Failed: %d\n", iResult);
		WSACleanup();
		return NULL;
	}
	SOCKET ConnectSocket = INVALID_SOCKET;

	ptr = result;

	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return NULL;
	}

	// Connect to server.
	iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	// Should really try the next address returned by getaddrinfo
	// if the connect call failed
	// But for this simple example we just free the resources
	// returned by getaddrinfo and print an error message

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("Unable to connect to server!\n");
		WSACleanup();
		return NULL;
	}
	int recvbuflen = DEFAULT_BUFLEN;

	char recvbuf[DEFAULT_BUFLEN];

	//int iResult;
	
	// Send an initial buffer
	iResult = send(ConnectSocket, data, (int) strlen(data), 0);
	if (iResult == SOCKET_ERROR) 
	{
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return NULL;
	}

	//printf("Bytes Sent: %ld\n", iResult);
	printf("Command \"%s\" requested...\nWaiting for server response.\n", data);
	// shutdown the connection for sending since no more data will be sent
	// the client can still use the ConnectSocket for receiving data
	
	if (iResult == SOCKET_ERROR) 
	{
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return NULL;
	}

	// Receive data until the server closes the connection
	if(!strstr(data, "get motd") && !strstr(data, "/announce"))
	{
		do {
			iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
			if (iResult > 0)
			{
				//printf("Bytes received: %d\n", iResult);
				szResponse = (char*)malloc(sizeof(recvbuf));
				szResponse = recvbuf;
				if(iResult == 2)
				{
					Global_Result = recvbuf[0];
					Global_Result += recvbuf[1];
				}
				else if(iResult == 16)
					Global_Result = "INVALID PASSWORD";
				else
					Global_Result = "ERROR";
			}
			else if (iResult == 0){}
				//printf("Connection closed\n");
			else
				printf("recv failed: %d\n", WSAGetLastError());
		} while (iResult > 0);
	}
	return recvbuf;
}

char* SendRequestToConsole(char* data, char* szPassword)
{
	WSADATA wsaData;
	int iResult;
	char* szResponse;
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(iResult != 0)
	{
		printf("WSAStartup Failed: %d\n", iResult);
		return NULL;
	}
	struct addrinfo *result = NULL,
                *ptr = NULL,
                hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	//iResult = getaddrinfo("76.183.8.30", DEFAULT_PORT, &hints, &result);
	iResult = getaddrinfo("216.245.221.50", DEFAULT_PORT, &hints, &result);
	if(iResult != 0)
	{
		printf("WSAStartup Failed: %d\n", iResult);
		WSACleanup();
		return NULL;
	}
	SOCKET ConnectSocket = INVALID_SOCKET;

	ptr = result;

	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return NULL;
	}

	// Connect to server.
	iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	// Should really try the next address returned by getaddrinfo
	// if the connect call failed
	// But for this simple example we just free the resources
	// returned by getaddrinfo and print an error message

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("Unable to connect to server!\n");
		WSACleanup();
		return NULL;
	}
	int recvbuflen = DEFAULT_BUFLEN;

	char recvbuf[DEFAULT_BUFLEN];

	//int iResult;
	
	// Send an initial buffer
	string NewData = data;
	NewData += "\n";
	NewData += szPassword;
	iResult = send(ConnectSocket, NewData.c_str(), (int) strlen(NewData.c_str()), 0);
	if (iResult == SOCKET_ERROR) 
	{
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return NULL;
	}

	//printf("Bytes Sent: %ld\n", iResult);
	printf("Command \"%s\" requested...\nWaiting for server response.\n", data);
	// shutdown the connection for sending since no more data will be sent
	// the client can still use the ConnectSocket for receiving data
	
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) 
	{
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return NULL;
	}

	// Receive data until the server closes the connection
	do {
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			printf("Bytes received: %d\n", iResult);
			szResponse = (char*)malloc(sizeof(recvbuf));
			if(iResult == 2)
				Global_Result = "OK";
			else if(iResult == 16)
				Global_Result = "INVALID PASSWORD";
			else
				Global_Result = "ERROR";
		}
		else if (iResult == 0){}
			//printf("Connection closed\n");
		else
			printf("recv failed: %d\n", WSAGetLastError());
	} while (iResult > 0);

	return recvbuf;
}

char* SendRequestToConsole(char* data, char* szPassword, bool waitForResponse)
{
	WSADATA wsaData;
	int iResult;
	char* szResponse;
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(iResult != 0)
	{
		printf("WSAStartup Failed: %d\n", iResult);
		return NULL;
	}
	struct addrinfo *result = NULL,
                *ptr = NULL,
                hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	//iResult = getaddrinfo("76.183.8.30", DEFAULT_PORT, &hints, &result);
	iResult = getaddrinfo("216.245.221.50", DEFAULT_PORT, &hints, &result);
	if(iResult != 0)
	{
		printf("WSAStartup Failed: %d\n", iResult);
		WSACleanup();
		return NULL;
	}
	SOCKET ConnectSocket = INVALID_SOCKET;

	ptr = result;

	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return NULL;
	}

	// Connect to server.
	iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	// Should really try the next address returned by getaddrinfo
	// if the connect call failed
	// But for this simple example we just free the resources
	// returned by getaddrinfo and print an error message

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("Unable to connect to server!\n");
		WSACleanup();
		return NULL;
	}
	int recvbuflen = DEFAULT_BUFLEN;

	char recvbuf[DEFAULT_BUFLEN];

	//int iResult;
	
	// Send an initial buffer
	string NewData = data;
	NewData += "\n";
	NewData += szPassword;
	iResult = send(ConnectSocket, NewData.c_str(), (int) strlen(NewData.c_str()), 0);
	if (iResult == SOCKET_ERROR) 
	{
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return NULL;
	}

	//printf("Bytes Sent: %ld\n", iResult);
	printf("Command \"%s\" requested...\nWaiting for server response.\n", data);
	// shutdown the connection for sending since no more data will be sent
	// the client can still use the ConnectSocket for receiving data
	if(waitForResponse == false)
	{
		return NULL;
	}
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) 
	{
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return NULL;
	}

	// Receive data until the server closes the connection
	do {
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			printf("Bytes received: %d\n", iResult);
			szResponse = (char*)malloc(sizeof(recvbuf));
			if(iResult == 2)
				Global_Result = "OK";
			else if(iResult == 16)
				Global_Result = "INVALID PASSWORD";
			else
				Global_Result = "ERROR";
		}
		else if (iResult == 0){}
			//printf("Connection closed\n");
		else
			printf("recv failed: %d\n", WSAGetLastError());
	} while (iResult > 0);

	return recvbuf;
}