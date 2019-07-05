// Fill out your copyright notice in the Description page of Project Settings.


#include "MessageProtocolsFunctionLibrary.h"
#include "IMessageProtocols.h"
#include "MessageProtocols.h"
#include "Connections/BaseConnection.h"

UBaseConnection* UMessageProtocolsFunctionLibrary::createConnection(UClass* templateClass, UObject* ownerObj)
{
	UBaseConnection* connection=nullptr;
	EObjectFlags flags = RF_Transient;
	if (ownerObj)
	{
		connection = NewObject<UBaseConnection>(ownerObj, templateClass, NAME_None, flags);
	}
	else
	{
		connection = NewObject<UBaseConnection>((UObject*)GetTransientPackage(), templateClass, NAME_None, flags);
	}
	connection->AddToRoot();
	connection->parser = NewObject<UBaseParserProtocol>(connection, connection->messageParserProtocolClass, NAME_None, flags);
	return connection;
}

UBaseConnection* UMessageProtocolsFunctionLibrary::createAndOpenConnection(UClass* templateClass, UObject* ownerObj)
{
	UBaseConnection* connection = createConnection(templateClass, ownerObj);
	bool bSuccessful=openConnection(connection);
	if (!bSuccessful)
	{
		connection->RemoveFromRoot();
		connection->ConditionalBeginDestroy();
		connection = nullptr;
	}
	return connection;
}

bool UMessageProtocolsFunctionLibrary::openConnection(UBaseConnection* connection)
{
	bool bSuccessful=connection->openConnection();
	if(bSuccessful)
		((FMessageProtocolsModule*)&IMessageProtocolsModule::get())->addConnection(connection);
	return bSuccessful;
}

bool UMessageProtocolsFunctionLibrary::closeConnection(UBaseConnection* connection)
{
	((FMessageProtocolsModule*)&IMessageProtocolsModule::get())->removeConnection(connection);
	bool bSuccessful = connection->closeConnection();
	connection->parser->ConditionalBeginDestroy();
	connection->RemoveFromRoot();
	connection->ConditionalBeginDestroy();
	return bSuccessful;
}
