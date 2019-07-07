// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPUnicastConnection.h"
#include "Sockets.h"
#include "MessageProtocolPrivatePCH.h"


template<>
void UBaseConnection::getConfig(FTCPUnicastConnectionConfig& config)
{
	const UTCPUnicastConnection* connection = Cast<UTCPUnicastConnection>(this);
	if (connection)
	{
		config.connectToIp = connection->connectToIp;
		config.connectToPort = connection->connectToPort;
		config.listenerIp = connection->listenerIp;
		config.listenerPort = connection->listenerPort;
		config.listenerName = connection->listenerName;
		config.receiveBufferSize = connection->receiveBufferSize;
		config.bAllowSwitching = connection->bAllowSwitching;
		config.bAllowMatchingOnly = connection->bAllowMatchingOnly;
		config.bMatchIpOnly = connection->bMatchIpOnly;
	}
	else
		LOG_ERR("Cannot get config for TCP from non TCP connection");
}

template<>
void UBaseConnection::setConfig(FTCPUnicastConnectionConfig& config)
{
	UTCPUnicastConnection* connection = Cast<UTCPUnicastConnection>(this);
	if (connection)
	{
		connection->connectToIp = config.connectToIp;
		connection->connectToPort = config.connectToPort;
		connection->listenerIp = config.listenerIp;
		connection->listenerPort = config.listenerPort;
		connection->listenerName = config.listenerName;
		connection->receiveBufferSize = config.receiveBufferSize;
		connection->bAllowSwitching = config.bAllowSwitching;
		connection->bAllowMatchingOnly = config.bAllowMatchingOnly;
		connection->bMatchIpOnly = config.bMatchIpOnly;
	}
	else
		LOG_ERR("Cannot set TCP config for non TCP connection");
}

void UTCPUnicastConnection::tryAndConnectSocket()
{
	if (!mutex.TryLock())
		return;
	
	ISocketSubsystem* socketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	bool pending;

	if (listenerSocket->HasPendingConnection(pending) && pending)
	{
		if (connectedSocket && connectedSocket->GetConnectionState() == SCS_Connected && !bAllowSwitching)
		{
			mutex.Unlock();
			return;
		}

		TSharedRef<FInternetAddr> remoteAddress = socketSubsystem->CreateInternetAddr();
		if (connectedSocket)
		{
			connectedSocket->Close();
			socketSubsystem->DestroySocket(connectedSocket);
		}

		TArray<FStringFormatArg> args = { FStringFormatArg(listenerName) };
		connectedSocket = listenerSocket->Accept(*remoteAddress, FString::Format(TEXT("{0} Connected Socket"), args));

		if (connectedSocket != nullptr)
		{
			connectedEndPoint = FIPv4Endpoint(remoteAddress);

			// If allow only matching , check connected end point and required end point 
			if (bAllowMatchingOnly && !(connectedEndPoint == requiredEndPoint || (bMatchIpOnly && connectedEndPoint.Address == requiredEndPoint.Address)))
			{
				connectedSocket->Close();
				socketSubsystem->DestroySocket(connectedSocket);
				connectedSocket = nullptr;
			}
		}
	}
	mutex.Unlock();
}

bool UTCPUnicastConnection::openConnection()
{
	uint8 ipParts[4];
	if (!ipStringToNums(connectToIp, ipParts))
	{
		LOGGER("Invalid IP(%s) entered!", *connectToIp);
		return false;
	}

	bool bSuccess = Super::openConnection();

	if (bSuccess)
	{
		requiredEndPoint = FIPv4Endpoint(FIPv4Address(ipParts[0], ipParts[1], ipParts[2], ipParts[3]), connectToPort);
	}
	return bSuccess;
}

bool UTCPUnicastConnection::closeConnection()
{
	ISocketSubsystem* socketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	if (connectedSocket)
	{
		connectedSocket->Close();
		socketSubsystem->DestroySocket(connectedSocket);
		connectedSocket = nullptr;
	}
	return Super::closeConnection();
}

void UTCPUnicastConnection::clearBuffers()
{
	dataBuffer.Empty();
	mutex.Unlock();
	Super::clearBuffers();
}

void UTCPUnicastConnection::receiveData()
{
	if (!listenerSocket) return;

	if (!connectedSocket)
	{
		tryAndConnectSocket();
	}

	if (connectedSocket)
	{
		TArray<uint8> receivedData;

		uint32 size;
		while (connectedSocket->HasPendingData(size))
		{
			receivedData.Init(0,FMath::Min(size, 65507u));

			int32 readCount = 0;
			connectedSocket->Recv(receivedData.GetData(), receivedData.Num(), readCount);

			if (receivedData.Num() <= 0){
				LOGGER("Received nothing");
				return;
			}
			else {
				receivedData.RemoveAt(readCount, receivedData.Num() - readCount, false);
				TArray<FProcessedData> processedData = processRecvData(receivedData, dataBuffer);

				if (processedData.Num() > 0)
				{
					appendToReceived(MoveTemp(processedData));
				}
			}
		}
	}
}

void UTCPUnicastConnection::sendData()
{

	if (!listenerSocket) return;

	if (!connectedSocket)
	{
		tryAndConnectSocket();
	}

	if (connectedSocket)
	{
		TArray<uint8> convertedData;
		for (int i = 0; i < dataToSend.Num(); i++)
		{
			convertedData.Empty(dataToSend[i].Num());
			processSendDataWithSizePrefix(dataToSend[i], convertedData);
			int32 currentBytesSent = 0;
			while (currentBytesSent < convertedData.Num())
			{
				int32 bytesSent;
				connectedSocket->Send(convertedData.GetData() + currentBytesSent, convertedData.Num() - currentBytesSent, bytesSent);
				currentBytesSent += bytesSent;
			}
		}
		dataToSend.Empty();
	}
}
