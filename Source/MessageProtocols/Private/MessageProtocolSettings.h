// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "MessageProtocolSettings.generated.h"

/**
 * 
 */
UCLASS(config = GameUserSettings, defaultconfig, meta = (DisplayName = "Message protocol settings"))
class UMessageProtocolSettings : public UDeveloperSettings
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, config, Category = "Protocol")
		int32 maxFrequency=50;
		
public:

	static float getSleepDuration();
	
};
