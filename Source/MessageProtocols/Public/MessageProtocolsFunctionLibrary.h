// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MessageProtocolsFunctionLibrary.generated.h"

class UBaseConnection;

/**
 * 
 */
UCLASS()
class MESSAGEPROTOCOLS_API UMessageProtocolsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Connections")
		static UBaseConnection* createConnection(UClass* templateClass, UObject* ownerObj);

	UFUNCTION(BlueprintCallable, Category = "Connections")
		static UBaseConnection* createAndOpenConnection(UClass* templateClass, UObject* ownerObj);

	UFUNCTION(BlueprintCallable, Category = "Connections")
		static bool openConnection(UBaseConnection* connection);

	UFUNCTION(BlueprintCallable, Category = "Connections")
		static bool closeConnection(UBaseConnection* connection);
};
