// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BaseParserProtocol.generated.h"


struct FProcessedData
{
public:

	FProcessedData() {
		timeStamp = FDateTime::UtcNow().ToUnixTimestamp();
	}


	FProcessedData(const FProcessedData& otherData)
	{
		timeStamp = FDateTime::UtcNow().ToUnixTimestamp();
		size = otherData.size;
		data.AddUninitialized(otherData.data.Num());
		FPlatformMemory::Memcpy(data.GetData(), otherData.data.GetData(), otherData.data.Num());
	}


	FProcessedData& operator=(FProcessedData&& otherData)
	{
		size = otherData.size;
		data = MoveTemp(otherData.data);
		timeStamp = otherData.timeStamp;
		return *this;
	}

	FProcessedData(FProcessedData&& otherData)
	{
		size = otherData.size;
		timeStamp = otherData.timeStamp;
		data = MoveTemp(otherData.data);
	}

	int32 size = -1;

	uint64 timeStamp;

	TArray<uint8> data;
};

/**
 * 
 */
UCLASS()
class MESSAGEPROTOCOLS_API UBaseParserProtocol : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		int32 maxAcceptedMsgSize = 2 * 1024 * 1024;

public:

	virtual bool parseMessageFrom(TArray<uint8>& messageToParse, TArray<FProcessedData>& processedMessages);

	virtual bool convertMessageTo(const TArray<uint8>& messageToConvert, TArray<uint8>& convertedMessage);
};
