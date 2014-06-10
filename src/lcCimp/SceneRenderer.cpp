#include "SceneRenderer.hpp"

SceneRenderer::SceneRenderer()
{
	m_window = nullptr;
}

void SceneRenderer::runGameLogicLoop()
{
	init();

	while (true)
	{
		sf::Event E;

		while (m_window->pollEvent(E))
		{
			handleEvent(E);
		}

		idle();
	}

	//TODO: Cleanup
}