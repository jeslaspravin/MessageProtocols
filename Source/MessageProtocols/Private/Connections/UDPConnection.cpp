// Fill out your copyright notice in the Description page of Project Settings.


#include "UDPConnection.h"
#include "Common/UdpSocketBuilder.h"
#include "MessageProtocolSettings.h"
#include "MessageProtocolPrivatePCH.h"

void UUDPConnection::recv(const FArrayReaderPtr& arrayReaderPtr, const FIPv4Endpoint& endPt)
{
	if (!bBlockUnknownReceiver || endPt.Address == receiveFromAddress)
	{
		receiverMutex.Lock();
		// Appending to inboxBuffer so that the receiver shall process when it gets time.
		inboxBuffer.Add(*arrayReaderPtr);
		receiverMutex.Unlock();
	}
}

bool UUDPConnection::openConnection()
{
	if (!Super::openConnection())
		return false;

	inboxBuffer.Empty(maxDataToReceive);

	ISocketSubsystem* socketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	// If socket creation failed then return false
	if (!listenAtIp.IsEmpty() && listenAtPort > 0)
	{
		// Creating listener port for receiving messages
		uint8 ipParts[4];
		if (!ipStringToNums(listenAtIp, ipParts))
		{
			LOGGER("Invalid IP(%s) entered!", *listenAtIp);
			udpSocket = nullptr;
			udpReceiver = nullptr;
			bIsReceiving = false;
			return false;
		}

		if(bBlockUnknownReceiver)
		{
			uint8 receiveFromIpParts[4];
			if (!ipStringToNums(receiveFromIp, receiveFromIpParts))
			{
				LOG_WARN("Invalid Receive from IP(%s) entered! Removing receiving constraint", *receiveFromIp);
				bBlockUnknownReceiver = false;
			}
			else
			{
				receiveFromAddress = FIPv4Address(receiveFromIpParts[0], receiveFromIpParts[1], receiveFromIpParts[2], receiveFromIpParts[3]);
			}
		}		

		FIPv4Endpoint endpoint(FIPv4Address(ipParts[0], ipParts[1], ipParts[2], ipParts[3]), listenAtPort);
		udpSocket = FUdpSocketBuilder(*connectionName)
			.AsNonBlocking()
			.AsReusable()
			.BoundToEndpoint(endpoint)
			.WithReceiveBufferSize(bufferSize);

		if (!udpSocket)
		{
			bIsReceiving = false;
			bIsBroadcasting = false;
			return false;
		}
		// Configuring listener thread
		FTimespan threadTiming = FTimespan::FromSeconds(UMessageProtocolSettings::getSleepDuration());
		udpReceiver = new FUdpSocketReceiver(udpSocket, threadTiming, *connectionName);
		udpReceiver->OnDataReceived().BindUObject(this, &UUDPConnection::recv);
		udpReceiver->Start();

		bIsReceiving = true;
	}
	else
	{
		LOG("Creating UDP receiver %s is skipped",*connectionName);
		udpSocket = nullptr;
		udpReceiver = nullptr;
		bIsReceiving = false;
	}

	if (!broadcasterToIp.IsEmpty() && broadcasterToPort > 0)
	{
		bool bIsValidIp;
		// Creating address to which to broadcast targeted messages.
		broadcastToAddress = socketSubsystem->CreateInternetAddr();
		broadcastToAddress->SetIp(*broadcasterToIp, bIsValidIp);

		if (!bIsValidIp)
		{
			LOG("Invalid IP(%s) entered!", *broadcasterToIp);
			broadcastToAddress.Reset();
			bIsBroadcasting = false;
			return false;
		}

		// Create or Set Broadcast mode for sending data
		if (!udpSocket)// Create new socket if not receiving
		{
			udpSocket = FUdpSocketBuilder(*connectionName)
				.AsNonBlocking()
				.AsReusable()
				.WithBroadcast();

			if (!udpSocket)
			{
				bIsReceiving = false;
				bIsBroadcasting = false;
				broadcastToAddress.Reset();
				return false;
			}
		}
		else
		{
			bIsBroadcasting = udpSocket->SetBroadcast(true);
		}
		
		int32 setBufferSize;
		udpSocket->SetSendBufferSize(bufferSize,setBufferSize);
		if (setBufferSize != bufferSize)
		{
			LOG_WARN("Requested buffer size %d but set buffer due to OS limitation is %d", bufferSize, setBufferSize);
		}
	}
	else
	{
		LOG("Creating UDP Broadcaster %s is skipped", *connectionName);
		broadcastToAddress.Reset();
		bIsBroadcasting = false;
	}

	return udpSocket != nullptr;
}

