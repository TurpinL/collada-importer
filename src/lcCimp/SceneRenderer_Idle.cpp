#include "SceneRenderer.hpp"

#include <iostream>

void SceneRenderer::idle()
{
	calcFrameTime();

	display();
}

void SceneRenderer::calcFrameTime()
{
	sf::Time curTime = m_gameClock.getElapsedTime();
	m_frameTime = curTime - m_lastClock;

	m_frameCount++;

	// FPS Printer
	if(m_frameCount % 20 == 0)
		std::cout << (1.0f / m_frameTime.asSeconds()) << std::endl;

	m_lastClock = curTime;
}