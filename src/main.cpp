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
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

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


    stbi_set_flip_vertically_on_load(true);
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader boxShader("resources/shaders/boxShader.vs", "resources/shaders/boxShader.fs");
    Shader roomShader("resources/shaders/roomShader.vs","resources/shaders/roomShader.fs");
    Shader treeShader("resources/shaders/treeShader.vs","resources/shaders/treeShader.fs");
    Shader starShader("resources/shaders/starShader.vs","resources/shaders/starShader.fs");
    Shader sladShader("resources/shaders/sladShader.vs","resources/shaders/sladShader.fs");
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

    float room[] = {
            //position    color
            -0.5f, -0.5f, -0.5f, 0.7f,0.5f,0.7f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 0.7f,0.5f,0.7f, 0.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 0.7f,0.5f,0.7f, 0.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 0.7f,0.5f,0.7f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.7f,0.5f,0.7f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.7f,0.5f,0.7f, 0.0f, 0.0f,

            0.5f, 0.5f, 0.5f,    0.7f,0.5f,0.7f, 0.0f, 0.0f,
            0.5f, 0.5f, -0.5f,  0.7f,0.5f,0.7f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  0.7f,0.5f,0.7f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,   0.7f,0.5f,0.7f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f,  0.7f,0.5f,0.7f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f,  0.7f,0.5f,0.7f, 0.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f ,1.0f, 0.0f,
            0.5f, -0.5f, 0.5f,  1.0f, 1.0f, 1.0f ,1.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f
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
    // position attribute


    unsigned int roomVBO, roomVAO;
    glGenVertexArrays(1, &roomVAO);
    glGenBuffers(1, &roomVBO);

    glBindVertexArray(roomVAO);

    glBindBuffer(GL_ARRAY_BUFFER, roomVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(room), room, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);



    unsigned int ng1 = loadTexture(FileSystem::getPath("resources/textures/c1.jpg").c_str());
    unsigned int ng2 = loadTexture(FileSystem::getPath("resources/textures/c2.jpg").c_str());
    unsigned int ng3 = loadTexture(FileSystem::getPath("resources/textures/c3.png").c_str());
    unsigned int floor = loadTexture(FileSystem::getPath("resources/textures/wooden.jpeg").c_str());
    unsigned int slad = loadTexture(FileSystem::getPath("resources/textures/wooden.jpeg").c_str());
    unsigned int star = loadTexture(FileSystem::getPath("resources/textures/base.jpg").c_str());

    Model treeModel("resources/objects/tree/tree.obj");
    Model starModel("resources/objects/star/star.obj");
    Model sladModel("resources/objects/sled/sled.obj");

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------

    roomShader.use();
    roomShader.setInt("floor_texture",15);
    glActiveTexture(GL_TEXTURE15);
    glBindTexture(GL_TEXTURE_2D, floor);

    sladShader.use();
    sladShader.setInt("texture_diffuse1",16);
    glActiveTexture(GL_TEXTURE16);
    glBindTexture(GL_TEXTURE_2D,slad);


    starShader.use();
    starShader.setInt("texturestar",17);
    glActiveTexture(GL_TEXTURE17);
    glBindTexture(GL_TEXTURE_2D,star);
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

        //glActiveTexture(GL_TEXTURE1);
        //glBindTexture(GL_TEXTURE_2D, texture2);

        // activate shader
        boxShader.use();

        glBindVertexArray(VAO);
        // create transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        boxShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        boxShader.setMat4("view", view);

        roomShader.use();


        glBindVertexArray(roomVAO);

        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        roomShader.setMat4("projection", projection);

        view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
        roomShader.setMat4("view", view);

        // calculate the model matrix for each object and pass it to shader before drawing
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model=glm::translate(model,glm::vec3(0.0f,5.0f,0.0f));
        model=glm::scale(model,glm::vec3(13.0f));
        roomShader.setMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 18);
        // render boxes
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 9; i++)
        {
            int n = i%3+1;
            // calculate the model matrix for each object and pass it to shader before drawing
            std::string name = "ng";
            name.append(to_string(n));

            boxShader.use();
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

            boxShader.use();
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


            treeShader.use();

            projection=glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            view = camera.GetViewMatrix();
            view = glm::translate(view,glm::vec3(glm::vec3(0.0f,-1.0f,0.0f)));
            view =glm::rotate(view,glm::radians(-90.0f),glm::vec3(1.0f, 0.0f, 0.0f));
            //view = glm::rotate(view,(float)glfwGetTime(),glm::vec3(0.0f,0.0f,1.0f));
            //view = glm::scale(view, glm::vec3(0.001f, 0.001f, 0.001f));
            treeShader.setMat4("projection", projection);
            treeShader.setMat4("view", view);

            model = glm::mat4(1.0f);
            //model = glm::translate(model,glm::vec3(0.0f+sin(glfwGetTime())*10.0f,-3.0f,0.0f+cos(glfwGetTime())*10.0f));
             //model= glm::rotate(model,(float)glfwGetTime(),glm::vec3(1.0f,0.0f,1.0f));
            model =glm::scale(model,glm::vec3(0.05f,0.05f,0.05f));
            treeShader.setMat4("model", model);
            treeModel.Draw(treeShader);
        }

        //sanke

        sladShader.use();

        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        //view = glm::translate(view,glm::vec3(3.0f,-1.5f,3.0f));
        view = glm::translate(view,-glm::vec3(0.0f+sin(glfwGetTime())*5.0f,1.5f,0.0f+cos(glfwGetTime())*5.0f));
        view =glm::rotate(view,glm::radians(-90.0f),glm::vec3(1.0f, 0.0f, 0.0f));
        view = glm::rotate(view,(float)glfwGetTime(),glm::vec3(0.0f,0.0f,1.0f));
        view = glm::scale(view, glm::vec3(0.1f, 0.1f, 0.1f));
        sladShader.setMat4("projection", projection);
        sladShader.setMat4("view", view);

        model = glm::mat4(1.0f);
        //model = glm::translate(model,glm::vec3(0.0f+sin(glfwGetTime())*10.0f,-3.0f,0.0f+cos(glfwGetTime())*10.0f));
        //model= glm::rotate(model,(float)glfwGetTime(),glm::vec3(1.0f,0.0f,1.0f));
        model =glm::scale(model,glm::vec3(0.15f,0.15f,0.15f));
        sladShader.setMat4("model", model);
        //glDrawArrays(GL_TRIANGLES, 0, 36);

        sladModel.Draw(sladShader);

        starShader.use();

        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        view = glm::translate(view,glm::vec3(0.0f,7.5f,0.2f));
        view =glm::rotate(view,glm::radians(-90.0f),glm::vec3(1.0f, 0.45f, 0.0f));
       // view = glm::rotate(view,glm::radians(0.0f),glm::vec3(0.0f,0.0f,1.0f));
        view = glm::scale(view, glm::vec3(0.05f, 0.05f, 0.05f));
        starShader.setMat4("projection", projection);
        starShader.setMat4("view", view);

        model = glm::mat4(1.0f);
        model=glm::rotate(model,glm::radians(15.0f),glm::vec3(1.0f,0.0f,1.0f));
        //   model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));    // it's a bit too big for our scene, so scale it down
        starShader.setMat4("model", model);
        starModel.Draw(starShader);

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
