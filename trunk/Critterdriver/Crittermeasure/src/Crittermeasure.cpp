#include <iostream>

#include "MeasuredSerialLink.h"
#include <network/Server.h>
#include "MeasuredActionWriter.h"
#include "MocActionBuffer.h"
#include "Measurements.h"

const bool LatencyLogging = false;

int main() {
	MeasuredActionWriter actionWriter;
	MeasuredSerialLink serialLink(actionWriter);
	if (!serialLink.init())
		return -1;
	MocActionBuffer actionBuffer;
	Server server(serialLink, actionBuffer, 2330);
	if (!server.start())
		return -1;
	// Main loop
	while (true) {
		if (server.hasClients() && Measurements::m().isDone())
			Measurements::m().start();
		Measurements::m().setStartWaiting();
		serialLink.waitForData();
		const CritterControlDrop* lastAction = actionBuffer.getLastAction();
		if (lastAction) {
			if (lastAction->m100 != 0)
				Measurements::m().setActionStartWriting();
			bool actionSend = serialLink.sendActionIfPossible(*lastAction);
			if (!actionSend)
				std::cerr << "Warning: action skipped" << std::endl;
		}
		CritterStateDrop* stateDrop = serialLink.readStateDrop();
		if (!stateDrop) {
			Measurements::m().nextTimeStep();
			continue;
		}
		if (server.hasClients() && Measurements::m().getTime() == 0) {
			for (int i = 0; i < IR_DISTANCE_SIZE; i++)
				stateDrop->ir_distance[i] = 0;
		}
		Measurements::m().setDoneReading();
		if (stateDrop->motor100.command != 0)
			Measurements::m().setCommandBackFromARM();
		if (Measurements::m().getTime() == 0)
			Measurements::m().setObservationSend();
		server.processStateDrop(*stateDrop);
		Measurements::m().nextTimeStep();
	}
	return 0;
}
