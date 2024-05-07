#include <GLFW/glfw3.h>

const int gWidth = 640;
const int gHeight = 480;

void render() {
	glBegin(GL_TRIANGLES);
	glVertex2f(-0.5f, -0.5f);
	glVertex2f(0.0f, 0.5f);
	glVertex2f(0.5f, -0.5f);
	glEnd();
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(gWidth, gHeight, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		render();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}