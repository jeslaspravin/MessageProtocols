// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Templates/SubclassOf.h"
#include "ParserProtocols/BaseParserProtocol.h"
#include "BaseConnection.generated.h"

class FProtocolsThreadBase;
class FProtocolsReceiverThread;
class FProtocolsSenderThread;
class UMessageProtocolsFunctionLibrary;
class FSocket;


/**
 * 
 */
UCLASS(Abstract)
class MESSAGEPROTOCOLS_API UBaseConnection : public UObject
{
	GENERATED_BODY()

	friend FProtocolsThreadBase;
	friend FProtocolsReceiverThread;
	friend FProtocolsSenderThread;
	friend UMessageProtocolsFunctionLibrary;

private:

	TArray<FProcessedData> rawDataReceived;

	UPROPERTY()
		UBaseParserProtocol* parser;

protected:
	
	TArray<TArray<uint8>> dataToSend;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		int32 maxDataToSend = 20;

	// Maximum number of messages can be held by receiver at a time
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		int32 maxDataToReceive = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parser")
		TSubclassOf<UBaseParserProtocol> messageParserProtocolClass = UBaseParserProtocol::StaticClass();

private:

	void sendStructAsJson(UStruct* uStruct, void* structPtr);
	bool readAsJson(FString str, UStruct* uStruct, void* structPtr);
	// validate send data before adding one
	void validateSendData();
	void validateAndSortReceived();

protected:

	virtual bool openConnection();
	virtual bool closeConnection() { return true; }
	virtual void receiveData() {}
	virtual void sendData() {}

	virtual void clearBuffers();

	void appendToReceived(TArray<FProcessedData>&& data);
	void appendToReceived(TArray<FProcessedData>& data);
	void appendToReceived(FProcessedData& data);
	void appendToReceived(FProcessedData&& data);

	TArray<FProcessedData> processRecvData(const TArray<uint8>& receivedData, TArray<uint8>& currentBuffer);
	void processSendDataWithSizePrefix(const TArray<uint8>& dataBeingSend, TArray<uint8>& dataProcessedToSend);

	bool ipStringToNums(const FString& ip, uint8 (&parts)[4]);

public:

	// Note : This clears any existing buffers of the current connection and reopens new connection with provided or modified settings.
	UFUNCTION(BlueprintCallable, Category = "Connections")
		bool reopenConnection();

	UFUNCTION(BlueprintCallable,Category="Data")
		FString asString(int32 index = 0);

	UFUNCTION(BlueprintCallable, Category = "Data")
		int32 asInt(int32 index = 0);

	UFUNCTION(BlueprintCallable, Category = "Data")
		float asFloat(int32 index = 0);

	UFUNCTION(BlueprintCallable, Category = "Data")
		void sendString(FString stringToSend);

	UFUNCTION(BlueprintCallable, Category = "Data")
		void sendFloat(float value);

	UFUNCTION(BlueprintCallable, Category = "Data")
		void sendInt(int32 value);

	UFUNCTION(BlueprintCallable, Category = "Data")
		bool hasReceivedData();

	UFUNCTION(BlueprintCallable, Category = "Data")
		int32 receivedDataNum();

	UFUNCTION(BlueprintCallable, Category = "Data")
		bool hasDataToSend();

	UFUNCTION(BlueprintCallable, Category = "Data")
		int32 sendDataNum();

	// Mostly used after successful reading
	UFUNCTION(BlueprintCallable, Category = "Data")
		bool clearReceivedData();

	template<typename T> 
	void sendStruct(T& structToSend);

	template<typename T> T asStruct(bool& success, int32 index = 0);

	UFUNCTION(BlueprintCallable, Category = "Json", CustomThunk, meta = (CustomStructureParam = "StructVariable"))
		static FString StructToJson(UProperty* StructVariable);

	DECLARE_FUNCTION(execStructToJson);
};

template<typename T>  
void UBaseConnection::sendStruct(T& structToSend)
{
	sendStructAsJson(T::StaticStruct(), &structToSend);
}

template<typename T> T UBaseConnection::asStruct(bool& success, int32 index /*= 0*/)
{
	T retVal = T();

	FString str = asString(index);
	success = readAsJson(str,T::StaticStruct(),&retVal);

	return retVal;
}

