#pragma once

#include "Threads/ProtocolsThreadBase.h"

class FProtocolsReceiverThread : public FProtocolsThreadBase
{	
	

protected:
	virtual void processConnections(const TArray<UWeakConnectionsPtr>& connectionsToProcess) override;

public:


	FProtocolsReceiverThread(FString tName) :FProtocolsThreadBase(tName)
	{

	}

};