#include "Threads/ProtocolsThreadBase.h"
#include "MessageProtocolPrivatePCH.h"

void FProtocolsThreadBase::processInternal()
{
	for (TArray<UWeakConnectionsPtr>::TIterator itr = connections.CreateIterator(); itr; ++itr)
	{
		if (!itr->IsValid(true, true))
		{
			itr.RemoveCurrent();
		}
	}
	processConnections(connections);
}

FProtocolsThreadBase::~FProtocolsThreadBase()
{
	if (syncEvent)
	{
		FPlatformProcess::ReturnSynchEventToPool(syncEvent);
		syncEvent = nullptr;
	}
	delete thread;
	thread = nullptr;
}

bool FProtocolsThreadBase::Init()
{
	doShutdown = false;
	doPause = false;
	isProcessing = false;
	isRunningThead = true;
	syncEvent=FPlatformProcess::GetSynchEventFromPool(false);
	return isRunningThead;
}

void FProtocolsThreadBase::Exit()
{
	if (syncEvent)
	{
		FPlatformProcess::ReturnSynchEventToPool(syncEvent);
		syncEvent = nullptr;
	}
}

void FProtocolsThreadBase::Tick()
{
	processInternal();
}

void FProtocolsThreadBase::shutDown(FProtocolsThreadBase* threadToShutdown)
{
	check(threadToShutdown);
	threadToShutdown->doShutdown = true;
	if (threadToShutdown->thread)
	{
		resumeThread(threadToShutdown);
		threadToShutdown->thread->WaitForCompletion();
	}
	threadToShutdown->isRunningThead = false;
}

bool FProtocolsThreadBase::isRunning(FProtocolsThreadBase* runnable)
{
	return runnable->isRunningThead && !runnable->doPause;
}

void FProtocolsThreadBase::pushConnection(FProtocolsThreadBase* runnable, UWeakConnectionsPtr connection)
{
	if (!runnable->thread)
	{
		runnable->thread = FRunnableThread::Create(runnable, *(runnable->threadName), 128 * 1024, TPri_Normal);
	}

	check(runnable);
	if (runnable->isProcessing)
		runnable->syncEvent->Wait();

	if (!runnable->connections.Contains(connection))
	{
		runnable->connections.Add(connection);
	}
}

void FProtocolsThreadBase::removeConnection(FProtocolsThreadBase* runnable, UWeakConnectionsPtr connection)
{
	check(runnable);

	if (runnable->isProcessing)
		runnable->syncEvent->Wait();

	runnable->connections.RemoveSingleSwap(connection);
}

void FProtocolsThreadBase::pauseThread(FProtocolsThreadBase* runnable)
{
	check(runnable);
	runnable->doPause = true;
}

void FProtocolsThreadBase::resumeThread(FProtocolsThreadBase* runnable)
{
	check(runnable);
	runnable->doPause = false;
	runnable->syncEvent->Trigger();
}

uint32 FProtocolsThreadBase::Run()
{
	while (!doShutdown)
	{
		if (doPause)
		{
			syncEvent->Wait();
		}

		isProcessing = true;
		processInternal();

		isProcessing = false;
		// To let insertion or deletion of connections
		syncEvent->Trigger();

		FPlatformProcess::Sleep(UMessageProtocolSettings::getSleepDuration());
	}
	return 0;
}


