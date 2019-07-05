#include "Threads/ProtocolsSenderThread.h"
#include "MessageProtocolPrivatePCH.h"

void FProtocolsSenderThread::processConnections(const TArray<UWeakConnectionsPtr>& connectionsToProcess)
{
	for (int i = 0; i < connectionsToProcess.Num(); i++)
	{
		UWeakConnectionsPtr connection = connectionsToProcess[i];
		connection->sendData();
	}
}
