#include <iostream>
#include <vector>
#include <cmath>

//: GLM для матриц
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

///////мои константы
const float MASHTAB = 0.0000001f;
const float W = 7.2921150e-5f ;
const float MY_PI = 3.1415926535f;
const float G =  6.67430e-11f;
const float EARTH_MASS = 5.9722e24f;
const float SPUTNIK_MASS =1400.0f;
const float ENERGY = -1.0e3f;
const float GM = G * EARTH_MASS;
///////мои константы


///////
float gpsHeight = 20200000.0f;
float earthRadius = 6371000.0f;
float orbitRadius = (earthRadius + gpsHeight);
float SPUTNIK_RADIAN = 55.0f * (MY_PI / 180.0f);
//начальное положение спутника моего
float orbitAngle = 0.0f;//для проверки  в основном коде не нужен
float x = orbitRadius * sin(orbitAngle) * cos(SPUTNIK_RADIAN);
float y = orbitRadius * cos(orbitAngle);
float z = orbitRadius * sin(orbitAngle) * sin(SPUTNIK_RADIAN);
////
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>


// тут мы  будем менять полодение центра масс спутникач
float sat_x, sat_y, sat_z;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

const GLuint WIDTH = 800, HEIGHT = 600;


// тцт щеёдеры
const GLchar* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n"
    "layout (location = 1) in vec3 color;\n"
    "uniform mat4 projection;\n"
    "uniform mat4 view;\n"
    "out vec3 fragColor;\n"
    "void main()\n"
    "{\n"
    "    fragColor = color;\n"
    "    // ТВОЯ СИСТЕМА КООРДИНАТ:\n"
    "    // position.x = вглубь (чем больше, тем дальше от нас)\n"
    "    // position.y = вправо (горизонталь)\n"
    "    // position.z = вверх (вертикаль)\n"
    "    \n"
    "    // Преобразуем твою систему в стандартную OpenGL:\n"
    "    // OpenGL: x = вправо, y = вверх, z = из экрана (отрицательная глубина)\n"
    "    vec3 pos = vec3(position.y, position.z, position.x);\n"
    "    \n"
    "    gl_Position = projection * view * vec4(pos, 1.0);\n"
    "}\0";

const GLchar* fragmentShaderSource = "#version 330 core\n"
    "in vec3 fragColor;\n"
    "out vec4 color;\n"
    "void main()\n"
    "{\n"
    "    color = vec4(fragColor, 1.0f);\n"
    "}\n\0";

////////////
struct Camera {
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    float yaw;
    float pitch;
    float movementSpeed;
    float rotationSpeed;
};

Camera camera;
bool keys[1024];

void updateCameraVectors() {
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    newFront.y = sin(glm::radians(camera.pitch));
    newFront.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    camera.front = glm::normalize(newFront);
    
    camera.right = glm::normalize(glm::cross(camera.front, camera.worldUp));
    camera.up = glm::normalize(glm::cross(camera.right, camera.front));
}

void initCamera() {
    camera.position = glm::vec3(0.0f, 0.0f, 15.0f);
    camera.front = glm::vec3(0.0f, 0.0f, -1.0f);
    camera.worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    camera.up = glm::vec3(0.0f, 1.0f, 0.0f);
    camera.yaw = -90.0f;
    camera.pitch = 0.0f;
    camera.movementSpeed = 5.0f;
    camera.rotationSpeed = 2.0f;
    updateCameraVectors();
}

