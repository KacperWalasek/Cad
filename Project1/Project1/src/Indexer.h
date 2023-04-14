#pragma once

class Indexer
{
	int current = 0;
public:
	int getNewIndex()
	{
		return current++;
	}
	
};