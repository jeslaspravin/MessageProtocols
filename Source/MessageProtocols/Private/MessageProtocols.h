#pragma once

#include "IMessageProtocols.h"

class FProtocolsThreadBase;

class FMessageProtocolsModule : public IMessageProtocolsModule
{

private:

	FProtocolsThreadBase* threads[2] = { nullptr,nullptr };

public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void addConnection(UBaseConnection* connection);
	void removeConnection(UBaseConnection* connection);
};