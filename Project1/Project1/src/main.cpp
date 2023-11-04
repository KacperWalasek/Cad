#include "CadApp.h"
#include <iostream>

int main()
{
	try {
		CadApp app;
		app.Run();
	}
	catch (std::invalid_argument e) 
	{
		std::cout << e.what() << std::endl;
	}
	return 0;
}