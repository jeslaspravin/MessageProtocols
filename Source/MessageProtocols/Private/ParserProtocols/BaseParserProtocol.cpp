// Fill out your copyright notice in the Description page of Project Settings.


#include "ParserProtocols/BaseParserProtocol.h"

bool UBaseParserProtocol::parseMessageFrom(TArray<uint8>& messageToParse, TArray<FProcessedData>& processedMessages)
{
	if(messageToParse.Num()<= maxAcceptedMsgSize)
	{
		FProcessedData processedData;
		processedData.data.Append(messageToParse);
		return true;
	}
	return false;
}

bool UBaseParserProtocol::convertMessageTo(const TArray<uint8>& messageToConvert, TArray<uint8>& convertedMessage)
{
	convertedMessage.Append(messageToConvert);
	return true;
}