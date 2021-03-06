#define GLEW_STATIC //静态链接
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include<iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
float mixValue = 0.0f;
// 定义摄像机的初始信息
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);		// 位置向量
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);	// 方向向量
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);		// 上向量
float deltaTime = 0.0f;	// 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
// 上一次鼠标的位置，默认是屏幕中心
float lastX = 400;
float lastY = 300;
float yaw = -90.0f;
float pitch = 0.0f;
float fov = 45.0f;
bool firstMouse = true;

using namespace std;
void pressChange(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        mixValue += 0.01f; 
        if (mixValue >= 1.0f)
            mixValue = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
    {
        mixValue -= 0.01f; 
        if (mixValue <= 0.0f)
            mixValue = 0.0f;
    }
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // 每次窗口变化时重新设置图形的绘制窗口，可以理解为画布
    glViewport(0, 0, width, height);
}
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5f * deltaTime; // 相应调整
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}
void cursor_position_callback(GLFWwindow* window, double x, double y)
{
    if (firstMouse)
    {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }

    float xoffset = x - lastX;
    float yoffset = lastY - y;
    lastX = x;
    lastY = y;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) //摁动右键移动视角
    {
        float sensitivity = 0.5f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
        front.y = sin(glm::radians(pitch));
        front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
        cameraFront = glm::normalize(front);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}

