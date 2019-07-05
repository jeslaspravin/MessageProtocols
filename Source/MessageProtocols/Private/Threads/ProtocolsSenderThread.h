#pragma once

#include "Threads/ProtocolsThreadBase.h"

class FProtocolsSenderThread : public FProtocolsThreadBase
{

protected:
	virtual void processConnections(const TArray<UWeakConnectionsPtr>& connectionsToProcess) override;
public:


	FProtocolsSenderThread(FString tName) :FProtocolsThreadBase(tName)
	{

	}
};