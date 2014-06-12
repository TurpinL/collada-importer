#ifndef SCENERENDERER_HPP
#define SCENERENDERER_HPP

#include <SFML/Window.hpp> // SFML 2.1
#include <SFML/OpenGL.hpp> // SFML 2.1

#include "Mesh.hpp"
#include "ltCimp.hpp"

class SceneRenderer
{
public: 
	SceneRenderer();
	void runGameLogicLoop();

private:
	bool m_isInitialized;
	sf::Window *m_window;

	ltcimp::Mesh m_testMesh;

	void init();
		void initSFML();
		void initGL();

	void handleEvent(sf::Event &E);
		void reshape(int w, int h, double fov);

	void idle();
		void calcFrameTime();
			sf::Clock m_gameClock;
			sf::Time m_frameTime; 
			sf::Time m_lastClock;
			int m_frameCount;

	void display();
};

#endif //SCENERENDERER_HPP