bool UUDPConnection::closeConnection()
{
	flushData();
	ISocketSubsystem* socketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	if (udpSocket)
	{	
		if(udpReceiver)
		{
			udpReceiver->Stop();
			delete udpReceiver;
			udpReceiver = nullptr;
		}

		udpSocket->Close();
		socketSubsystem->DestroySocket(udpSocket);
		udpSocket = nullptr;
	}
	return true;
}

void UUDPConnection::clearBuffers()
{
	inboxBuffer.Empty(maxDataToReceive);
	receiverMutex.Unlock();
	Super::clearBuffers();
}

void UUDPConnection::receiveData()
{
	if (bIsReceiving && receiverMutex.TryLock())
	{
		TArray<uint8> dataBuffer;
		for(int i=0;i<inboxBuffer.Num();i++)
		{
			// Since it is UDP Message, packets are in same form as sent so it is safe to clear buffer after processing.
			dataBuffer.Empty();
			TArray<FProcessedData> processedData = processRecvData(inboxBuffer[i], dataBuffer);
			inboxBuffer.Empty();
			if (processedData.Num() > 0)
			{
				appendToReceived(MoveTemp(processedData));
			}
		}
		receiverMutex.Unlock();
	}
}

void UUDPConnection::sendData()
{
	if (udpSocket && bIsBroadcasting)
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
				udpSocket->SendTo(convertedData.GetData()+currentBytesSent, convertedData.Num()-currentBytesSent, bytesSent,*broadcastToAddress);
				currentBytesSent += bytesSent;
			}
		}
		dataToSend.Empty();
	}
	else if(dataToSend.Num())
	{
		LOG_ERR("No broadcaster available to send the requested data");
	}
}

void UUDPConnection::getConfigInternal(void* const config)
{
	const UUDPConnection* connection = this;
	FUDPConnectionConfig* const configPtr = reinterpret_cast<FUDPConnectionConfig * const>(config);
	configPtr->bBlockUnknownReceiver = connection->bBlockUnknownReceiver;
	configPtr->bufferSize = connection->bufferSize;
	configPtr->broadcasterToIp = connection->broadcasterToIp;
	configPtr->broadcasterToPort = connection->broadcasterToPort;
	configPtr->connectionName = connection->connectionName;
	configPtr->listenAtIp = connection->listenAtIp;
	configPtr->listenAtPort = connection->listenAtPort;
	configPtr->receiveFromIp = connection->receiveFromIp;
}

void UUDPConnection::setConfigInternal(void* const config)
{
	UUDPConnection* connection = this;
	FUDPConnectionConfig* const configPtr = reinterpret_cast<FUDPConnectionConfig * const>(config);
	connection->bBlockUnknownReceiver = configPtr->bBlockUnknownReceiver;
	connection->bufferSize = configPtr->bufferSize;
	connection->broadcasterToIp = configPtr->broadcasterToIp;
	connection->broadcasterToPort = configPtr->broadcasterToPort;
	connection->connectionName = configPtr->connectionName;
	connection->listenAtIp = configPtr->listenAtIp;
	connection->listenAtPort = configPtr->listenAtPort;
	connection->receiveFromIp = configPtr->receiveFromIp;
}
