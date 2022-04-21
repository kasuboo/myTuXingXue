#define GLEW_STATIC //静态链接
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include<iostream>
#include<glm.hpp>
#include<gtc/matrix_transform.hpp>
#include<gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;
int main(void)
{
	GLFWwindow* window; //定义一个窗口

	if (!glfwInit()) //初始化glfw库
		return -1;

	//创建一个窗口,窗口尺寸、名称
	window = glfwCreateWindow(640, 480, "myPage", NULL, NULL);
	if (!window) //确定是否创建成功
	{
		glfwTerminate(); //终止当前窗口
		return -1;
	}
	glfwMakeContextCurrent(window); //创建上下文,用于初始化glew
	//初始化glew
	if (glewInit() != GLEW_OK)
	{
		cout << "init glew failed!";
		glfwTerminate(); //终止窗口
		return -1;
	}
	else
		cout << "init is ok!";
	cout << glGetString(GL_VERSION);

	//定义顶点数据
	float vertices[60] = {
		//矩形
		-0.5f,0.0f,0.0f,0.5f,
		-0.5f,0.25f,0.0f,0.75f,
		0.0f,0.25f,0.5f,0.75f, //第一个
		-0.5f,0.0f,0.0f,0.5f,
		0.0f,0.25f,0.5f,0.75f,
		0.0f,0.0f,0.5f,0.5f, //第二个
		//房子
		-0.5f,-0.75f,0.0f,-0.25f,
		-0.25f,-0.5f,0.25f,0.0f,
		0.0f,-0.75f,0.5f,-0.25f, //第一个
		-0.5f,-0.95f,0.0f,-0.45f,
		-0.5f,-0.75f,0.0f,-0.25f,
		0.0f,-0.95f,0.5f,-0.45f, //第二个
		0.0f,-0.95f,0.5f,-0.45f,
		-0.5f,-0.75f,0.0f,-0.25f,
		0.0f,-0.75f,0.5f,-0.25f
	};
	unsigned int index[] = { //索引下标
		0,1,2,
		3,4,5,
		6,7,8,
		9,10,11,
		12,13,14
	};

	//发送数据
	unsigned int buffer;  //VBO对象  
	glGenBuffers(1, &buffer); //创建buffer个数、缓冲区对象id，建立缓冲对象VBO管理内存
	unsigned int VAOS; //VAO对象
	glGenVertexArrays(1, &VAOS);
	unsigned int EBOS; //EBO对象
	glGenBuffers(1, &EBOS);

	glBindVertexArray(VAOS); //绑定VAO
	glBindBuffer(GL_ARRAY_BUFFER, buffer); //在当前缓冲类型下绑定buffer  
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);//将顶点数据复制到当前缓冲   
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOS); //绑定EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

	//解析数据    
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0); //配置   
	glEnableVertexAttribArray(0); //配置完成,启用

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
	glEnableVertexAttribArray(1);

	//创建纹理
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); //绑定
	//为绑定设置纹理形式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//翻转图片y轴,使得和opengl方向一致
	stbi_set_flip_vertically_on_load(true);

	int width, height, nr;
	unsigned char* data = stbi_load("test.jpg", &width, &height, &nr, 0);
	cout << width << " " << height << " " << nr << endl;
	if (data)
	{
		//用载入的图片生成一个纹理
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		cout << "fail to load texture" << endl;
	}
	stbi_image_free(data); //释放内存

	//引入着色器
	//顶点着色器
	const char* vertexShaderSource =
		"#version 330 core\n"
		"layout (location = 0) in vec2 aPos;\n" //设定输入对象的位置值
		"layout (location=1) in vec2 atexCord;\n" //定义纹理属性作为顶点着色器的输入
		"out vec2 myTexCord;\n"

		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
		"myTexCord = atexCord;\n" //直接将顶点着色器的纹理坐标输出
		"}\0";

	//片段着色器
	const char* fragmentShaderSource =
		"#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec2 myTexCord;\n"
		"uniform sampler2D ourTexture;\n"

		"void main()\n"
		"{\n"
		//"FragColor=vec4(0.0f,0.0f,1.0f,1.0f);\n" //4个元素的数组：红色、绿色、蓝色和alpha(透明度)分量
		"FragColor = texture(ourTexture,myTexCord);\n"
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

	//删除着色器对象
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//激活程序对象
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT); //清空颜色缓冲
		glClearColor(1.0, 1.0, 1.0, 0.0); //设置窗口白色
		glUseProgram(shaderProgram); //
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(VAOS);

		glDrawElements(GL_TRIANGLES, 15, GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_QUADS, 0, 4); //绘制模式、索引数组的起始位置、顶点数量
		//glDrawArrays(GL_POLYGON, 8, 5);

		glBindVertexArray(0); //解绑VAO
		glfwSwapBuffers(window);
		glfwPollEvents();

	}


	glfwTerminate();
	return 0;

}