void processCameraMovement(float dt) {
    float velocity = camera.movementSpeed * dt;
    
    if (keys[GLFW_KEY_W])
        camera.position += camera.front * velocity;
    if (keys[GLFW_KEY_S])
        camera.position -= camera.front * velocity;
    if (keys[GLFW_KEY_A])
        camera.position -= camera.right * velocity;
    if (keys[GLFW_KEY_D])
        camera.position += camera.right * velocity;
    if (keys[GLFW_KEY_Q])
        camera.position += camera.worldUp * velocity;
    if (keys[GLFW_KEY_E])
        camera.position -= camera.worldUp * velocity;
    
    float rotationVelocity = camera.rotationSpeed * dt * 50.0f;
    bool rotated = false;
    
    if (keys[GLFW_KEY_LEFT]) {
        camera.yaw -= rotationVelocity;
        rotated = true;
    }
    if (keys[GLFW_KEY_RIGHT]) {
        camera.yaw += rotationVelocity;
        rotated = true;
    }
    if (keys[GLFW_KEY_UP]) {
        camera.pitch += rotationVelocity;
        if (camera.pitch > 89.0f) camera.pitch = 89.0f;
        rotated = true;
    }
    if (keys[GLFW_KEY_DOWN]) {
        camera.pitch -= rotationVelocity;
        if (camera.pitch < -89.0f) camera.pitch = -89.0f;
        rotated = true;
    }
    
    if (keys[GLFW_KEY_R]) {
        camera.position = glm::vec3(0.0f, 0.0f, 15.0f);
        camera.yaw = -90.0f;
        camera.pitch = 0.0f;
        rotated = true;
    }
    
    if (rotated) updateCameraVectors();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    
    if (action == GLFW_PRESS)
        keys[key] = true;
    else if (action == GLFW_RELEASE)
        keys[key] = false;
}

/////////
struct sputnik{
    std::vector<float>vertices;
    std::vector<float>n_vertices;
    std::vector<float>colors;
    std::vector<unsigned int> indices;
    float center[3];
    float MASS;
};

sputnik genSputnik(float q, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f){
    sputnik MIR;
    
    MIR.center[0] = centerX;
    MIR.center[1] = centerY;
    MIR.center[2] = centerZ;
    
    float R = q/2.0f;
    
    MIR.vertices.push_back(centerX + R);
    MIR.vertices.push_back(centerY - R);
    MIR.vertices.push_back(centerZ + R);
    
    MIR.vertices.push_back(centerX + R);
    MIR.vertices.push_back(centerY - R);
    MIR.vertices.push_back(centerZ - R);
    
    MIR.vertices.push_back(centerX - R);
    MIR.vertices.push_back(centerY - R);
    MIR.vertices.push_back(centerZ + R);
    
    MIR.vertices.push_back(centerX - R);
    MIR.vertices.push_back(centerY - R);
    MIR.vertices.push_back(centerZ - R);
    
    MIR.vertices.push_back(centerX - R);
    MIR.vertices.push_back(centerY + R);
    MIR.vertices.push_back(centerZ + R);
    
    MIR.vertices.push_back(centerX - R);
    MIR.vertices.push_back(centerY + R);
    MIR.vertices.push_back(centerZ - R);
    
    MIR.vertices.push_back(centerX + R);
    MIR.vertices.push_back(centerY + R);
    MIR.vertices.push_back(centerZ - R);
    
    MIR.vertices.push_back(centerX + R);
    MIR.vertices.push_back(centerY + R);
    MIR.vertices.push_back(centerZ + R);

    MIR.vertices.push_back(centerX + R/2);
    MIR.vertices.push_back(centerY + 2*R);
    MIR.vertices.push_back(centerZ - R/2);

    MIR.vertices.push_back(centerX + R/2);
    MIR.vertices.push_back(centerY + 2*R);
    MIR.vertices.push_back(centerZ + R/2);

    MIR.vertices.push_back(centerX + R/2);
    MIR.vertices.push_back(centerY + R);
    MIR.vertices.push_back(centerZ + R/2);
    
    MIR.vertices.push_back(centerX + R/2);
    MIR.vertices.push_back(centerY + R);
    MIR.vertices.push_back(centerZ - R/2);
    
    MIR.vertices.push_back(centerX + R/2);
    MIR.vertices.push_back(centerY - 2*R);
    MIR.vertices.push_back(centerZ - R/2);

    MIR.vertices.push_back(centerX + R/2);
    MIR.vertices.push_back(centerY - 2*R);
    MIR.vertices.push_back(centerZ + R/2);

    MIR.vertices.push_back(centerX + R/2);
    MIR.vertices.push_back(centerY - R);
    MIR.vertices.push_back(centerZ + R/2);
    
    MIR.vertices.push_back(centerX + R/2);
    MIR.vertices.push_back(centerY - R);
    MIR.vertices.push_back(centerZ - R/2);
    
    MIR.n_vertices = MIR.vertices;
    
    MIR.indices.push_back(2); MIR.indices.push_back(3); MIR.indices.push_back(5);
    MIR.indices.push_back(2); MIR.indices.push_back(5); MIR.indices.push_back(4);
    
    MIR.indices.push_back(0); MIR.indices.push_back(1); MIR.indices.push_back(6);
    MIR.indices.push_back(0); MIR.indices.push_back(6); MIR.indices.push_back(7);
    
    MIR.indices.push_back(1); MIR.indices.push_back(3); MIR.indices.push_back(5);
    MIR.indices.push_back(1); MIR.indices.push_back(5); MIR.indices.push_back(6);
    
    MIR.indices.push_back(0); MIR.indices.push_back(2); MIR.indices.push_back(4);
    MIR.indices.push_back(0); MIR.indices.push_back(4); MIR.indices.push_back(7);
    
    MIR.indices.push_back(0); MIR.indices.push_back(1); MIR.indices.push_back(3);
    MIR.indices.push_back(0); MIR.indices.push_back(3); MIR.indices.push_back(2);
    
    MIR.indices.push_back(4); MIR.indices.push_back(5); MIR.indices.push_back(6);
    MIR.indices.push_back(4); MIR.indices.push_back(6); MIR.indices.push_back(7);
    
    MIR.indices.push_back(8); MIR.indices.push_back(9); MIR.indices.push_back(10);
    MIR.indices.push_back(8); MIR.indices.push_back(10); MIR.indices.push_back(11);
    
    MIR.indices.push_back(12); MIR.indices.push_back(13); MIR.indices.push_back(14);
    MIR.indices.push_back(12); MIR.indices.push_back(14); MIR.indices.push_back(15);
    
    for(int i = 0; i < 8; i++) {
        MIR.colors.push_back(1.0f);
        MIR.colors.push_back(1.0f); 
        MIR.colors.push_back(1.0f);
    }
    
    for(int i = 0; i < 8; i++) {
        MIR.colors.push_back(1.0f);
        MIR.colors.push_back(0.0f);
        MIR.colors.push_back(0.0f);
    }
    MIR.MASS = SPUTNIK_MASS;
    return MIR;
}

