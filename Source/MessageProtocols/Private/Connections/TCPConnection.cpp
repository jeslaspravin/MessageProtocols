// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPConnection.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "SocketSubsystem.h"
#include "Common/TcpSocketBuilder.h"
#include "Sockets.h"
#include "MessageProtocolPrivatePCH.h"

bool UTCPConnection::openConnection()
{
	if (!Super::openConnection())
		return false;

	uint8 ipParts[4];
	if (!ipStringToNums(listenerIp, ipParts))
	{
		LOGGER("Invalid IP(%s) entered!",*listenerIp);
		return false;
	}

	FIPv4Endpoint endpoint(FIPv4Address(ipParts[0], ipParts[1], ipParts[2], ipParts[3]), listenerPort);
	listenerSocket = FTcpSocketBuilder(*listenerName)
		.AsReusable()
		.BoundToEndpoint(endpoint)
		.Listening(8);

	if (!listenerSocket)
		return false;

	//Set Buffer Size
	int32 newSize = 0;
	listenerSocket->SetReceiveBufferSize(receiveBufferSize, newSize);
	return true;
}

bool UTCPConnection::closeConnection()
{
	if (listenerSocket)
	{
		listenerSocket->Close();
		ISocketSubsystem* socketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
		socketSubsystem->DestroySocket(listenerSocket);
		listenerSocket = nullptr;
	}
	return Super::closeConnection();
}