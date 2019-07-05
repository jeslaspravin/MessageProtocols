// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParserProtocols/BaseParserProtocol.h"
#include "EquiSegmentedMessageParser.generated.h"

/**
 * 
 */
UCLASS()
class UEquiSegmentedMessageParser : public UBaseParserProtocol
{
	GENERATED_BODY()

public:

	// Constant Message Size in bytes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parser")
		int32 messageSize=4;
	
public:
	virtual bool parseMessageFrom(TArray<uint8>& messageToParse, TArray<FProcessedData>& processedMessages) override;


	virtual bool convertMessageTo(const TArray<uint8>& messageToConvert, TArray<uint8>& convertedMessage) override;

};