void update_satellite_position(float dt) {
    float r = sqrt(sat_x*sat_x + sat_y*sat_y + sat_z*sat_z);
    float r_xy = sqrt(sat_x*sat_x + sat_y*sat_y);
    
    if (r < 1e-10f) r = 1e-10f;
    if (r_xy < 1e-10f) r_xy = 1e-10f;
    
    float v_radial = sqrt(2.0f * (GM / r + ENERGY / SPUTNIK_MASS) - (r * W) * (r * W));
    
    float vx = v_radial * (sat_x / r) - (r * W) * (sat_y / r_xy);
    float vy = v_radial * (sat_y / r) + (r * W) * (sat_x / r_xy);
    float vz = v_radial * (sat_z / r);
    
    sat_x += vx * dt;
    sat_y += vy * dt;
    sat_z += vz * dt;
}

void update_sputnik_vertices(sputnik& sat, float centerX, float centerY, float centerZ, float size) {
    sat.vertices.clear();
    float R = size / 2.0f;
    
    // 8 вершин куба
    sat.vertices.push_back(centerX + R);
    sat.vertices.push_back(centerY - R);
    sat.vertices.push_back(centerZ + R);
    
    sat.vertices.push_back(centerX + R);
    sat.vertices.push_back(centerY - R);
    sat.vertices.push_back(centerZ - R);
    
    sat.vertices.push_back(centerX - R);
    sat.vertices.push_back(centerY - R);
    sat.vertices.push_back(centerZ + R);
    
    sat.vertices.push_back(centerX - R);
    sat.vertices.push_back(centerY - R);
    sat.vertices.push_back(centerZ - R);
    
    sat.vertices.push_back(centerX - R);
    sat.vertices.push_back(centerY + R);
    sat.vertices.push_back(centerZ + R);
    
    sat.vertices.push_back(centerX - R);
    sat.vertices.push_back(centerY + R);
    sat.vertices.push_back(centerZ - R);
    
    sat.vertices.push_back(centerX + R);
    sat.vertices.push_back(centerY + R);
    sat.vertices.push_back(centerZ - R);
    
    sat.vertices.push_back(centerX + R);
    sat.vertices.push_back(centerY + R);
    sat.vertices.push_back(centerZ + R);
    
    // 8 вершин антенн
    float ant_offset = R * 0.5f;
    
    sat.vertices.push_back(centerX + ant_offset);
    sat.vertices.push_back(centerY + 2*R);
    sat.vertices.push_back(centerZ - ant_offset);
    
    sat.vertices.push_back(centerX + ant_offset);
    sat.vertices.push_back(centerY + 2*R);
    sat.vertices.push_back(centerZ + ant_offset);
    
    sat.vertices.push_back(centerX + ant_offset);
    sat.vertices.push_back(centerY + R);
    sat.vertices.push_back(centerZ + ant_offset);
    
    sat.vertices.push_back(centerX + ant_offset);
    sat.vertices.push_back(centerY + R);
    sat.vertices.push_back(centerZ - ant_offset);
    
    sat.vertices.push_back(centerX + ant_offset);
    sat.vertices.push_back(centerY - 2*R);
    sat.vertices.push_back(centerZ - ant_offset);
    
    sat.vertices.push_back(centerX + ant_offset);
    sat.vertices.push_back(centerY - 2*R);
    sat.vertices.push_back(centerZ + ant_offset);
    
    sat.vertices.push_back(centerX + ant_offset);
    sat.vertices.push_back(centerY - R);
    sat.vertices.push_back(centerZ + ant_offset);
    
    sat.vertices.push_back(centerX + ant_offset);
    sat.vertices.push_back(centerY - R);
    sat.vertices.push_back(centerZ - ant_offset);
}
/////
struct Planet {
    std::vector<float> vertices;
    std::vector<float> n_vertices;
    std::vector<float> colors;
    std::vector<unsigned int> indices;
    float MASS;
};
//////
Planet genPlanet(float radius, int Ashag, int Bshag){
    Planet earth;

    for(int i =0; i <= Ashag; i++){
        float phi = (MY_PI * i) / Ashag;
        
        for(int j = 0; j <= Bshag; j++){
            float teta = (2.0f * MY_PI * j) / Bshag;	
            earth.vertices.push_back((radius * sin(phi) * cos(teta))* MASHTAB);
            earth.vertices.push_back((radius * cos(phi))* MASHTAB);
            earth.vertices.push_back((radius * sin(phi) * sin(teta))* MASHTAB);
        }
    }
    
    earth.colors = std::vector<float>(earth.vertices.size(), 0.0f);
    
    int verticesPerRow = Bshag + 1;
    
    for(int i = 0; i < Ashag; i++) {
        for(int j = 0; j < Bshag; j++) {
            int topLeft = i * verticesPerRow + j;
            int topRight = topLeft + 1;
            int bottomLeft = topLeft + verticesPerRow;
            int bottomRight = bottomLeft + 1;
            
            earth.indices.push_back(topLeft);
            earth.indices.push_back(bottomLeft);
            earth.indices.push_back(topRight);
            
            earth.indices.push_back(bottomLeft);
            earth.indices.push_back(bottomRight);
            earth.indices.push_back(topRight);
            
            float r, g, b;
            if ((i >= 3 && i < 11 && j >= 5 && j < 13) ||    // квадрат 1
                (i >= 3 && i < 11 && j >= 18 && j < 26) ||   // квадрат 2
                (i >= 15 && i < 23 && j >= 7 && j < 15) ||   // квадрат 3  
                (i >= 18 && i < 26 && j >= 22 && j < 30) ||  // квадрат 4
                (i >= 10 && i < 14 && j >= 10 && j < 14) ||  // остров 1
                (i >= 5 && i < 9 && j >= 25 && j < 29) ||    // остров 2
                (i >= 20 && i < 24 && j >= 2 && j < 6) ||    // остров 3
                (i >= 25 && i < 29 && j >= 15 && j < 19) ||  // остров 4
                (i >= 12 && i < 16 && j >= 30 && j < 34)) {  // остров 5
                r = 0.0f; g = 0.8f; b = 0.0f;
            } else {
                r = 0.0f; g = 0.0f; b = 0.8f;
            }
            
            earth.colors[topLeft * 3] = r;
            earth.colors[topLeft * 3 + 1] = g;
            earth.colors[topLeft * 3 + 2] = b;
            
            earth.colors[topRight * 3] = r;
            earth.colors[topRight * 3 + 1] = g;
            earth.colors[topRight * 3 + 2] = b;
            
            earth.colors[bottomLeft * 3] = r;
            earth.colors[bottomLeft * 3 + 1] = g;
            earth.colors[bottomLeft * 3 + 2] = b;
            
            earth.colors[bottomRight * 3] = r;
            earth.colors[bottomRight * 3 + 1] = g;
            earth.colors[bottomRight * 3 + 2] = b;
        }
    }
    earth.n_vertices = earth.vertices;
    earth.MASS = EARTH_MASS;
    return earth;
}
/////
void update_cord(Planet& earth){
    float result = W * (glfwGetTime());
    for(int i = 0; i < earth.vertices.size(); i = i+3) {
        float a = earth.n_vertices[i];
        float b = earth.n_vertices[i+1];
        
        earth.vertices[i] = a*cos(result) - b*sin(result); 
        earth.vertices[i+1]  = a*sin(result) + b*cos(result);
    }
}
/////////////

