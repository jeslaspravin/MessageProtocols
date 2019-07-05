// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Connections/BaseConnection.h"
#include "TCPConnection.generated.h"


/**
 * 
 */
UCLASS(Abstract)
class UTCPConnection : public UBaseConnection
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connection")
		FString listenerIp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connection")
		int32 listenerPort;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connection")
		FString listenerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connection")
		int32 receiveBufferSize=2*1024*1024;

protected:
	// your endpoint socket
	FSocket* listenerSocket=nullptr;
	
protected:
	virtual bool openConnection() override;

	virtual bool closeConnection() override;

};
