#pragma once

class ISimulation {
public:
	virtual void start() = 0;
	virtual void stop() = 0;
	virtual void reset() = 0;
	virtual void update() = 0;
	virtual bool isRunning() const = 0;
};