#include "Threads/ProtocolsReceiverThread.h"
#include "MessageProtocolPrivatePCH.h"

void FProtocolsReceiverThread::processConnections(const TArray<UWeakConnectionsPtr>& connectionsToProcess)
{
	for (int i = 0; i < connectionsToProcess.Num(); i++)
	{
		UBaseConnection* connection = connectionsToProcess[i].Get();
		connection->receiveData();
	}
}
