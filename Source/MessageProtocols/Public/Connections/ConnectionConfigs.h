// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FTCPUnicastConnectionConfig 
{
		FString connectToIp;

		int32 connectToPort;

		FString listenerIp;

		int32 listenerPort;

		FString listenerName;

		int32 receiveBufferSize = 2 * 1024 * 1024;
	// Allow to connect to new connection in case new one tries to connect. Disconnects previous connection
		bool bAllowSwitching = false;

		bool bAllowMatchingOnly = true;

		bool bMatchIpOnly = true;
};

struct FUDPConnectionConfig
{
		int32 bufferSize = 2 * 1024 * 1024;

		FString connectionName = "Unreal UDP Socket";

	// Whether to receive only from provided receive from IP 
		bool bBlockUnknownReceiver = true;

		FString receiveFromIp;

		FString listenAtIp;

		int32 listenAtPort;

		FString broadcasterToIp;

		int32 broadcasterToPort;
};