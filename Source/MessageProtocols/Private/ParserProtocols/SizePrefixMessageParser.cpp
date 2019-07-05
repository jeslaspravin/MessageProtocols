// Fill out your copyright notice in the Description page of Project Settings.


#include "SizePrefixMessageParser.h"
#include "MessageProtocolPrivatePCH.h"

bool USizePrefixMessageParser::parseMessageFrom(TArray<uint8>& messageToParse, TArray<FProcessedData>& processedMessages)
{
	int32 totalBytesAccepted = 0;
	int32 totalBytesReceived = 0;

	int index = 0;
	while (messageToParse.Num() >= index+4)
	{
		const uint8* startPtr = messageToParse.GetData()+index;
#if PLATFORM_LITTLE_ENDIAN
		int32 messageSize = startPtr[3];
		messageSize = (messageSize << 8) | startPtr[2];
		messageSize = (messageSize << 8) | startPtr[1];
		messageSize = (messageSize << 8) | startPtr[0];
#else
		int32 messageSize = startPtr[0];
		messageSize = (messageSize << 8) | startPtr[1];
		messageSize = (messageSize << 8) | startPtr[2];
		messageSize = (messageSize << 8) | startPtr[3];
#endif
		LOGGER("Message of size %d bytes", messageSize);

		if (messageToParse.Num() >= 4 + index + messageSize)// Size header and Message size
		{
			totalBytesReceived += messageSize;
			if (messageSize > 0 && messageSize <= maxAcceptedMsgSize)
			{
				FProcessedData processingData;
				LOGGER("Received Message!");
				const uint8* messageStart = startPtr + 4;
				processingData.size = messageSize;
				processingData.data.AddUninitialized(messageSize);
				FPlatformMemory::Memcpy(processingData.data.GetData(), messageStart, messageSize);
				processedMessages.Add(processingData);
				totalBytesAccepted += messageSize;
			}
			// Shifting index to next start
			index += 4 + messageSize;
		}
	}
	// As long as at least one message received
	if (index != 0)
		messageToParse.RemoveAt(0, index);
	LOGGER("Total bytes accepted %d of total bytes received %d", totalBytesAccepted, totalBytesReceived);

	return totalBytesAccepted > 0;
}

bool USizePrefixMessageParser::convertMessageTo(const TArray<uint8>& messageToConvert, TArray<uint8>& convertedMessage)
{
	convertedMessage.Empty(messageToConvert.Num() + 4);
	convertedMessage.Append(messageToConvert);
	int32 sizeOfData = messageToConvert.Num();
	convertedMessage.Insert((uint8*)& sizeOfData, 4, 0);
	return true;
}
