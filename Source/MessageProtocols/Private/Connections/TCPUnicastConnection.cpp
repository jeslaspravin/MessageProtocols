// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPUnicastConnection.h"
#include "Sockets.h"
#include "MessageProtocolPrivatePCH.h"


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
	flushData();
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

void UTCPUnicastConnection::getConfigInternal(void* const config)
{
	FTCPUnicastConnectionConfig* const configPtr = reinterpret_cast<FTCPUnicastConnectionConfig* const>(config);
	const UTCPUnicastConnection* connection = this;
	
	configPtr->connectToIp = connection->connectToIp;
	configPtr->connectToPort = connection->connectToPort;
	configPtr->listenerIp = connection->listenerIp;
	configPtr->listenerPort = connection->listenerPort;
	configPtr->listenerName = connection->listenerName;
	configPtr->receiveBufferSize = connection->receiveBufferSize;
	configPtr->bAllowSwitching = connection->bAllowSwitching;
	configPtr->bAllowMatchingOnly = connection->bAllowMatchingOnly;
	configPtr->bMatchIpOnly = connection->bMatchIpOnly;

}

void UTCPUnicastConnection::setConfigInternal(void* const config)
{
	FTCPUnicastConnectionConfig* const configPtr = reinterpret_cast<FTCPUnicastConnectionConfig * const>(config);
	UTCPUnicastConnection* connection = this;

	connection->connectToIp = configPtr->connectToIp;
	connection->connectToPort = configPtr->connectToPort;
	connection->listenerIp = configPtr->listenerIp;
	connection->listenerPort = configPtr->listenerPort;
	connection->listenerName = configPtr->listenerName;
	connection->receiveBufferSize = configPtr->receiveBufferSize;
	connection->bAllowSwitching = configPtr->bAllowSwitching;
	connection->bAllowMatchingOnly = configPtr->bAllowMatchingOnly;
	connection->bMatchIpOnly = configPtr->bMatchIpOnly;
}
