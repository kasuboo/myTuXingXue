#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);


    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    float vertices[12] = {//三个顶点的位置坐标，以及纹理坐标
        -0.5f, -0.5f, 0.0f, 0.0f, // 左下角，点（-0.5f, -0.5f）对应的纹理坐标是（0.0f, 0.0f）
         0.0f, 0.5f, 0.5f, 1.0f, // 上中，点(0.0f, 0.5f) 对应的纹理坐标是（0.5f, 1.0f ）
         0.5f, -0.5f, 1.0f, 0.0f // 右下角,点( 0.5f, -0.5f) 对应的纹理坐标是 （1.0f, 0.0f）
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);//创建buffer,第一个参数表示创建buffer的个数，参数二表示生成的缓冲区对象的id
    glBindBuffer(GL_ARRAY_BUFFER, buffer);//绑定创建的buffer,第一个参数表示当前的缓冲类型，我们创建的是顶点缓冲对象，对应的是GL_ARRAY_BUFFER缓冲类型，同时第二个参数是我们传入的缓冲区的id

    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
    glEnableVertexAttribArray(1);

    unsigned int texture;//创建纹理

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);//绑定纹理

    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //下面这句用来翻转图片y轴，因为图片y轴正方向的定义往往和opengl的相反，所以在加载图片前加上这一句
    stbi_set_flip_vertically_on_load(true);
    
    int width, height, nrChannels;
    unsigned char* data = stbi_load("myTexturePic.jpeg", &width, &height, &nrChannels, 0);//注意自己纹理图片的格式,可能是png,jpg等
    //注意，这里输出的channels数如果是3，说明没有透明度通道 glTexImage2D中应该是GL_RGB,而非GL_RGBA
    std::cout << width << " " << height << " " << nrChannels << std::endl;
    if (data)
    {
        //用载入的图片数据生成一个纹理，纹理可以通过glTexImage2D来生成：
       // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);//生成多级渐远纹理
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);//释放内存



    //顶点着色器的源码
    const char* vertexShaderSource = "#version 330 core\n" //声明版本，opengl3.3
        "layout (location = 0) in vec2 aPos;\n"
        "layout (location = 1) in vec2 atexCord;\n"

        "out vec2 myTexCord;\n"//顶点着色器的输出会直接送到片段着色器，作为片段着色器的输入
      
        " uniform mat4 transform;\n"//修改顶点着色器让其接收一个mat4的uniform变量

        "void main()\n"
        "{\n"
        "   gl_Position = transform* vec4(aPos.x, aPos.y,0.0, 1.0);\n"//用接收的变换矩阵乘以顶点位置坐标
        "   myTexCord = atexCord;\n"

        "}\0";

    const char* fragmentShaderSource =  //片段着色器的源码
        " #version 330 core\n"
        " out vec4 FragColor;\n"

        " in vec2 myTexCord;\n"//从顶点着色器接收输入

        " uniform sampler2D ourTexture;\n"//定义一个uniform采样器，把一个纹理添加到片段着色器中，稍后我们会把纹理赋值给这个uniform。

        " void main()\n"
        " {\n"
        // "   FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);\n"
         //最终片段着色器输出的各像素的颜色值由纹理采样器从纹理中采样获得
         /*我们使用GLSL内建的texture函数来采样纹理的颜色，它第一个参数是纹理采样器，第二个参数是对应的纹理坐标。
         texture函数会使用之前设置的纹理参数对相应的颜色值进行采样。*/
        " FragColor =  texture(ourTexture, myTexCord);\n"
        " }\n";

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);//创CreateShader返回类型为unsigned int ,返回的是创建出的着色器的id

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);//将前面写的着色器源码（vertexShaderSource）附加给着色器对象（vertexShader）
    glCompileShader(vertexShader);//编译这个着色器对象

 //检查顶点着色器对象是否编译成功
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    //检查片段着色器是否编译成功
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram(); //创建着色器程序对象，着色器程序对象(Shader Program Object)是多个着色器合并之后并最终链接完成的版本。

    glAttachShader(shaderProgram, vertexShader);//将前面创建的顶点着色器对象附加到着色器程序对象上
    glAttachShader(shaderProgram, fragmentShader);//将前面创建的片段着色器对象附加到着色器程序对象上
    glLinkProgram(shaderProgram);//链接程序

    //检查程序是否链接成功
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::shader link failed\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    ////定义向量vec 坐标为（1.0,0.0,0.0） 第四个分量1.0是齐次坐标
    //glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    //std::cout<<"原向量为："<<"(" << vec.x << "," << vec.y << "," << vec.z << ")" << std::endl;

    ////定义一个4x4的单位矩阵作为初始变换矩阵
    //glm::mat4 trans = glm::mat4(1.0f);

    ////单位矩阵和位移向量(1,1,0)传递给glm::translate函数得到最终的位移变换矩阵
    //trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f)); 

    ////将变换矩阵乘以原向量得到经过位移变换后的向量
    //vec = trans * vec;
    //std::cout << "经过位移变换后向量变为了：" << "(" << vec.x << "," << vec.y << "," << vec.z << ")" << std::endl;

    //单位矩阵，初始的变换矩阵
    glm::mat4 trans = glm::mat4(1.0f);
    //旋转90度（转换成了弧度），绕（0,0,1）即z轴旋转
    trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    //缩放，放大到原来的两倍
    trans = glm::scale(trans, glm::vec3(2.0, 2.0, 2.0));


    glUseProgram(shaderProgram);//设置uniform之前一定要先激活程序！对于所有的uniform都一样
    //获取uniform变量tansform的地址 ，如果没有找到则会返回-1，（debug时可以打印出来确认一下),shaderProgram为前面定义的着色器程序
    unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
   /*通过glUniform4f函数设置uniform值
     参数1：uniform变量 transform的位置值
     参数2：要发送的矩阵的个数 此处为1
     参数3：是否需要对矩阵进行转置
     最后一个参数是真正的矩阵数据，但是GLM并不是把它们的矩阵储存为OpenGL所希望接受的那种，
     因此我们要先用GLM的自带的函数value_ptr来变换这些数据。
     */
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        //使用程序对象
        glUseProgram(shaderProgram);
        //绘制指令,三个参数分别为 绘制模式；指定启用的数组中的起始索引(从postions[0]开始读取）;要呈现的索引数量，一共三个顶点，所以三个索引
        glDrawArrays(GL_TRIANGLES, 0, 3);
  
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
