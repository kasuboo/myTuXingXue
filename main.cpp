#define GLEW_STATIC //静态链接
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include<iostream>
#include<stdlib.h>
using namespace std;
int main(void)
{
	GLFWwindow* window; //定义一个窗口

	if (!glfwInit()) //初始化glfw库
		return -1;

	/*//利用glfwwindowHint对GLFW进行配置
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

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

	//定义顶点数据
	float vertices[6] = {
		-0.5f,-0.5f,0.0f,
		0.5f,0.5f,-0.5f
	};
	//发送数据
	unsigned int buffer;
	glGenBuffers(1, &buffer); //创建buffer个数、缓冲区对象id，建立缓冲对象VBO管理内存
	glBindBuffer(GL_ARRAY_BUFFER, buffer); //在当前缓冲类型下绑定buffer
	//将顶点数据复制到当前缓冲
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), vertices, GL_STATIC_DRAW);

	//解析数据
	//配置
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
	//启用
	glDisableVertexAttribArray(0);

	//引入着色器
	//顶点着色器
	const char* vertexShaderSource = 
		"#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n" //设定输入对象的位置值
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";
	//片段着色器
	const char* fragmentShaderSource =
		"#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		    "FragColor=vec4(1.0f,0.5f,0.2f,1.0f);\n" //4个元素的数组：红色、绿色、蓝色和alpha(透明度)分量
		"}\n";
	unsigned int vertexShader; //创建一个顶点着色器对象
	vertexShader = glCreateShader(GL_VERTEX_SHADER); //返回创建出的着色器id
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); //将顶点着色器赋给对象，并编译
	glCompileShader(vertexShader); //编译这个对象
	
	unsigned int fragmentShader; //创建一个片段着色器对象
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	//将着色器对象附加给着色器程序对象
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram(); //创建着色器程序对象
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram); //链接程序

	//激活程序对象
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT); //清空颜色缓冲
		glUseProgram(shaderProgram); //
		glDrawArrays(GL_TRIANGLES, 0, 3); //绘制模式、索引数组的起始位置、顶点数量
		glfwSwapBuffers(window);
		glfwPollEvents();
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}


	glfwTerminate();
	return 0;

}


