// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParserProtocols/BaseParserProtocol.h"
#include "SizePrefixMessageParser.generated.h"

/**
 * Messages like <first4bytesize>abc...xyz<nextmsg4bytesize>zyx.... ,Be aware of Endians
 */
UCLASS(BlueprintType, Blueprintable)
class USizePrefixMessageParser : public UBaseParserProtocol
{
	GENERATED_BODY()
	
public:
	virtual bool parseMessageFrom(TArray<uint8>& messageToParse, TArray<FProcessedData>& processedMessages) override;


	virtual bool convertMessageTo(const TArray<uint8>& messageToConvert, TArray<uint8>& convertedMessage) override;

};
