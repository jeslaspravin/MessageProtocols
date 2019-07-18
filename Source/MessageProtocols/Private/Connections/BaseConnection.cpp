// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseConnection.h"
#include "Sockets.h"
#include "JsonObjectConverter.h"
#include "IMessageProtocols.h"
#include "MessageProtocols.h"
#include "MessageProtocolPrivatePCH.h"

bool UBaseConnection::openConnection()
{
	if (!parser)
	{
		LOG_ERR("Parser not available,Cannot open connection");
		return false;
	}
	rawDataReceived.Empty(maxDataToReceive);
	return true;
}

void UBaseConnection::clearBuffers()
{
	dataToSend.Empty(maxDataToSend);
	rawDataReceived.Empty(maxDataToReceive);
}

void UBaseConnection::getConfigInternal(void* const config)
{
	unimplemented()
}

void UBaseConnection::setConfigInternal(void* const config)
{
	unimplemented()
}

void UBaseConnection::appendToReceived(TArray<FProcessedData>&& data)
{
	rawDataReceived.Append(data);
	validateAndSortReceived();
}

void UBaseConnection::appendToReceived(TArray<FProcessedData>& data)
{
	rawDataReceived.Append(data);
	validateAndSortReceived();
}

void UBaseConnection::appendToReceived(FProcessedData& data)
{
	rawDataReceived.Add(data);
	validateAndSortReceived();
}

void UBaseConnection::appendToReceived(FProcessedData&& data)
{
	rawDataReceived.Add(data);
	validateAndSortReceived();
}

void UBaseConnection::validateAndSortReceived()
{
	if (rawDataReceived.Num() > maxDataToReceive)
	{
		rawDataReceived.RemoveAtSwap(0, maxDataToReceive - rawDataReceived.Num(), false);
		// Reorder in older to newer message order
		rawDataReceived.Sort([](const FProcessedData& lData, const FProcessedData& rData) {
			return lData.timeStamp < rData.timeStamp;
		});

	}
}

TArray<FProcessedData> UBaseConnection::processRecvData(const TArray<uint8>& receivedData, TArray<uint8>& currentBuffer)
{
	TArray<FProcessedData> processedData;

	currentBuffer.Append(receivedData);

	parser->parseMessageFrom(currentBuffer, processedData);

	return processedData;
}

void UBaseConnection::processSendDataWithSizePrefix(const TArray<uint8>& dataBeingSend, TArray<uint8>& dataProcessedToSend)
{
	parser->convertMessageTo(dataBeingSend, dataProcessedToSend);
}

bool UBaseConnection::ipStringToNums(const FString& ip, uint8(&parts)[4])
{
	ip.Replace(TEXT(" "), TEXT(""));
	TArray<FString> strParts;
	ip.ParseIntoArray(strParts, TEXT("."), true);
	if (strParts.Num() != 4)
		return false;

	for (int32 i = 0; i < 4; ++i)
	{
		parts[i] = FCString::Atoi(*strParts[i]);
	}

	return true;
}

bool UBaseConnection::reopenConnection()
{
	((FMessageProtocolsModule*)& IMessageProtocolsModule::get())->removeConnection(this);
	bool bSuccessful = closeConnection();
	if (bSuccessful)
	{
		clearBuffers();
		bSuccessful = openConnection();
		if(bSuccessful)
			((FMessageProtocolsModule*)& IMessageProtocolsModule::get())->addConnection(this);
		return bSuccessful;
	}
	return false;
}

FString UBaseConnection::asString(int32 index /*= 0*/)
{
	if(rawDataReceived.IsValidIndex(index))
		return FString(rawDataReceived[index].data.Num(),FUTF8ToTCHAR((ANSICHAR*)rawDataReceived[index].data.GetData(),rawDataReceived[index].data.Num()).Get());

	return "";
}

int32 UBaseConnection::asInt(int32 index /*= 0*/)
{
	if (!rawDataReceived.IsValidIndex(index) || rawDataReceived[index].data.Num() != 4)
	{
		return 0;
	}

	return *reinterpret_cast<int32*>(rawDataReceived[index].data.GetData());
}

