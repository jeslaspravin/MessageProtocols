// Fill out your copyright notice in the Description page of Project Settings.


#include "DelimitedMessageParser.h"
#include "MessageProtocolPrivatePCH.h"

bool UDelimitedMessageParser::parseMessageFrom(TArray<uint8>& messageToParse, TArray<FProcessedData>& processedMessages)
{

	int32 totalBytesAccepted = 0;
	int32 totalBytesReceived = 0;

	FString message = FString(messageToParse.Num(), FUTF8ToTCHAR((ANSICHAR*)messageToParse.GetData(), messageToParse.Num()).Get());
	
	int index = 0;
	while (true)
	{
		int endIndex = message.Find(delimiter, ESearchCase::CaseSensitive, ESearchDir::FromStart, index);
		if (endIndex != -1)
		{
			int32 messageSize = endIndex - index;
			totalBytesReceived += messageSize;

			if (messageSize > 0 && messageSize <= maxAcceptedMsgSize)
			{
				FProcessedData processingData;
				LOGGER("Received Message!");
				const uint8* messageStart = messageToParse.GetData() + index;
				processingData.size = messageSize;
				processingData.data.AddUninitialized(messageSize);
				FPlatformMemory::Memcpy(processingData.data.GetData(), messageStart, messageSize);
				processedMessages.Add(processingData);
				totalBytesAccepted += messageSize;
			}
			index = endIndex+delimiter.Len();
		}
		else
			break;
	}

	// As long as at least one message received
	if(index !=0)
		// As Index holds the next start index to search for delimiter, which means we are good to remove everything before that
		messageToParse.RemoveAt(0, index);

	LOGGER("Total bytes accepted %d of total bytes received %d", totalBytesAccepted, totalBytesReceived);

	return totalBytesAccepted > 0;
}

bool UDelimitedMessageParser::convertMessageTo(const TArray<uint8>& messageToConvert, TArray<uint8>& convertedMessage)
{
	auto delimiterArr = FTCHARToUTF8(*delimiter);
	convertedMessage.Empty(messageToConvert.Num() + delimiterArr.Length());
	convertedMessage.Append(messageToConvert);
	convertedMessage.Append(TArray<uint8>((uint8*)delimiterArr.Get(), delimiterArr.Length()));
	return true;
}
