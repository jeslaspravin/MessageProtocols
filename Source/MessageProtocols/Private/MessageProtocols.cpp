// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "MessageProtocols.h"
#include "Threads/ProtocolsReceiverThread.h"
#include "Threads/ProtocolsSenderThread.h"
#include "MessageProtocolPrivatePCH.h"

DEFINE_LOG_CATEGORY(LogMessageProtocolsModule)

#define LOCTEXT_NAMESPACE "FMessageProtocolsModule"

void FMessageProtocolsModule::StartupModule()
{
	threads[RECEIVER_THREAD_IDX] = new FProtocolsReceiverThread("ProtocolReceiverThread");

	threads[SENDER_THREAD_IDX] = new FProtocolsSenderThread("ProtocolSenderThread");
}

void FMessageProtocolsModule::ShutdownModule()
{
	for (int i = 0; i < THREAD_NUM; i++)
	{
		FProtocolsThreadBase::shutDown(threads[i]);
		delete threads[i];
		threads[i] = nullptr;
	}
}

void FMessageProtocolsModule::addConnection(UBaseConnection* connection)
{
	FProtocolsThreadBase::pushConnection(threads[RECEIVER_THREAD_IDX], connection);
	FProtocolsThreadBase::pushConnection(threads[SENDER_THREAD_IDX], connection);
}

void FMessageProtocolsModule::removeConnection(UBaseConnection* connection)
{
	FProtocolsThreadBase::removeConnection(threads[RECEIVER_THREAD_IDX], connection);
	FProtocolsThreadBase::removeConnection(threads[SENDER_THREAD_IDX], connection);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMessageProtocolsModule, MessageProtocols)