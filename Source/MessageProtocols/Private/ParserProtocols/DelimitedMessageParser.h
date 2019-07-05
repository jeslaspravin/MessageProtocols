// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParserProtocols/BaseParserProtocol.h"
#include "DelimitedMessageParser.generated.h"

/**
 * For messages like "abc....xyz<SomeDelimiter>nextmessage..."
 */
UCLASS(BlueprintType, Blueprintable)
class UDelimitedMessageParser : public UBaseParserProtocol
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		FString delimiter = "==xyz==";

public:
	virtual bool parseMessageFrom(TArray<uint8>& messageToParse, TArray<FProcessedData>& processedMessages) override;


	virtual bool convertMessageTo(const TArray<uint8>& messageToConvert, TArray<uint8>& convertedMessage) override;

};
