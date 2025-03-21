// PROJEKT V5.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include "Vec_template_class.cpp"

enum
{
    EXIT,
    FULL_WINDOW,
    ASPECT_1_1,
    ORTO,
    FRUST,
    PERSP
};

#define M_PI 3.14159265358979323846
#define M_PI_D (M_PI/180.0f)

// ustawienie wartoœci startowych
GLint skala = FULL_WINDOW;
GLint rzut = PERSP;
GLdouble fovy = 90;
// ustawienie parametrów zakresu rzutni
GLdouble aspect = 1;
GLfloat zakres = 10.0f;
GLfloat blisko = 2.0f;
GLfloat daleko = 500.0f;

struct Material {
    std::string name;
    vec3<double> color = {1,1,1};
    vec3<float> ambient = {1,1,1};
    vec3<float> diffuse = { 1,1,1 };
    vec3<float> specular = { 1,1,1 };

    float shininess;
    unsigned int textureID = 0; // Added texture ID

    int width =0, height = 0;
    string img_loc;
    // Constructor
    Material() : shininess(0.0), textureID(0) {}
};

struct face {
    vec3<int> vertexIndex;
    vec3<int> textrueIndex;
    vec3<int> normalIndex;
};

struct face_mat {
    unsigned id = 0;
    vector<face> faces;
};

struct mesh{
    string name;
    vec3<double> pos = {0,0,0};
    vec3<double> rot = {0,0,0};
    vec3<double> scale = {1,1,1};
    vector<vec3<double>> vertex;
    vector<vec3<double>> normals;
    vector<vec2<double>> texCoords;
    vector<face_mat> faces;

    void Execute_tranformation() const{
        //Euler to radians
        double radRotX = fmod(rot.d[0], 360) * M_PI_D;
        double radRotY = fmod(rot.d[1], 360) * M_PI_D;
        double radRotZ = fmod(rot.d[2], 360) * M_PI_D;

        //trigonometric values
        double cosX = cos(radRotX);
        double sinX = sin(radRotX);
        double cosY = cos(radRotY);
        double sinY = sin(radRotY);
        double cosZ = cos(radRotZ);
        double sinZ = sin(radRotZ);


        glMatrixMode(GL_MODELVIEW);
        //glLoadIdentity();

        GLdouble transformationMatrix[16] = {

            scale.d[0] * cosY * cosZ,
            -scale.d[0] * cosY * sinZ,
            scale.d[0] * sinY,
            0.0f,

            scale.d[1] * (sinX * sinY * cosZ + cosX * sinZ),
            -scale.d[1] * (sinX * sinY * sinZ - cosX * cosZ),
            scale.d[1] * -sinX * cosY,
            0.0f,

            scale.d[2] * (-cosX * sinY * cosZ + sinX * sinZ),
            scale.d[2] * (cosX * sinY * sinZ + sinX * cosZ),
            scale.d[2] * cosX * cosY,
            0.0f,

            pos.d[0],
            pos.d[1],
            pos.d[2],
            1.0f
        };

        glMultMatrixd(transformationMatrix);
    }
};

unsigned char* loadBMP(const std::string& filename, int& width, int& height)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to open BMP file: " << filename << std::endl;
        return nullptr;
    }


    // Read the header
    char header[54];
    file.read(header, 54);

    // Extract important header information
    width = *(int*)&header[18];
    height = *(int*)&header[22];
    int bpp = *(int*)&header[28];

    // Ensure the BMP file is of 24 bits per pixel
    if (bpp != 24)
    {
        std::cerr << "Invalid BMP format. Only 24 bits per pixel BMP files are supported." << std::endl;
        return nullptr;
    }

    // Calculate the row padding
    int rowSize = width * 3;
    int padding = (4 - (rowSize % 4)) % 4;
    rowSize += padding;

    // Read the pixel data
    std::vector<unsigned char> pixels(width * height * 3);
    for (int y = height - 1; y >= 0; --y)
    {
        for (int x = 0; x < width; ++x)
        {
            char color[3];
            file.read(color, 3);
            pixels[(y * width + x) * 3] = color[2];   // Red
            pixels[(y * width + x) * 3 + 1] = color[1]; // Green
            pixels[(y * width + x) * 3 + 2] = color[0]; // Blue
        }
        // Skip the row padding
        file.seekg(padding, std::ios::cur);
    }

    // Allocate memory for OpenGL-compatible pixel data
    unsigned char* imageData = new unsigned char[width * height * 3];
    std::memcpy(imageData, pixels.data(), width * height * 3);

    return imageData;
}

