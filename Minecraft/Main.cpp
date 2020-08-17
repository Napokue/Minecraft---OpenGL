#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "Header Files/Shaders.h"

#include "Header Files/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Player {
private:
    const float locationBoundsOffset = 0.2f;
    int upKey;
    int downKey;
    float playerLocation = 0.0f;

public:
    const float playerWidth = 10.0f;
    float playerVertices[6];

    Player(float startLocation, int glfwUpKey, int glfwDownKey)
        : playerVertices{
        // positions       
         startLocation, -locationBoundsOffset, 0.0f,
         startLocation, locationBoundsOffset, 0.0f,
        }, 
        upKey(glfwUpKey),
        downKey(glfwDownKey)
    {}

    int GetUpKey()
    {
        return upKey;
    }

    int GetDownKey()
    {
        return downKey;
    }

    float GetUpLocation()
    {
        return playerLocation + locationBoundsOffset;
    }

    float GetDownLocation()
    {
        return playerLocation - locationBoundsOffset;
    }

    float GetNoOffsetLocation()
    {
        return playerLocation;
    }
    
    glm::vec2 GetBoundsLocation()
    {
        return glm::vec2(GetUpLocation(), GetDownLocation());
    }

    float MoveUp(float amount)
    {
        if (GetUpLocation() >= 1.0f)
        {
            playerLocation = 1.0f - locationBoundsOffset;
        }
        else
        {
            playerLocation += amount;
        }

        return GetUpLocation();
    }

    float MoveDown(float amount)
    {
        if (GetDownLocation() <= -1.0f)
        {
            playerLocation = -1.0f + locationBoundsOffset;
        }
        else
        {
            playerLocation -= amount;
        }

        return GetDownLocation();
    }
};

class PlayerRenderer {
private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int playerMovementUniform;
public:
    Player player;
    Shader playerShader;

    PlayerRenderer(Player player, Shader shader)
        : player(player), playerShader(shader)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(player.playerVertices), player.playerVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        playerMovementUniform = glGetUniformLocation(shader.ID, "movementTransform");
    }

    void RenderPlayer()
    {
        playerShader.use();

        glUniform1f(playerMovementUniform, player.GetNoOffsetLocation());
        glBindVertexArray(VAO);
        glLineWidth(player.playerWidth);
        glDrawArrays(GL_LINES, 0, 2);
    }

    void ProcessInput(GLFWwindow* window)
    {
        if (glfwGetKey(window, player.GetUpKey()) == GLFW_PRESS)
        {
            player.MoveUp(0.01f);
        }
        if (glfwGetKey(window, player.GetDownKey()) == GLFW_PRESS)
        {
            player.MoveDown(0.01f);
        }
    }

    ~PlayerRenderer()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
};

struct Ball {
private:
    const float radius;
    glm::vec2 position;
    float ballVertices[];
public:
    
    Ball(glm::vec2 startPosition, float radius = 0.2f)
        : position(startPosition),
        radius(radius)
        //ballVertices{
        //    position.y , position.x + radius, 0.0f,
        //    position.y - radius, position.x - radius, 0.0f,
        //    position.y + radius, position.x - radius, 0.0f,
        //}
    {

    }

    void* getVertices()
    {
        ballVertices = {
            position.y , position.x + radius, 0.0f,
            position.y - radius, position.x - radius, 0.0f,
            position.y + radius, position.x - radius, 0.0f,

        };

        return ballVertices;
    }
};

class BallRenderer {
private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int playerMovementUniform;
public:
    Ball ball;
    Shader ballShader;

    BallRenderer(Ball ball, Shader ballShader)
        : ball(ball), ballShader(ballShader)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        float vertices[9] = ball.getVertices();

        //glBufferData(GL_ARRAY_BUFFER, sizeof((float[])ball.getVertices()), ball.ballVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        //playerMovementUniform = glGetUniformLocation(ballShader.ID, "movementTransform");
    }

    void RenderBall()
    {
        ballShader.use();

        //glUniform1f(playerMovementUniform, player.GetNoOffsetLocation());
        glBindVertexArray(VAO);
        //glLineWidth(ball.playerWidth);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    ~BallRenderer()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
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

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader ballShader("Shaders/Ball.vert", "Shaders/Fragment.frag");
    Shader playerShader("Shaders/Player.vert", "Shaders/Fragment.frag");
    
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    const float vertices[] = {
        // positions          // colors           // texture coords
         0.1f,  0.1f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.1f, -0.1f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.1f, -0.1f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.1f,  0.1f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int textures[2];
    glGenTextures(2, textures);
    glBindTexture(GL_TEXTURE_2D, textures[0]);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load("Textures/wall.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, textures[1]);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // make the image flip when loaded
    stbi_set_flip_vertically_on_load(true);

    // load and generate the texture
    data = stbi_load("Textures/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // no longer needed to flip the images that will load
    stbi_set_flip_vertically_on_load(false);

    PlayerRenderer playerRenderers[] = {
        PlayerRenderer(Player(-0.98f, GLFW_KEY_UP, GLFW_KEY_DOWN), playerShader),
        PlayerRenderer(Player(0.98f, GLFW_KEY_W, GLFW_KEY_S), playerShader),
    };

    const float playerVertices[] = {
        // positions       
         -0.98f, -0.2f, 0.0f,
         -0.98f, 0.2f, 0.0f,
    };

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    ballShader.setInt("texture1", 0);
    ballShader.setInt("texture2", 1);

    float alpha = 0.2f;

    unsigned int transformLoc = glGetUniformLocation(ballShader.ID, "transform");

    float translationOffset = 0.5f;

    bool rightDirection = false;

    BallRenderer renderer = BallRenderer(Ball(glm::vec2(0.8f, 0.0f)), ballShader);
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        for (PlayerRenderer& renderer : playerRenderers)
        {
            renderer.ProcessInput(window);
        }

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 trans = glm::mat4(1.0f);

        bool shouldCheckBounds = false;
        if (rightDirection)
        {
            translationOffset += 0.01f;
        }
        else
        {
            translationOffset -= 0.01f;
        }

        if (translationOffset <= -0.9f)
        {
            rightDirection = true;
            shouldCheckBounds = true;
        }
        else if (translationOffset >= 0.9f)
        {
            rightDirection = false;
            shouldCheckBounds = true;
        }

        trans = glm::translate(trans, glm::vec3(translationOffset, 0.0f, 0.0f));

        //trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

        for (PlayerRenderer& renderer : playerRenderers)
        {
            if (shouldCheckBounds)
            {
                glm::vec2 playerBounds = renderer.player.GetBoundsLocation();
                //if (playerBounds.x >= )
            }
            renderer.RenderPlayer();
        }

        renderer.RenderBall();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);

        ballShader.use();

        ballShader.setFloat("alpha", alpha);

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
        
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE)== GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}




