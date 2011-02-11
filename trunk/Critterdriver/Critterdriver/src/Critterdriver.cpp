#include <iostream>

#include <seriallink/CritterSerialLink.h>
#include "log/LatencyLogger.h"
#include <network/SyncActionBuffer.h>
#include <network/Server.h>

const bool LatencyLogging = false;

int main() {
	ActionWriter actionWriter;
	CritterSerialLink serialLink(actionWriter);
	if (!serialLink.init())
		return -1;
	LatencyLogger latencyLogger;
	if (LatencyLogging)
		latencyLogger.init();
	SyncActionBuffer actionBuffer;
	Server server(serialLink, actionBuffer, 2330);
	if (!server.start())
		return -1;

	// Main loop
	while (true) {
		serialLink.waitForData();
		const CritterControlDrop* lastAction = actionBuffer.getLastAction();
		if (lastAction) {
			bool actionSend = serialLink.sendActionIfPossible(*lastAction);
			if (!actionSend)
				std::cerr << "Warning: action skipped" << std::endl;
		}
		CritterStateDrop* stateDrop = serialLink.readStateDrop();
		if (!stateDrop)
			continue;
		server.processStateDrop(*stateDrop);
	}
	return 0;
}