class Mat_manager {
public:
    vector<Material> materials;

    void Remove_mat(Material mat) {
        glDeleteTextures(1, &mat.textureID);
        for (unsigned i = 0; i < materials.size(); i++)
            if (mat.textureID == materials[i].textureID) materials.erase(materials.begin() + i);
    }



    void add(Material mat) {
        glEnable(GL_TEXTURE_2D);
        unsigned int mat_id;
        glGenTextures(1, &mat_id);
        glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        //if (mat.pixels == nullptr) throw "HOW DID WE GET HERE!?";
        glBindTexture(GL_TEXTURE_2D, mat_id);
        mat.textureID = mat_id;
        if (mat.img_loc != "") {
 
            unsigned char* pixel = loadBMP(mat.img_loc, mat.width, mat.height);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, mat.width, mat.height, GL_RGB, GL_UNSIGNED_BYTE, pixel);
            free(pixel);
        }
        materials.push_back(mat);
    };

    ~Mat_manager(){
            for (unsigned i = 0; i < materials.size(); i++)
                if (0 != materials[i].textureID) glDeleteTextures(1, &materials[i].textureID);
    }
};

static void resize(int width, int height)
{
    const float ar = (float)width / (float)height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    switch (rzut)
    {
    case ORTO:
        if (skala == ASPECT_1_1)
        {
            if (width < height && width > 0) glOrtho(-zakres, zakres, -zakres * height / width, zakres * height / width, -zakres, zakres);
            else if (width >= height && height > 0) glOrtho(-zakres * width / height, zakres * width / height, -zakres, zakres, -zakres, zakres);
        }
        else glOrtho(-zakres, zakres, -zakres, zakres, -zakres, zakres);
        break;
    case FRUST:
        if (skala == ASPECT_1_1)
        {
            if (width < height && width > 0) glFrustum(-zakres, zakres, -zakres * height / width, zakres * height / width, blisko, daleko);
            else if (width >= height && height > 0) glFrustum(-zakres * width / height, zakres * width / height, -zakres, zakres, blisko, daleko);
        }
        else glFrustum(-zakres, zakres, -zakres, zakres, blisko, daleko);
        break;
    default:
    {
        if (height > 0)
            aspect = width / (GLdouble)height;
        gluPerspective(fovy, aspect, blisko, daleko);
    }
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Menu(int value)
{
    switch (value)
    {
        // wyjœcie
    case EXIT:
        exit(0);
    case FULL_WINDOW:
        skala = FULL_WINDOW;
        resize(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        break;
    case ASPECT_1_1:
        skala = ASPECT_1_1;
        resize(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        break;
    case ORTO:
        rzut = ORTO;
        resize(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        break;
    case FRUST:
        rzut = FRUST;
        resize(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        break;
    case PERSP:
        rzut = PERSP;
        resize(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        break;
    }
}

class light_obj {

public:
    bool enabled;
    bool type = false;

    vec4<float> ambient;
    vec4<float> diffuse;
    vec4<float> specular;
    vec4<float> pos;

    vec4<float> direct;
    float CutOff = 20; 
    int light_id;

    void Update() {
        //disables light 
        if (enabled) glEnable(light_id);
        else glDisable(light_id);

        glLightfv(light_id, GL_AMBIENT, ambient.d);
        glLightfv(light_id, GL_DIFFUSE, diffuse.d);
        glLightfv(light_id, GL_SPECULAR, specular.d);
        glLightfv(light_id, GL_POSITION, pos.d);
        if (type) {
            glLightfv(light_id, GL_SPOT_DIRECTION, direct.d);
            glLightf(light_id, GL_SPOT_CUTOFF, CutOff);
        }
        else  glLightf(light_id, GL_SPOT_CUTOFF, 360);
    };

    light_obj(int id) {
        light_id = id;
        enabled = true;
        ambient = { 0.0f, 0.0f, 0.0f, 1.0f };
        diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
        specular = { 1.0f, 1.0f, 1.0f, 1.0f };
        pos = { 0.0f, 0.0f, 0.0f, 1.0f };
        direct = { 0.0f, 0.0f, 0.0f, 1.0f };
    }

};

struct Camera_ctr {
    double cameraPosX = 0.0f; 
    double cameraPosY = 0.0f;
    double cameraPosZ = 0.0f;
    double cameraAngleX = 0.0f;
    double cameraAngleY = 0.0f;
    double cameraSpeed = 0.1f;
    double cam_dist = 5.0f;
    int lastMouseX = 0;
    int lastMouseY = 0;
    void render_cam() {
        if (rzut == FRUST) cam_dist = 0; else cam_dist = 5;
        double cameraPosX = 5 * sin(cameraAngleY * M_PI_D) * cos(cameraAngleX * M_PI_D);
        double cameraPosZ = 5 * cos(cameraAngleY * M_PI_D) * cos(cameraAngleX * M_PI_D);
        double cameraPosY = 5 * sin(cameraAngleX * M_PI_D);
        gluLookAt(cameraPosX, cameraPosY, cameraPosZ, 0,0,0 , 0, 1, 0);
    }
};

class Scene {
public:
    Mat_manager mat_manager;
    vector<mesh> meshes;
    vector<light_obj> lights;
    Camera_ctr cam;
    void load_light(int id)
    {
        for (auto& l : lights) if (l.light_id == id) return;
        lights.push_back(light_obj(id));
    }

    void load_model(string name)
    {
        load_mtlFile("./obj/" + name + ".mtl");
        load_objFile("./obj/" + name + ".obj", name);
    }

    unsigned int find_key(string key) {
        for (const Material& name : mat_manager.materials) 
        { if (name.name == key) return name.textureID; }
        return 0;
    }

    unsigned int find_id(unsigned int key) {
        for (unsigned int i = 0; i < mat_manager.materials.size(); i++) { if (mat_manager.materials[i].textureID == key) return i; }
        return mat_manager.materials.size();
    }

    bool load_objFile(const string filename,string name) {
        mesh mesh;
        mesh.name = name;
        ifstream file(filename);
        if (!file) { cerr << "Failed to load file: " << filename << endl; return false; }

        face_mat cur_mat;
        string line;
        bool ctr = false;
        while (getline(file, line))
        {

            istringstream iss(line);
            string token;
            iss >> token;
            if (token == "usemtl")
            {
                string name;
                if (ctr) {
                    mesh.faces.push_back(cur_mat);
                    cur_mat = face_mat();
                    }
                ctr = true;
                iss >> name;
                cur_mat.id = find_key(name);

            }
            if (token == "v")
            {
                vec3<double> v;
                iss >> v;
                mesh.vertex.push_back(v);
            }
            else if
                (token == "vn")
            {
                vec3<double> vn;
                iss >> vn;
                mesh.normals.push_back(vn);
            }
            else if
                (token == "vt")
            {
                vec2<double> txc;
                iss >> txc;
                mesh.texCoords.push_back(txc);
            }
            else if
                (token == "f")
            {
                face fin;
                string faceIndex;
                char i = 0;
                while (iss >> faceIndex)
                {
                    char s;
                    istringstream faceIss(faceIndex);
                    if (faceIndex.find("//") != string::npos)
                    {
                        fin.textrueIndex.d[i] = 1;
                        faceIss >> fin.vertexIndex.d[i] >> s >> s >> fin.normalIndex.d[i];
                    } else faceIss >> fin.vertexIndex.d[i] >> s >> fin.textrueIndex.d[i] >> s >> fin.normalIndex.d[i];
                    i++;
                }
                cur_mat.faces.push_back(fin);
            }
        }
        mesh.faces.push_back(cur_mat);
        file.close();

        meshes.push_back(mesh);

        return true;
    }

    bool load_mtlFile(string mtlFilename) {
        ifstream mfile(mtlFilename);
        string line;
        Material currentMaterial;
        string currentMaterialName;
        if (!mfile) return false;
        while (std::getline(mfile, line)) {
            std::istringstream iss(line);
            std::string token;
            iss >> token;

            if (token == "newmtl") {
                if (!currentMaterialName.empty()) {
                    mat_manager.add(currentMaterial);

                    currentMaterial = Material();
                }
                iss >> currentMaterialName;
                currentMaterial.name = currentMaterialName;
            }
            else if (token == "Ka") {
                iss >> currentMaterial.ambient;
            }
            else if (token == "Kd") {
                iss >> currentMaterial.diffuse;
            }
            else if (token == "Ks") {
                iss >> currentMaterial.specular;
            }
            else if (token == "Ns") {
                iss >> currentMaterial.shininess;
            }

            else if (token == "map_Kd") {
                string texfilename;
                iss >> texfilename;
                currentMaterial.img_loc = "./tex/" + texfilename;
            }
        }
        mfile.close();
        mat_manager.add(currentMaterial);
        return true;
    }

    void updateMenu() {
        // Clear the menu
        glutDetachMenu(GLUT_RIGHT_BUTTON);

        // Create a new menu
        int Aspectmenu = glutCreateMenu(Menu);
        glutAddMenuEntry("Rodzaj skalowania - cale okno", FULL_WINDOW);
        glutAddMenuEntry("Rodzaj skalowania - skala 1:1", ASPECT_1_1);
        glutAddMenuEntry("Rzutowanie ortogonalne", ORTO);
        glutAddMenuEntry("Rzutowanie frustum", FRUST);
        glutAddMenuEntry("Rzutowanie perspective", PERSP);
        
        //glutCreateMenu()
        //glutAddSubMenu("Render menu", Aspectmenu)
        
        //glutAddMenuEntry("Wyjscie", EXIT);
        glutAttachMenu(GLUT_RIGHT_BUTTON);
    }

    void draw()
    {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        for (light_obj l : lights) l.Update();
        cam.render_cam();
        for (const mesh& mesh: meshes ){
            glPushMatrix();
            mesh.Execute_tranformation();

        for (const face_mat& mat_curr : mesh.faces) {
            glBindTexture(GL_TEXTURE_2D, mat_curr.id);
            unsigned id = find_id(mat_curr.id);
            Material& work_mat = mat_manager.materials[id];
            glMaterialfv(GL_FRONT, GL_AMBIENT, work_mat.ambient.d);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, work_mat.diffuse.d);
            glMaterialfv(GL_FRONT, GL_SPECULAR, work_mat.specular.d);
            glMaterialf(GL_FRONT, GL_SHININESS, work_mat.shininess);
            glColor3dv(work_mat.color.d);
            glBegin(GL_TRIANGLES);
            for (const face& Face : mat_curr.faces) {


                for (int i = 0; i < 3; ++i) {
                    const int vertexIndex = Face.vertexIndex.d[i];
                    const int normalIndex = Face.normalIndex.d[i];
                    const int texCoordIndex = Face.textrueIndex.d[i];

                    const vec3<double>& vert = mesh.vertex[vertexIndex - 1];
                    const vec3<double>& normal = mesh.normals[normalIndex - 1];
                    const vec2<double>& texCoord = mesh.texCoords[texCoordIndex - 1];

                    glNormal3dv(normal.d);
                    glTexCoord2dv(texCoord.d);
                    glVertex3dv(vert.d);
                }
            }
            glEnd();
        }; glPopMatrix();
    }
    }
};

/* GLUT callback Handlers */
Scene GLOB_SCENE;

static void display(void)
{
    const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    const double a = t * 90.0;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    GLOB_SCENE.draw();
    glPopMatrix();

    glutSwapBuffers();
}

int color;

static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:
    case 'q':
        exit(0);
        break;


    //movement and rotation;

    case '4':
        GLOB_SCENE.meshes[0].pos += { -0.1,0,0 };
        break;
    case '6':
        GLOB_SCENE.meshes[0].pos += { 0.1,0,0 };
        break;
    case '8':
        GLOB_SCENE.meshes[0].pos += { 0,0,-0.1 };
        break;
    case '2':
        GLOB_SCENE.meshes[0].pos += { 0,0,0.1 };
        break;
    case '5':
        GLOB_SCENE.meshes[0].pos = { 0,5,0 };
        GLOB_SCENE.meshes[0].rot = { 90,0,0 };
        break;
    case '7':
        GLOB_SCENE.meshes[0].rot += { 1,0,0 };
        break;
    case '9':
        GLOB_SCENE.meshes[0].rot += { -1, 0, 0 };
        break;
    case '1':
        GLOB_SCENE.meshes[0].rot += { 0, 0, 1 };
        break;
    case '3':
        GLOB_SCENE.meshes[0].rot += { 0, 0, -1 };
        break;

    case '.':
        GLOB_SCENE.lights[0].enabled = !GLOB_SCENE.lights[0].enabled;
        break;

    case ',':
        GLOB_SCENE.lights[1].enabled = !GLOB_SCENE.lights[1].enabled;
        break;


    case '-':
        if (GLOB_SCENE.lights[1].CutOff > 5)
            GLOB_SCENE.lights[1].CutOff--;
        break;

    case 'H':
        GLOB_SCENE.meshes[0].scale += {1,1,1};
        break;

    case '+':
        GLOB_SCENE.lights[1].CutOff++;
        break;
    case 'o':
        GLOB_SCENE.load_model("sword_dev");
        break;
    case 'a':
        GLOB_SCENE.lights[0].diffuse = { 1,0,0,1 };
        break;
    case 's':
        GLOB_SCENE.lights[0].diffuse = { 0,1,0,1 };
        break;
    case 'd':
        GLOB_SCENE.lights[0].diffuse = { 0,0,1,1 };
        break;
    case 'r':
        GLOB_SCENE.lights[0].diffuse = { 1,1,1,1 };
        GLOB_SCENE.lights[1].diffuse = { 1,1,1,1 };
        break;
    case 'z':
        GLOB_SCENE.lights[1].diffuse = { 1,0,0,1 };
        break;
    case 'x':
        GLOB_SCENE.lights[1].diffuse = { 0,1,0,1 };
        break;
    case 'c':
        GLOB_SCENE.lights[1].diffuse = { 0,0,1,1 };
        break;


    };
    glutPostRedisplay();
}

static void idle(void)
{
    glutPostRedisplay();
}

void handleMouseMotion(int x, int y) {
    Camera_ctr& cam = GLOB_SCENE.cam;
    int deltaX = x - cam.lastMouseX;
    int deltaY = y - cam.lastMouseY;

    cam.cameraAngleX += static_cast<float>(deltaY) * 0.2f;
    cam.cameraAngleY += static_cast<float>(deltaX) * 0.2f;

    cam.lastMouseX = x;
    cam.lastMouseY = y;

    glutPostRedisplay();
}
void handleMouseClick(int button, int state, int x, int y) {
    Camera_ctr& cam = GLOB_SCENE.cam;
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        cam.lastMouseX = x;
        cam.lastMouseY = y;
    }
}
/* Program entry point */

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(10, 10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("GLUT Shapes");

    

    glutReshapeFunc(resize);
    glutDisplayFunc(display);

    glutMouseFunc(handleMouseClick);
    glutMotionFunc(handleMouseMotion);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);

    glClearColor(1, 1, 1, 1);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    GLOB_SCENE.load_model("sword_dev");
    GLOB_SCENE.load_model("sceene");
    int bark = GLOB_SCENE.find_id(GLOB_SCENE.find_key("Bark"));
    int leaf = GLOB_SCENE.find_id(GLOB_SCENE.find_key("Leaf"));
    int stone = GLOB_SCENE.find_id(GLOB_SCENE.find_key("Stone_B"));
    GLOB_SCENE.mat_manager.materials[bark].color = { 102./255., 73./255., 58./255. };
    GLOB_SCENE.mat_manager.materials[leaf].color = { 173./255., 203./255., 110./255. };
    GLOB_SCENE.mat_manager.materials[stone].color = { 169./255., 164./255., 160./255.};
    GLOB_SCENE.updateMenu();
    GLOB_SCENE.meshes[1].pos = { 0,-4,0 };
    GLOB_SCENE.meshes[0].rot = { 90,0,0 };
    GLOB_SCENE.meshes[0].pos = { 0,5,0 };
    GLOB_SCENE.load_light(GL_LIGHT0);
    GLOB_SCENE.load_light(GL_LIGHT1);
    GLOB_SCENE.lights[0].pos = { 2.0f, 5.0f, 5.0f, 0.0f };
    GLOB_SCENE.lights[1].type = true;
    GLOB_SCENE.lights[1].direct = { 0,0,-10,1 }; 
    GLOB_SCENE.lights[1].pos = { 0,0,0,1 };
    GLOB_SCENE.lights[1].enabled = false;
   
    glutMainLoop();


    return EXIT_SUCCESS;
}
