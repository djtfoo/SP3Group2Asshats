#include "General/Application.h"
#include "General/CheckMemoryLeak.h"

int main( void )
{
	Application &app = Application::GetInstance();
	app.Init();
	app.Run();
	app.Exit();

    CheckMemoryLeaks();
}