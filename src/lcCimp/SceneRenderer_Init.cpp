#include "SceneRenderer.hpp"

#include <iostream>
#include <string>

std::string read(const std::string &filename);

void SceneRenderer::init()
{
	initSFML();
	initGL();

	// Read collada file.
	ltcimp::Importer importer;

	importer.setContentDirectory("../../content/");
	importer.importScene("BonedBox.dae");
	m_testMesh = importer.getMesh();

	m_lastClock = m_gameClock.getElapsedTime();
	m_frameCount = 0;
}

void SceneRenderer::initSFML()
{
	m_window = new sf::Window(sf::VideoMode(800, 600), "Collada Viewer", sf::Style::Default, sf::ContextSettings(32));
	m_window->setVerticalSyncEnabled(true);
	sf::Vector2u &windowSize = m_window->getSize();
	reshape(windowSize.x, windowSize.y, 60);
}

void SceneRenderer::initGL()
{
	glClearColor(0.0f, 0.1f, 0.2f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
}

