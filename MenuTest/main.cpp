#include "./src/Game.hpp"
#include "./src/logger/Logger.hpp"

int main(int argc, char* args[]) 
{
	GameLogger* logger = new GameLogger();
	logger->SetVerbosity(LogPriority::DebugP);
	Game* game = Game::GetInstance(logger);

	if ( !game->init() )
	{
		logger->LogError("Game can not init");
		
		return -1;
	}

	if ( !game->start() )
	{
		logger->LogError("Game can not start");

		return -1;
	}

	return 0;
}
