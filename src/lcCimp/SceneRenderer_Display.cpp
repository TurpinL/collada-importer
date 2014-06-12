#include "SceneRenderer.hpp"

#include <iostream>

void renderMesh(ltcimp::Mesh &mesh);

void SceneRenderer::display()
{
	static float rotation = 0;
	rotation += 0.01;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		glTranslatef(0.0f, 0.0f, -3.0f);
		glRotatef(rotation, 0, 1, 0);

		renderMesh(m_testMesh);

		glFlush();
	m_window->display();
}

void renderMesh(ltcimp::Mesh &mesh)
{
	glBegin(GL_TRIANGLES);
		for(int i = 0; i < mesh.numVerts * 3; i += 3)
		{
			glNormal3f(mesh.norms[i], mesh.norms[i+1], mesh.norms[i+2]);
			glVertex3f(mesh.verts[i], mesh.verts[i+1], mesh.verts[i+2]);
		}
	glEnd();
}