struct Axes {
    std::vector<float> vertices;
    std::vector<float> colors;
};

Axes genAxes(float length = 2.0f) {
    Axes axes;

    axes.vertices.push_back(0.0f);
    axes.vertices.push_back(0.0f);
    axes.vertices.push_back(0.0f);
    
    axes.vertices.push_back(length);
    axes.vertices.push_back(0.0f);
    axes.vertices.push_back(0.0f);
    
    axes.vertices.push_back(0.0f);
    axes.vertices.push_back(0.0f);
    axes.vertices.push_back(0.0f);
    
    axes.vertices.push_back(0.0f);
    axes.vertices.push_back(length);
    axes.vertices.push_back(0.0f);
    
    axes.vertices.push_back(0.0f);
    axes.vertices.push_back(0.0f);
    axes.vertices.push_back(0.0f);
    
    axes.vertices.push_back(0.0f);
    axes.vertices.push_back(0.0f);
    axes.vertices.push_back(length);
    
    axes.colors.push_back(1.0f);
    axes.colors.push_back(0.0f);
    axes.colors.push_back(0.0f);
    
    axes.colors.push_back(1.0f);
    axes.colors.push_back(0.0f);
    axes.colors.push_back(0.0f);
    
    axes.colors.push_back(0.0f);
    axes.colors.push_back(1.0f);
    axes.colors.push_back(0.0f);
    
    axes.colors.push_back(0.0f);
    axes.colors.push_back(1.0f);
    axes.colors.push_back(0.0f);
    
    axes.colors.push_back(0.0f);
    axes.colors.push_back(0.0f);
    axes.colors.push_back(1.0f);
    
    axes.colors.push_back(0.0f);
    axes.colors.push_back(0.0f);
    axes.colors.push_back(1.0f);
    
    return axes;
}
/////////балуемся с физикой

 

