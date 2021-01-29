#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>
#include <string>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);


unsigned int loadTexture(const char *path);
unsigned int loadCubemap(vector<std::string>faces);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct PointLight {
    glm::vec3 position;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

Camera camera(glm::vec3(-1.0f, 6.0f, 20.0f));
glm::vec3 lightPos = glm::vec3(2.0,2.0,2.0);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    stbi_set_flip_vertically_on_load(false);
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::vec3 pointLightColors[] = {
            glm::vec3(0.5f, 0.5f, 0.6f),
            glm::vec3(0.3f, 0.3f, 0.7f),
            glm::vec3(0.0f, 0.0f, 0.3f),
            glm::vec3(0.4f, 0.4f, 0.4f)
    };

    PointLight pointLight;
    pointLight.position = lightPos;
    pointLight.ambient = glm::vec3(pointLightColors[0].x * 1.0,  pointLightColors[0].y * 1.0,  pointLightColors[0].z * 1.0);
    pointLight.diffuse = glm::vec3( pointLightColors[0].x ,  pointLightColors[0].y,  pointLightColors[0].z);
    pointLight.specular = glm::vec3( pointLightColors[0].x,  pointLightColors[0].y,  pointLightColors[0].z);
    pointLight.constant = 1.0f;
    pointLight.linear = 0.09f;
    pointLight.quadratic = 0.032f;


    // build and compile our shader zprogram
    // ------------------------------------
    Shader boxShader("resources/shaders/boxShader.vs", "resources/shaders/boxShader.fs");
    Shader roomShader("resources/shaders/roomShader.vs","resources/shaders/roomShader.fs");
    Shader lightCube("resources/shaders/lightCube.vs","resources/shaders/lightCube.fs");
    Shader ourShader("resources/shaders/ourShader.vs","resources/shaders/ourShader.fs");
    Shader skyBoxShader("resources/shaders/skyBox.vs","resources/shaders/skyBox.fs");
    Shader windowShader("resources/shaders/window.vs","resources/shaders/window.fs");
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
            // positions          // normals           // texture coords
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    float skyboxVertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };
    float room[] = {
            //position    color
            /*-0.5f, -0.5f, -0.5f, 0.7f,0.5f,0.7f, 0.0f,  0.0f, -1.0f,0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 0.7f,0.5f,0.7f, 0.0f,  0.0f, -1.0f,0.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 0.7f,0.5f,0.7f, 0.0f,  0.0f, -1.0f,0.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 0.7f,0.5f,0.7f, 0.0f,  0.0f, -1.0f,0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.7f,0.5f,0.7f, 0.0f,  0.0f, -1.0f,0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.7f,0.5f,0.7f, 0.0f,  0.0f, -1.0f,0.0f, 0.0f,
*/
            0.5f, 0.5f, 0.5f,    0.7f,0.5f,0.7f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
            0.5f, 0.5f, -0.5f,  0.7f,0.5f,0.7f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  0.7f,0.5f,0.7f,   1.0f,  0.0f,  0.0f,0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,   0.7f,0.5f,0.7f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f,  0.7f,0.5f,0.7f,   1.0f,  0.0f,  0.0f,0.0f, 0.0f,
            0.5f, 0.5f, 0.5f,  0.7f,0.5f,0.7f,   1.0f,  0.0f,  0.0f,0.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f,  0.0f,0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f , 0.0f, -1.0f,  0.0f,1.0f, 0.0f,
            0.5f, -0.5f, 0.5f,  1.0f, 1.0f, 1.0f , 0.0f, -1.0f,  0.0f,1.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
             -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f
    };

    float transparentVertices[] = {
            // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
            0.0f,  0.5f,  0.0f, 0.0f,  0.0f,
            0.0f, -0.5f,  0.0f, 0.0f,  1.0f,
            1.0f, -0.5f,  0.0f,   1.0f,  1.0f,

            0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
            1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    };


    // world space positions of our cubes
    glm::vec3 cubePositions[] = {
            glm::vec3( -3.0f,  -1.0f,  0.0f),
            glm::vec3( 2.4f,  -1.0f,  -0.5f),
            glm::vec3(-1.5f,-1.0f,0.5f),
            glm::vec3(-1.0f,-1.0f,-1.0f),
            glm::vec3(-0.6f,-1.0f,2.5f),
            glm::vec3( -2.2f,  -1.0f,  1.8f),
            glm::vec3( 0.8f,  -1.0f,  2.2f),
            glm::vec3(1.0f,-1.0f,-1.5f),
            glm::vec3(1.7f,-1.0f,1.0f),
            glm::vec3(1.7f,-0.35f,1.0f),
            glm::vec3( -3.0f,  -0.35f,  0.0f),
            glm::vec3( -0.6f,  -0.35f,  2.5f),
            glm::vec3( 0.8f,  -0.35,  2.2f),
            glm::vec3( -1.0f,  -0.35f,  -1.0f)
    };


    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    unsigned int roomVBO, roomVAO;
    glGenVertexArrays(1, &roomVAO);
    glGenBuffers(1, &roomVBO);
    glBindVertexArray(roomVAO);
    glBindBuffer(GL_ARRAY_BUFFER, roomVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(room), room, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(3);


    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);


    unsigned int ng1 = loadTexture(FileSystem::getPath("resources/textures/c1.jpg").c_str());
    unsigned int ng2 = loadTexture(FileSystem::getPath("resources/textures/c2.jpg").c_str());
    unsigned int ng3 = loadTexture(FileSystem::getPath("resources/textures/c3.png").c_str());
    unsigned int floor = loadTexture(FileSystem::getPath("resources/textures/wooden.jpeg").c_str());
    unsigned int slad = loadTexture(FileSystem::getPath("resources/textures/wooden.jpeg").c_str());
    unsigned int star = loadTexture(FileSystem::getPath("resources/textures/base.jpg").c_str());
    unsigned int window1 = loadTexture(FileSystem::getPath("resources/textures/bur.jpg").c_str());
    //unsigned int window2=loadTexture(FileSystem::getPath("resources/textures/specular.jpg").c_str());


    vector<std::string> faces
            {
                    FileSystem::getPath("resources/textures/skybox/right.jpg"),
                    FileSystem::getPath("resources/textures/skybox/left.jpg"),
                    FileSystem::getPath("resources/textures/skybox/top.jpg"),
                    FileSystem::getPath("resources/textures/skybox/bottom.jpg"),
                    FileSystem::getPath("resources/textures/skybox/front.jpg"),
                    FileSystem::getPath("resources/textures/skybox/back.jpg")
            };
    unsigned int cubemapTexture = loadCubemap(faces);

    Model treeModel(FileSystem::getPath("resources/objects/tree/tree.obj"));
    Model starModel(FileSystem::getPath("resources/objects/star/star.obj"));
    Model sladModel(FileSystem::getPath("resources/objects/sled/sled.obj"));
    Model picModel(FileSystem::getPath("resources/objects/sat/sat.obj"));


    treeModel.SetShaderTextureNamePrefix("material.");
    starModel.SetShaderTextureNamePrefix("material.");
    sladModel.SetShaderTextureNamePrefix("material.");
    picModel.SetShaderTextureNamePrefix("material.");
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------


    roomShader.use();
    roomShader.setInt("floor_texture",15);
    glActiveTexture(GL_TEXTURE15);
    glBindTexture(GL_TEXTURE_2D, floor);


    windowShader.use();
    windowShader.setInt("texture1", 18);

    skyBoxShader.use();
    skyBoxShader.setInt("skybox", 0);
     // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

        // bind textures on corresponding texture units

        ourShader.use();

        ourShader.setVec3("dirLight.direction",  -0.2f, -1.0f, -0.3f);
        ourShader.setVec3("dirLight.ambient", 0.25f, 0.25f, 0.1f);
        ourShader.setVec3("dirLight.diffuse", 0.2f, 0.2f, 0.7f);
        ourShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

        pointLight.position=glm::vec3(4.0*cos(currentFrame),4.0f,4.0*sin(currentFrame));
        ourShader.setVec3("pointLight.position",pointLight.position);
        ourShader.setVec3("pointLight.ambient", pointLight.ambient);
        ourShader.setVec3("pointLight.diffuse",pointLight.diffuse);
        ourShader.setVec3("pointLight.specular",pointLight.specular);
        ourShader.setFloat("material.shininess",32.0f);


        ourShader.setFloat("pointLight.constant",pointLight.constant);
        ourShader.setFloat("pointLight.linear",pointLight.linear);
        ourShader.setFloat("pointLight.quadratic",pointLight.quadratic);


        ourShader.setVec3("spotLight.position", camera.Position);
        ourShader.setVec3("spotLight.direction", camera.Front);
        ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        ourShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("spotLight.constant", 1.0f);
        ourShader.setFloat("spotLight.linear", 0.09);
        ourShader.setFloat("spotLight.quadratic", 0.032);
        ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));



        ourShader.setVec3("viewPosition",camera.Position);

        glm::mat4 projection=glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        view =glm::rotate(view,glm::radians(-90.0f),glm::vec3(1.0f, 0.0f, 0.0f));
        view = glm::translate(view,glm::vec3(glm::vec3(0.0f,-1.0f,-1.0f)));
        //view = glm::rotate(view,(float)glfwGetTime(),glm::vec3(0.0f,0.0f,1.0f));
        //view = glm::scale(view, glm::vec3(0.001f, 0.001f, 0.001f));
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::translate(model,glm::vec3(0.0f+sin(glfwGetTime())*10.0f,-3.0f,0.0f+cos(glfwGetTime())*10.0f));
        //model= glm::rotate(model,(float)glfwGetTime(),glm::vec3(1.0f,0.0f,1.0f));

        model =glm::scale(model,glm::vec3(0.05f,0.05f,0.05f));
        ourShader.setMat4("model", model);
        treeModel.Draw(ourShader);

        //SLAD

        ourShader.setInt("material.texture_diffuse1",16);
        glActiveTexture(GL_TEXTURE16);
        glBindTexture(GL_TEXTURE_2D,slad);

        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        //view = glm::translate(view,glm::vec3(3.0f,-1.5f,3.0f));
        view = glm::translate(view,-glm::vec3(0.0f+sin(glfwGetTime())*5.0f,1.5f,0.0f+cos(glfwGetTime())*5.0f));
        view =glm::rotate(view,glm::radians(-90.0f),glm::vec3(1.0f, 0.0f, 0.0f));
        view = glm::rotate(view,(float)glfwGetTime(),glm::vec3(0.0f,0.0f,1.0f));
        view = glm::scale(view, glm::vec3(0.1f, 0.1f, 0.1f));
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        model = glm::mat4(1.0f);
        //model = glm::translate(model,glm::vec3(0.0f+sin(glfwGetTime())*10.0f,-3.0f,0.0f+cos(glfwGetTime())*10.0f));
        //model= glm::rotate(model,(float)glfwGetTime(),glm::vec3(1.0f,0.0f,1.0f));
        model =glm::scale(model,glm::vec3(0.15f,0.15f,0.15f));
        ourShader.setMat4("model", model);
        //glDrawArrays(GL_TRIANGLES, 0, 36);

        sladModel.Draw(ourShader);

        //star

        ourShader.setInt("material.texture_diffuse1",17);
        glActiveTexture(GL_TEXTURE17);
        glBindTexture(GL_TEXTURE_2D,star);

        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        view = glm::translate(view,glm::vec3(0.0f,7.2f,1.0f));
        view =glm::rotate(view,glm::radians(-90.0f),glm::vec3(-0.8f, 0.0f, 0.0f));
        // view = glm::rotate(view,glm::radians(0.0f),glm::vec3(0.0f,0.0f,1.0f));
        view = glm::scale(view, glm::vec3(0.05f, 0.05f, 0.05f));
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        model = glm::mat4(1.0f);
        model=glm::rotate(model,glm::radians(15.0f),glm::vec3(1.0f,0.0f,1.0f));
        //   model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));    // it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        starModel.Draw(ourShader);

        //clock

        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        view = glm::translate(view,glm::vec3(7.8f,6.0f,-2.0f));
       // view = glm::translate(view,-glm::vec3(0.0f+sin(glfwGetTime())*5.0f,1.5f,0.0f+cos(glfwGetTime())*5.0f));
        view =glm::rotate(view,glm::radians(-90.0f),glm::vec3(0.0f, 1.0f, .0f));
        //view = glm::rotate(view,(float)glfwGetTime(),glm::vec3(0.0f,0.0f,1.0f));
        view = glm::scale(view, glm::vec3(0.3f, 0.3f, 0.3f));
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        model = glm::mat4(1.0f);
        //model = glm::translate(model,glm::vec3(0.0f+sin(glfwGetTime())*10.0f,-3.0f,0.0f+cos(glfwGetTime())*10.0f));
        //model= glm::rotate(model,(float)glfwGetTime(),glm::vec3(1.0f,0.0f,1.0f));
        model =glm::scale(model,glm::vec3(0.15f,0.15f,0.15f));
        ourShader.setMat4("model", model);
        //glDrawArrays(GL_TRIANGLES, 0, 36);

        picModel.Draw(ourShader);

        glBindVertexArray(transparentVAO);


        windowShader.use();

        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        view = glm::translate(view,glm::vec3(-7.55f,6.25f,-10.2f));
        // view = glm::translate(view,-glm::vec3(0.0f+sin(glfwGetTime())*5.0f,1.5f,0.0f+cos(glfwGetTime())*5.0f));
        //view =glm::rotate(view,glm::radians(90.0f),glm::vec3(1.0f, 0.0f, 0.0f));
        //view = glm::rotate(view,(float)glfwGetTime(),glm::vec3(0.0f,0.0f,1.0f));
        view = glm::scale(view, glm::vec3(0.103f, 0.1187f, 0.1f));
        windowShader.setMat4("projection", projection);
        windowShader.setMat4("view", view);

        model = glm::mat4(1.0f);
        model = glm::translate(model,glm::vec3(-4.0f,2.0f,2.0f));
        model = scale(model,glm::vec3(155.0f,135.0f,160.0f));
        windowShader.setMat4("model", model);

        glActiveTexture(GL_TEXTURE18);
        glBindTexture(GL_TEXTURE_2D, window1);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        roomShader.use();

        roomShader.setVec3("viewPos",camera.Position);
        roomShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        roomShader.setVec3("dirLight.ambient", 0.25f, 0.25f, 0.2f);
        roomShader.setVec3("dirLight.diffuse", 0.2f, 0.2f, 0.7f);
        roomShader.setVec3("dirLight.specular", 0.7f, 0.7f, 0.7f);

        roomShader.setVec3("spotLight.position", camera.Position);
        roomShader.setVec3("spotLight.direction", camera.Front);
        roomShader.setVec3("spotLight.ambient", 0.5f, 0.5f, 0.5f);
        roomShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        roomShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        roomShader.setFloat("spotLight.constant", 1.0f);
        roomShader.setFloat("spotLight.linear", 0.09);
        roomShader.setFloat("spotLight.quadratic", 0.032);
        roomShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        roomShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));


        glBindVertexArray(roomVAO);

        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        roomShader.setMat4("projection", projection);

        view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
        roomShader.setMat4("view", view);

        // calculate the model matrix for each object and pass it to shader before drawing
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model=glm::translate(model,glm::vec3(0.0f,6.5f,-2.0f));
        model=glm::scale(model,glm::vec3(16.0f));
        roomShader.setMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 18);


        // render boxes


        boxShader.use();



        boxShader.setVec3("viewPos",camera.Position);

        boxShader.setVec3("pointLight.position",pointLight.position);
        boxShader.setVec3("pointLight.ambient", pointLight.ambient);
        boxShader.setVec3("pointLight.diffuse",pointLight.diffuse);
        boxShader.setVec3("pointLight.specular",pointLight.specular);


        boxShader.setFloat("pointLight.constant",pointLight.constant);
        boxShader.setFloat("pointLight.linear",pointLight.linear);
        boxShader.setFloat("pointLight.quadratic",pointLight.quadratic);

        boxShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        boxShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.1f);
        boxShader.setVec3("dirLight.diffuse", 0.2f, 0.2f, 0.7f);
        boxShader.setVec3("dirLight.specular", 0.7f, 0.7f, 0.7f);


        boxShader.setVec3("spotLight.position", camera.Position);
        boxShader.setVec3("spotLight.direction", camera.Front);
        boxShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        boxShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        boxShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        boxShader.setFloat("spotLight.constant", 1.0f);
        boxShader.setFloat("spotLight.linear", 0.09);
        boxShader.setFloat("spotLight.quadratic", 0.032);
        boxShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        boxShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        glBindVertexArray(VAO);

        // create transformations
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        boxShader.setMat4("projection", projection);

        // camera/view transformation
        view = camera.GetViewMatrix();
        boxShader.setMat4("view", view);

        for (unsigned int i = 0; i < 9; i++)
        {
            int n = i%3+1;
            // calculate the model matrix for each object and pass it to shader before drawing
            std::string name = "ng";
            name.append(to_string(n));


            boxShader.setInt("ng_texture", 14);

            if(name == "ng1") {
                glActiveTexture(GL_TEXTURE14);
                glBindTexture(GL_TEXTURE_2D,ng1);
            }
            if(name == "ng2") {
                glActiveTexture(GL_TEXTURE14);
                glBindTexture(GL_TEXTURE_2D,ng2);
            }

            if(name == "ng3") {
                glActiveTexture(GL_TEXTURE14);
                glBindTexture(GL_TEXTURE_2D, ng3);
            }


            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
            model = glm::rotate(model,angle,glm::vec3(0.0f,1.0f,0.0f));
            boxShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);


        }


        for (unsigned int i = 9; i < 14; i++) {
            // calculate the model matrix for each object and pass it to shader before drawing
            int n = i%3+1;
            // calculate the model matrix for each object and pass it to shader before drawing
            std::string name = "ng";
            name.append(to_string(n));


            boxShader.setInt("ng_texture", 14);

            if(name == "ng1") {
                glActiveTexture(GL_TEXTURE14);
                glBindTexture(GL_TEXTURE_2D,ng1);
            }
            if(name == "ng2") {
                glActiveTexture(GL_TEXTURE14);
                glBindTexture(GL_TEXTURE_2D,ng2);
            }

            if(name == "ng3") {
                glActiveTexture(GL_TEXTURE14);
                glBindTexture(GL_TEXTURE_2D, ng3);
            }

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::scale(model, glm::vec3(0.5, 0.3, 0.5));
            model = glm::rotate(model,angle,glm::vec3(0.0f,1.0f,0.0f));
            boxShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);

        }

        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyBoxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyBoxShader.setMat4("view", view);
        skyBoxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);


       /* lightCube.use();
        lightCube.setMat4("projection", projection);
        lightCube.setMat4("view", view);

        // we now draw as many light bulbs as we have point lights.
        glBindVertexArray(cubeVAO);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
        lightCube.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
*/
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}