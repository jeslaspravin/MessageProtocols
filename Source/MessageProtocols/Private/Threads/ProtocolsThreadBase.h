#pragma once

#include "HAL/Runnable.h"
#include "Misc/SingleThreadRunnable.h"
#include "HAL/RunnableThread.h"

class UBaseConnection;

class FProtocolsThreadBase : public FRunnable, public FSingleThreadRunnable
{
public:

	typedef TWeakObjectPtr<UBaseConnection> UWeakConnectionsPtr;

private:

	bool doShutdown;

	FRunnableThread* thread=nullptr;
	FEvent* syncEvent=nullptr;

	bool doPause;
	bool isProcessing;
	bool isRunningThead;
	
	TArray<UWeakConnectionsPtr> connections;

	FString threadName;

private:

	void processInternal();

protected:

	virtual void processConnections(const TArray<UWeakConnectionsPtr>& connectionsToProcess) = 0; 
	
	FProtocolsThreadBase(FString tName) :threadName(tName)
	{
		isRunningThead = false;
	}

public:

	

	virtual ~FProtocolsThreadBase();

	virtual bool Init() override;

	uint32 Run() override;

	virtual void Exit() override;

	virtual void Tick() override;
	
	static void shutDown(FProtocolsThreadBase* threadToShutdown);

	static bool isRunning(FProtocolsThreadBase* runnable);

	static void pushConnection(FProtocolsThreadBase* runnable,UWeakConnectionsPtr connection);

	static void removeConnection(FProtocolsThreadBase* runnable,UWeakConnectionsPtr connection);
	
	/* Should be called only on running thread */
	static void pauseThread(FProtocolsThreadBase* runnable);
	static void resumeThread(FProtocolsThreadBase* runnable);

};