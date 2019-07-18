// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Connections/TCPConnection.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "TCPUnicastConnection.generated.h"

class FSocket;

/**
 * 
 */
UCLASS(Blueprintable,BlueprintType)
class UTCPUnicastConnection : public UTCPConnection
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connection")
		 FString connectToIp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connection")
		 int32 connectToPort;

	// Allow to connect to new connection in case new one tries to connect. Disconnects previous connection
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connection")
		 bool bAllowSwitching = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connection")
		 bool bAllowMatchingOnly=true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connection")
		 bool bMatchIpOnly = true;

private:

	// other endpoint socket
	FSocket* connectedSocket = nullptr;
	FIPv4Endpoint connectedEndPoint;
	FIPv4Endpoint requiredEndPoint;
	// Connection excluder
	FCriticalSection mutex;

	TArray<uint8> dataBuffer;

private:

	void tryAndConnectSocket();

protected:

	virtual bool openConnection() override;

	virtual bool closeConnection() override;

	virtual void clearBuffers() override;

	virtual void receiveData() override;

	virtual void sendData() override;

	virtual void getConfigInternal(void* const config) override;


	virtual void setConfigInternal(void* const config) override;

};
