// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParserProtocols/BaseParserProtocol.h"
#include "IdenticalParser.generated.h"

/**
 * Parses the message as it is provided,no changes
 */
UCLASS(BlueprintType,Blueprintable)
class UIdenticalParser : public UBaseParserProtocol
{
	GENERATED_BODY()
	
};
