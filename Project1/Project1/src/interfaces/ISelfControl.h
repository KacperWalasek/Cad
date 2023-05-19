#pragma once

class ISelfControl
{
public:
	virtual bool canBeDeleted() const = 0;
};