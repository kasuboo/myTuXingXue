#define GLEW_STATIC //静态链接
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include<iostream>
using namespace std;
int main(void)
{
	GLFWwindow* window; //定义一个窗口

	if (!glfwInit()) //初始化glfw库
		return -1;
	//利用glfwwindowHint对GLFW进行配置
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//创建一个窗口,窗口尺寸、名称
	window = glfwCreateWindow(640, 480, "myPage", NULL, NULL);
	if (!window) //确定是否创建成功
	{
		glfwTerminate(); //终止当前窗口
		return -1;
	}
	glfwMakeContextCurrent(window); //创建上下文,用于初始化glew
	//初始化glew
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		cout<<"init glew failed!";
		glfwTerminate(); //终止窗口
		return -1;
	}
	else
		cout<<"init is ok!";
	cout << glGetString(GL_VERSION);

	//循环,不断绘制
	while (!glfwWindowShouldClose(window)) //当无需关闭时
	{
		//清空颜色缓存
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(1.0, 1.0, 1.0, 0.0); //设置窗口白色
		glColor3f(0.0, 0.0, 0.0); //绘制的颜色为绿色
		//绘制三角形
		glBegin(GL_TRIANGLES);
		glVertex2f(-0.5f, -0.5f);
		glVertex2f(0.0f, 0.5f);
		glVertex2f(0.5f, -0.5f);
		glEnd();

		glfwSwapBuffers(window); //缓冲区
		glfwPollEvents(); //处理已经在队列的事件
	}
	glfwTerminate();
	return 0;

}


