// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class IMessageProtocolsModule : public IModuleInterface
{
public:

	static IMessageProtocolsModule& get()
	{
		return FModuleManager::GetModuleChecked<IMessageProtocolsModule>("MessageProtocols");
	}

};
