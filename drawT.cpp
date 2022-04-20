#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    float vertices[6] = {//三个顶点的位置(我们这里把顶点定义为二维的了，但是learnopengl教程中将其定义为3维的，且大作业中也需要定义为3维的
        -0.5f, -0.5f, //点（-0.5f, -0.5f）
         0.0f,  0.5f,// 点（ 0.0f,  0.5f)
         0.5f, -0.5f
    };

    unsigned int buffer;//它相当于learnopengl教程中的vbo
    glGenBuffers(1, &buffer);//创建缓冲区,第一个参数表示创建的缓冲个数，参数二表示生成的缓冲区对象的id
    glBindBuffer(GL_ARRAY_BUFFER, buffer);//绑定创建的buffer,第一个参数表示当前的缓冲类型，我们创建的是顶点缓冲对象，对应的是GL_ARRAY_BUFFER缓冲类型，同时第二个参数是我们传入的缓冲区的id
   
  /*
   * glBufferData是一个专门用来把用户定义的数据复制到当前绑定缓冲的函数。

        第一个参数：指定目标缓冲的类型，顶点缓冲对象应绑定到GL_ARRAY_BUFFER目标。
        第二个参数：指定传输数据的大小(以字节为单位)，我们用sizeof计算出顶点数据
        大小。
        第三个参数：我们希望发送的实际数据。
        第四个参数：指定了我们希望显卡如何管理给定的数据。

   */
    // vertices中的data，通过 glBufferData 拷贝到 gpu 的显存中
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //下面两句，是在设置顶点属性，也是在解释传入gpu的数据的布局
    /*使用glVertexAttribPointer函数告诉OpenGL该如何解析顶点数据（应用到逐个顶点属性上）,顶点着色器会对顶点属性进行处理*/

   /*
    第一个参数指定我们要配置的顶点属性。即我们数据的哪一部分对应哪个顶点属性。（顶点着色器会对顶点属性进行处理）
    这里我们的数据中只有顶点的位置坐标（之后可能会添加纹理坐标，顶点颜色等数据），所以只需要将这里的位置坐标和顶点着色器的顶点位置属性对应起来
    第一个参数为0表示的意思是，后面4个参数确定下来的这些数据对应的是顶点着色器中位置值（索引值 location)为0的那个顶点属性
   （我们之后会在顶点着色器中使layout(location = 0)来定义一个“position” 顶点属性，它表示把"position"顶点属性的位置值设置为0了。）
    因为我们希望把顶点的位置数据传递到这一个顶点属性中，所以 glVertexAttribPointer(）中第一个参数我们传入0。
 
    第二个参数指定我们正在配置的数据的大小，目前我们正在配置顶点位置数据（实际上目前我们也只有该数据），我们的顶点位置是用x,y坐标表示的，是2个float类型的数，
    所以第2个参数是2，第三个参数是3
    第三个参数指定数据的类型，这里是GL_FLOAT
    第四个参数定义我们是否希望数据被标准化(Normalize)。如果我们设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间。我们把它设置为GL_FALSE。
    第五个参数叫做步长(Stride)，你可以将其理解为两个顶点之间的间隔，我们前面定义的数据（vertices)中一共有三个顶点，从第一个顶点到第二个顶点，
    中间只间隔了第一个顶点的位置坐标，即经过两个float数据，就会从一个顶点到另一个顶点。所以我们把步长设置为3 * sizeof(float)。
    最后一个参数的类型是void*，所以需要我们进行这个奇怪的强制类型转换。它表示位置数据在缓冲中起始位置的偏移量(Offset)。由于位置数据在vertices数组的开头，所以这里是0。
    
    基本上第3、4个参数不用改变，当数据布局发生改变时你应该改变其他三个参数
   */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    // 配置完顶点属性后，记得启用它
    glEnableVertexAttribArray(0);

    //顶点着色器的源码
    const char* vertexShaderSource = "#version 330 core\n" //声明版本，opengl3.3
        "layout (location = 0) in vec2 aPos;\n"/*使用in关键字，在顶点着色器中声明所有的输入顶点属性(Input Vertex Attribute)。
                                               现在我们只有位置(Position)数据，所以我们只需要一个顶点属性。
                                               我定义的每个顶点的位置坐标是2维的，所以创建一个vec2输入变量aPos。用于接收位置坐标数据。
                                               我们同样也通过layout (location = 0)设定了输入变量（位置属性）的位置值(Location)*/
        "void main()\n"//每一行的\n别忘记写了
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y,0.0, 1.0);\n"/*为了设置顶点着色器的输出，我们必须把位置数据赋值给预定义gl_Position变量，
                                                           它在幕后是vec4类型的。在main函数的最后，我们将gl_Position设置的值会成为该顶点着色器的输出。
                                                           由于我们的输入是一个2分量的向量，我们必须把它转换为4分量的。把z坐标设为0，w坐标设为1即可*/
        "}\0";

    const char* fragmentShaderSource =  //片段着色器的源码
        " #version 330 core\n"
        " out vec4 FragColor;\n"//输出

        " void main()\n"
        " {\n"
        "   FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);\n"
        " }\n";

    //创建一个顶点着色器对象
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);//CreateShader返回类型为unsigned int ,返回的是创建出的着色器的id

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);//将前面写的着色器源码（vertexShaderSource）附加给着色器对象（vertexShader）
    glCompileShader(vertexShader);//编译这个着色器对象

    //检查编译顶点着色器时是否发生了错误
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

    //检查编译片段着色器时是否发生了错误
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

    //检查链接过程中是否有错误
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::shader link failed\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);//删除着色器对象，已经链接完了，不需要它们了
    glDeleteShader(fragmentShader);



    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);//清空颜色缓冲

         //使用shaderProgram着色器程序
        glUseProgram(shaderProgram);
        //绘制指令
         //三个参数分别为 绘制模式；指定启用的数组中的起始索引(从vertices[0]开始读取）;要呈现的索引数量，一共三个顶点，所以三个索引
        glDrawArrays(GL_TRIANGLES, 0, 3);
       
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