/////////балуемся с физикой

////
int main()
{
    std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "3D Planet", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    glewExperimental = GL_TRUE;
    glewInit();
    glEnable(GL_DEPTH_TEST);
    
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);  
    glViewport(0, 0, width, height);
    
    initCamera();
    glfwSetKeyCallback(window, key_callback);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // ПОЛУЧАЕМ LOCATION UNIFORM ПЕРЕМЕННЫХ (ИЗМЕНИЛ)
    GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");

    Planet earth = genPlanet(6371000.0f, 32, 32);
    Axes axes = genAxes(1.5f);
    
    ///для планеты
    GLuint VBO, VAO, EBO;
    GLuint VBO_colors;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);  
    glGenBuffers(1, &VBO_colors);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, earth.vertices.size() * sizeof(float), earth.vertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_colors);
    glBufferData(GL_ARRAY_BUFFER, earth.colors.size() * sizeof(float), earth.colors.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, earth.indices.size() * sizeof(unsigned int), earth.indices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    sat_x = orbitRadius * sin(orbitAngle) * cos(SPUTNIK_RADIAN);
    sat_y = orbitRadius * cos(orbitAngle);
    sat_z = orbitRadius * sin(orbitAngle) * sin(SPUTNIK_RADIAN);
    ///для спутника (ИЗМЕНИЛ позицию)
    sputnik satellite = genSputnik(0.09f, sat_x*MASHTAB, sat_y*MASHTAB, sat_z*MASHTAB);

    GLuint sputnikVAO, sputnikVBO, sputnikEBO, sputnikColorVBO;
    glGenVertexArrays(1, &sputnikVAO);
    glGenBuffers(1, &sputnikVBO);
    glGenBuffers(1, &sputnikColorVBO);
    glGenBuffers(1, &sputnikEBO);

    glBindVertexArray(sputnikVAO);

    glBindBuffer(GL_ARRAY_BUFFER, sputnikVBO);
    glBufferData(GL_ARRAY_BUFFER, satellite.vertices.size() * sizeof(float), satellite.vertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, sputnikColorVBO);
    glBufferData(GL_ARRAY_BUFFER, satellite.colors.size() * sizeof(float), satellite.colors.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sputnikEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, satellite.indices.size() * sizeof(unsigned int), satellite.indices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    ////для осей
    GLuint axesVAO, axesVBO, axesColorVBO;
    glGenVertexArrays(1, &axesVAO);
    glGenBuffers(1, &axesVBO);
    glGenBuffers(1, &axesColorVBO);
    
    glBindVertexArray(axesVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, axesVBO);
    glBufferData(GL_ARRAY_BUFFER, axes.vertices.size() * sizeof(float), axes.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, axesColorVBO);
    glBufferData(GL_ARRAY_BUFFER, axes.colors.size() * sizeof(float), axes.colors.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    glLineWidth(2.0f);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        float aspect = (float)width / (float)height;
        
        static float lastTime = glfwGetTime();
        float currentTime = glfwGetTime();
        float dt = currentTime - lastTime;
        lastTime = currentTime;
        processCameraMovement(dt); 
        dt *= 1000.0f;
        

        update_cord(earth);
        update_satellite_position(dt);

        // Обновляем центр спутника
        satellite.center[0] = sat_x * MASHTAB;
        satellite.center[1] = sat_y * MASHTAB;
        satellite.center[2] = sat_z * MASHTAB;
        
        update_sputnik_vertices(satellite, satellite.center[0], satellite.center[1], satellite.center[2], 0.09f);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, earth.vertices.size() * sizeof(float), earth.vertices.data());
        
        glBindBuffer(GL_ARRAY_BUFFER, sputnikVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0,  satellite.vertices.size() * sizeof(float), satellite.vertices.data());

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(shaderProgram);
        
        // матрица камеры
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
        
       glm::mat4 view = glm::lookAt(
        camera.position,
        camera.position + camera.front,
        camera.up
    );

        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        
        // Отрисовка
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, earth.indices.size(), GL_UNSIGNED_INT, 0);
        
        glBindVertexArray(axesVAO);
        glDrawArrays(GL_LINES, 0, 6);
        
        glBindVertexArray(sputnikVAO);
        glDrawElements(GL_TRIANGLES, satellite.indices.size(), GL_UNSIGNED_INT, 0);
        
        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }
    
    //очищаем планету
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &VBO_colors);
    glDeleteBuffers(1, &EBO);

    //очищаем ось
    glDeleteVertexArrays(1, &axesVAO);
    glDeleteBuffers(1, &axesVBO);
    glDeleteBuffers(1, &axesColorVBO);
    
    //очищаем спутник
    glDeleteVertexArrays(1, &sputnikVAO);
    glDeleteBuffers(1, &sputnikVBO);
    glDeleteBuffers(1, &sputnikColorVBO);
    glDeleteBuffers(1, &sputnikEBO);
    
    glfwTerminate();
    return 0;
}

