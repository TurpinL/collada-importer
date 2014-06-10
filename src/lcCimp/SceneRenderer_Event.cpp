#include "SceneRenderer.hpp"

void SceneRenderer::handleEvent(sf::Event &E)
{
	switch(E.type)
	{
	case sf::Event::Resized:
		reshape(E.size.width, E.size.height, 60);
	}
}

void SceneRenderer::reshape(int w, int h, double fov)
{
	const double DEG_TO_RAD = 0.0174532925;
	double ratio; // The ratio between width and height

	// Render to the whole window
	glViewport(0, 0, w, h);

	// Swap to the projection matrix
	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		double nearClipping = 0.1;
		double extent = nearClipping * tan(fov*DEG_TO_RAD*0.5);

		if(w <= h)
		{
			ratio = (double)h / (double)w;
			glFrustum(-extent, extent, -extent*ratio, extent*ratio, nearClipping, 1000.0);
		}
		else
		{
			ratio = (double)w / (double)h;
			glFrustum(-extent*ratio, extent*ratio, -extent, extent, nearClipping, 1000.0);
		}
	glMatrixMode(GL_MODELVIEW);
}