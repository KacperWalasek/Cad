#pragma once

class ISelfControl
{
public:
	virtual bool canBeDeleted() const = 0;
	virtual bool canBeMoved() const = 0;
};