float UBaseConnection::asFloat(int32 index /*= 0*/)
{
	if (!rawDataReceived.IsValidIndex(index) || rawDataReceived[index].data.Num() != 4)
	{
		return 0;
	}

	return *reinterpret_cast<float*>(rawDataReceived[index].data.GetData());
}

void UBaseConnection::sendString(FString stringToSend)
{
	validateSendData();
	auto charArr = FTCHARToUTF8(*stringToSend);
	dataToSend.Add(TArray<uint8>((uint8*)charArr.Get(), charArr.Length()));
}

void UBaseConnection::sendFloat(float value)
{
	validateSendData();
	dataToSend.Add(TArray<uint8>(reinterpret_cast<uint8*>(&value), 4));
}

void UBaseConnection::sendInt(int32 value)
{
	validateSendData();
	dataToSend.Add(TArray<uint8>(reinterpret_cast<uint8*>(&value), 4));
}

bool UBaseConnection::hasReceivedData()
{
	return rawDataReceived.Num() > 0;
}

int32 UBaseConnection::receivedDataNum()
{
	return rawDataReceived.Num();
}

bool UBaseConnection::hasDataToSend()
{
	return dataToSend.Num() > 0;
}

int32 UBaseConnection::sendDataNum()
{
	return dataToSend.Num();
}

bool UBaseConnection::clearReceivedData()
{
	if (rawDataReceived.Num())
	{
		rawDataReceived.Empty(maxDataToReceive);
		return true;
	}
	return false;
}

void UBaseConnection::flushData()
{
	sendData();
}

DEFINE_FUNCTION(UBaseConnection::execStructToJson)
{
	// Steps into the stack, walking to the next property in it
	Stack.Step(Stack.Object, NULL);

	// Grab the last property found when we walked the stack
	// This does not contains the property value, only its type information
	UStructProperty* structProperty = ExactCast<UStructProperty>(Stack.MostRecentProperty);
	// Grab the base address where the struct actually stores its data
	// This is where the property value is truly stored
	void* structPtr = Stack.MostRecentPropertyAddress;

	// We need this to wrap up the stack
	P_FINISH;
	if (Z_Param__Result)
	{
		FString& jsonString=*(reinterpret_cast<FString*>(Z_Param__Result));
		FJsonObjectConverter::UStructToJsonObjectString(structProperty->Struct, structPtr, jsonString, 0, 0, 0, nullptr, false);
	}
	
}

void UBaseConnection::sendStructAsJson(UStruct* uStruct, void* structPtr)
{
	FString jsonString;
	if (FJsonObjectConverter::UStructToJsonObjectString(uStruct, structPtr, jsonString, 0, 0, 0, nullptr, false))
	{
		sendString(jsonString);
	}
}

bool UBaseConnection::readAsJson(FString str, UStruct* uStruct, void* structPtr)
{
	TSharedPtr<FJsonObject> jsonObject;
	TSharedRef<TJsonReader<> > jsonReader = TJsonReaderFactory<>::Create(str);
	if (!FJsonSerializer::Deserialize(jsonReader, jsonObject) || !jsonObject.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("JsonObjectStringToUStruct - Unable to parse json=[%s]"), *str);
		return false;
	}

	if (!FJsonObjectConverter::JsonObjectToUStruct(jsonObject.ToSharedRef(), uStruct,structPtr))
	{
		UE_LOG(LogTemp, Warning, TEXT("JsonObjectStringToUStruct - Unable to deserialize. json=[%s]"), *str);
		return false;
	}
	return true;
}

void UBaseConnection::validateSendData()
{
	if (dataToSend.Num() >= maxDataToSend)
	{
		int32 numToRemove = (maxDataToSend - dataToSend.Num()) + 1;// As we are adding 1 after removal
		dataToSend.RemoveAtSwap(0, numToRemove);
	}
}