int main(void)
{
    GLFWwindow* window; //定义一个窗口

    if (!glfwInit()) //初始化glfw库
        return -1;

    //创建一个窗口,窗口尺寸、名称
    window = glfwCreateWindow(820, 700, "myPage", NULL, NULL);
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

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST); //深度测试
    //定义顶点数据
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    unsigned int index[] = { //索引下标
        0,1,2,
        3,4,5,

        6,7,8,
        9,10,11,

        12,13,14,
        15,16,17,

        18,19,20,
        21,22,23,

        24,25,26,
        27,28,29,

        30,31,32,
        33,34,35
    };

    glEnable(GL_DEPTH_TEST); //深度测试
    //定义顶点数据
    float vertices1[] = {
        //矩形
        -1.0f,0.0f, -0.5f,0.5f,
        -1.0f,0.5f, -0.5f,1.0f,
        -0.65f,0.5f, -0.15f,1.0f, //第一个
        -1.0f,0.0f, -0.5f,0.5f,
        -0.65f,0.5f, -0.5f,1.0f,
        -0.65f,0.0f, -0.5f,0.5f, //第二个
        //房子
        -0.5f,-0.75f, 0.0f,-0.25f,
        -0.25f,-0.5f, 0.25f,0.0f,
        0.0f,-0.75f, 0.5f,-0.25f, //第一个
        -0.5f,-0.95f, 0.0f,-0.45f,
        -0.5f,-0.75f, 0.0f,-0.25f,
        0.0f,-0.95f, 0.5f,-0.45f, //第二个
        0.0f,-0.95f, 0.5f,-0.45f,
        -0.5f,-0.75f, 0.0f,-0.25f,
        0.0f,-0.75f, 0.5f,-0.25f
    };
    unsigned int index1[] = { //索引下标
        0,1,2,
        3,4,5,
        6,7,8,
        9,10,11,
        12,13,14
    };

    /*----------------------------------------------------立方体的--------------------------------------*/
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0); //配置   
    glEnableVertexAttribArray(0); //配置完成,启用
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    //创建纹理
    unsigned int texture1,texture2;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1); //绑定
    //为绑定设置纹理形式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //坐标轴
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //纹理过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //翻转图片y轴,使得和opengl方向一致
    stbi_set_flip_vertically_on_load(true);

    int width1, height1, nr1;
    unsigned char* data1 = stbi_load("mypic.jpg", &width1, &height1, &nr1, 0);
    if (data1)
    {
        //用载入的图片生成一个纹理
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        cout << "fail to load texture" << endl;
    }
    stbi_image_free(data1); //释放内存

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2); //绑定
    //为绑定设置纹理形式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //坐标轴
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //纹理过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //翻转图片y轴,使得和opengl方向一致
    stbi_set_flip_vertically_on_load(true);
    data1 = stbi_load("test.jpg", &width1, &height1, &nr1, 0);
    if (data1)
    {
        //用载入的图片生成一个纹理
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        cout << "fail to load texture" << endl;
    }
    stbi_image_free(data1); //释放内存


    //引入着色器
    //顶点着色器
    const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n" //设定输入对象的位置值
        "layout (location = 1) in vec2 atexCord;\n" //定义纹理属性作为顶点着色器的输入
        "out vec2 myTexCord;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection; \n"

        "void main()\n"
        "{\n"
        "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
        "   myTexCord = atexCord;\n"    //直接将顶点着色器的纹理坐标输出
        "}\0";

    //片段着色器
    const char* fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec2 myTexCord;\n"
        "uniform float mixValue;\n"
        "uniform sampler2D texture1;\n"
        "uniform sampler2D texture2;\n"

        "void main()\n"
        "{\n"
        "    FragColor = mix(texture(texture1, myTexCord), texture(texture2, myTexCord), mixValue);\n"
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

    /*------------------------------------------------------------矩形和多边形的-------------------------------------------------------*/
    unsigned int VBO1, VAO1, EBO1; //创建对象
    glGenBuffers(1, &VBO1);
    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &EBO1);
    //绑定对象
    glBindVertexArray(VAO1); //绑定VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBO1); //在当前缓冲类型下绑定
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);//将顶点数据复制到当前缓冲   
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1); //绑定EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index1), index1, GL_STATIC_DRAW);

    //解析数据    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0); //配置   
    glEnableVertexAttribArray(0); //配置完成,启用
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
    glEnableVertexAttribArray(1);

    //创建纹理
    unsigned int texture, textures;
    glGenTextures(1, &texture);   //生成纹理数量
    glBindTexture(GL_TEXTURE_2D, texture);   //绑定

    //为绑定设置纹理形式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nr;
    stbi_set_flip_vertically_on_load(true);  //翻转图片y轴,使得和opengl方向一致
    unsigned char* data = stbi_load("mypic.jpg", &width, &height, &nr, 0);
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

    glGenTextures(1, &textures);
    glBindTexture(GL_TEXTURE_2D, textures);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //纹理样式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(true);

    data = stbi_load("test.jpg", &width, &height, &nr, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); //用载入的图片生成一个纹理
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        cout << "fail to load texture" << endl;
    }
    stbi_image_free(data); //释放内存

    //引入着色器
    //顶点着色器
    const char* vertexShaderSource1 =
        "#version 330 core\n"
        "layout (location = 0) in vec2 aPos;\n" //设定输入对象的位置值
        "layout (location=1) in vec2 atexCord;\n" //定义纹理属性作为顶点着色器的输入
        "out vec2 myTexCord;\n"

        "uniform mat4 model1;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection; \n"

        "void main()\n"
        "{\n"
        "   gl_Position = projection * view * model1 * vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
        "   myTexCord = vec2(atexCord.x, atexCord.y);\n"    //直接将顶点着色器的纹理坐标输出
        "}\0";

    //片段着色器
    const char* fragmentShaderSource1 =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec2 myTexCord;\n"
        "uniform float mixValue;\n"
        "uniform sampler2D texture;\n"
        "uniform sampler2D textures;\n" //纹理单元2

        "void main()\n"
        "{\n"
        "    FragColor = mix(texture(texture, myTexCord), texture(textures, myTexCord), mixValue);\n" 
        "}\n";

    unsigned int vertexShader1; //创建一个顶点着色器对象
    vertexShader1 = glCreateShader(GL_VERTEX_SHADER); //返回创建出的着色器id
    glShaderSource(vertexShader1, 1, &vertexShaderSource1, NULL); //将顶点着色器赋给对象，并编译
    glCompileShader(vertexShader1); //编译这个对象

    unsigned int fragmentShader1; //创建一个片段着色器对象
    fragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader1, 1, &fragmentShaderSource1, NULL);
    glCompileShader(fragmentShader1);

    //将着色器对象附加给着色器程序对象
    unsigned int shaderProgram1;
    shaderProgram1 = glCreateProgram(); //创建着色器程序对象
    glAttachShader(shaderProgram1, vertexShader1);
    glAttachShader(shaderProgram1, fragmentShader1);
    glLinkProgram(shaderProgram1); //链接程序

    //删除着色器对象
    glDeleteShader(vertexShader1);
    glDeleteShader(fragmentShader1);
    /*------------------------------------------------over------------------------------------------------------*/

    glUseProgram(shaderProgram1);
    glUniform1i(glGetUniformLocation(shaderProgram1, "texture"), 0); //设置着色器的纹理单元
    glUniform1i(glGetUniformLocation(shaderProgram1, "textures"), 1);

    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 2); //设置着色器的纹理单元
    glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 3);

    //激活程序对象
    while (!glfwWindowShouldClose(window))
    {
        pressChange(window);
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window); // 处理输入事件

        glClear(GL_COLOR_BUFFER_BIT); //清空颜色缓冲
        glClearColor(1.0, 1.0, 1.0, 0.0); //设置窗口白色

        glm::mat4 view(1.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection(1.0f); //投影矩阵
        projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);

        glUseProgram(shaderProgram);
        int modelView = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(modelView, 1, GL_FALSE, glm::value_ptr(view));
        int modelProj = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(modelProj, 1, GL_FALSE, glm::value_ptr(projection));

        //矩阵
        glm::mat4 model;
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(1.0f, 1.0f, 1.0f)); //绕y轴旋转
        glUseProgram(shaderProgram);
        unsigned int transformLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE2); //矩形和多边形的
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE3); 
        glBindTexture(GL_TEXTURE_2D, texture2);
        glUniform1f(glGetUniformLocation(shaderProgram, "mixValue"), mixValue);

        glUseProgram(shaderProgram); //立方体的
        glBindVertexArray(VAOS);
        glDrawElements(GL_TRIANGLES, sizeof(index), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0); //解绑VAO

        /*-----------------------------平面-------------------------------------*/
        glUseProgram(shaderProgram1);
        int modelView1 = glGetUniformLocation(shaderProgram1, "view");
        glUniformMatrix4fv(modelView1, 1, GL_FALSE, glm::value_ptr(view));
        int modelProj1 = glGetUniformLocation(shaderProgram1, "projection");
        glUniformMatrix4fv(modelProj1, 1, GL_FALSE, glm::value_ptr(projection));
        glm::mat4 model1;
        model1 = glm::rotate(model1, (float)glfwGetTime() * glm::radians(20.0f), glm::vec3(1.0f, 1.0f, 1.0f)); //绕中心轴旋转
        glUseProgram(shaderProgram1);
        unsigned int transformLoc1 = glGetUniformLocation(shaderProgram1, "model1");
        glUniformMatrix4fv(transformLoc1, 1, GL_FALSE, glm::value_ptr(model1));
        /*-------------------------------------------------------------------------------------------*/

        glActiveTexture(GL_TEXTURE0); //矩形和多边形的
        glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures);
        glUniform1f(glGetUniformLocation(shaderProgram1, "mixValue"), mixValue);

        glUseProgram(shaderProgram1);
        glBindVertexArray(VAO1);
        glDrawElements(GL_TRIANGLES, 15, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0); //解绑VAO

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glfwTerminate();
    return 0;
}
