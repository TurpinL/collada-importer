#include "SceneRenderer.hpp"

#include <iostream>
#include <fstream>
#include <string>

std::string read(const std::string &filename);

void SceneRenderer::init()
{
	initSFML();
	initGL();

	// Read collada file.
	ltcimp::import(read("../../content/testScene.dae"), m_testMesh);
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
}

std::string read(const std::string &filename)
{
	std::ifstream file(filename, std::ios::in | std::ios::binary);

	if (file.is_open())
	{
		std::string contents;
		
		// Work out the size of the file so we can
		// set the string size.
		file.seekg(0, std::ios::end);
		contents.resize((unsigned int )file.tellg());

		// Read the entire file into contents.
		file.seekg(0, std::ios::beg);
		file.read(&contents[0], contents.size());

		file.close();

		return contents;
	}

	// File didn't open, return a blank string.
	std::cerr << "File Not Found: \"" << filename << "\"" << std::endl;
	return "";
}