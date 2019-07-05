// Fill out your copyright notice in the Description page of Project Settings.


#include "EquiSegmentedMessageParser.h"
#include "MessageProtocolPrivatePCH.h"

bool UEquiSegmentedMessageParser::parseMessageFrom(TArray<uint8>& messageToParse, TArray<FProcessedData>& processedMessages)
{
	int32 totalBytesAccepted = 0;
	int32 totalBytesReceived = 0;

	int index = 0;
	while (messageToParse.Num() >= index + messageSize)
	{
		totalBytesReceived += messageSize;
		if (messageSize <= maxAcceptedMsgSize)
		{
			FProcessedData processingData;
			LOGGER("Received Message!");
			const uint8* startPtr = messageToParse.GetData() + index;
			processingData.size = messageSize;
			processingData.data.AddUninitialized(messageSize);
			FPlatformMemory::Memcpy(processingData.data.GetData(), startPtr, messageSize);
			processedMessages.Add(processingData);
			totalBytesAccepted += messageSize;
		}
		// Shifting index to next start
		index += messageSize;
	}
	// As long as at least one message received
	if (index != 0)
		messageToParse.RemoveAt(0, index);
	LOGGER("Total bytes accepted %d of total bytes received %d", totalBytesAccepted, totalBytesReceived);

	return totalBytesAccepted > 0;
}

bool UEquiSegmentedMessageParser::convertMessageTo(const TArray<uint8>& messageToConvert, TArray<uint8>& convertedMessage)
{
	if (convertedMessage.Num() >= messageSize)
	{
		convertedMessage.Append(messageToConvert.GetData(), messageSize);
		return true;
	}
	return false;
}
