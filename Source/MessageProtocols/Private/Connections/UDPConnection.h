// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Connections/BaseConnection.h"
#include "Common/UdpSocketReceiver.h"
#include "UDPConnection.generated.h"

/**
 * 
 */
UCLASS()
class UUDPConnection : public UBaseConnection
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connection")
		 int32 bufferSize = 2 * 1024 * 1024;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connection")
		 FString connectionName = "Unreal UDP Socket";

	// Whether to receive only from provided receive from IP 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Receiver")
		 bool bBlockUnknownReceiver=true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Receiver", meta = (EditCondition = "bBlockUnknownReceiver"))
		 FString receiveFromIp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Listener")
		 FString listenAtIp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Listener")
		 int32 listenAtPort;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sender")
		 FString broadcasterToIp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sender")
		 int32 broadcasterToPort;


private:

	FIPv4Address receiveFromAddress;

	FSocket* udpSocket = nullptr;
	FUdpSocketReceiver* udpReceiver=nullptr;

	TSharedPtr<FInternetAddr> broadcastToAddress = nullptr;

	TArray<TArray<uint8>> inboxBuffer;
	
	FCriticalSection receiverMutex;

	bool bIsBroadcasting = false;
	bool bIsReceiving = false;

private:

	void recv(const FArrayReaderPtr& arrayReaderPtr, const FIPv4Endpoint& endPt);

protected:

	virtual bool openConnection() override;

	virtual bool closeConnection() override;

	virtual void clearBuffers() override;

	virtual void receiveData() override;

	virtual void sendData() override;

	virtual void getConfigInternal(void* const config) override;


	virtual void setConfigInternal(void* const config) override;

};
