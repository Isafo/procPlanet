#include <GL/glew.h>
#include <GLFW/glfw3.h>


#ifdef __WIN32__

#include <GL/glext.h>
/* Global function pointers for everything we need beyond OpenGL 1.1.
* This is a requirement from Microsoft Windows. Other platforms have
* proper built-in support for modern OpenGL, and don't need this.
*/
extern PFNGLCREATEPROGRAMPROC            glCreateProgram;
extern PFNGLDELETEPROGRAMPROC            glDeleteProgram;
extern PFNGLUSEPROGRAMPROC               glUseProgram;
extern PFNGLCREATESHADERPROC             glCreateShader;
extern PFNGLDELETESHADERPROC             glDeleteShader;
extern PFNGLSHADERSOURCEPROC             glShaderSource;
extern PFNGLCOMPILESHADERPROC            glCompileShader;
extern PFNGLGETSHADERIVPROC              glGetShaderiv;
extern PFNGLGETPROGRAMIVPROC             glGetProgramiv;
extern PFNGLATTACHSHADERPROC             glAttachShader;
extern PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog;
extern PFNGLGETPROGRAMINFOLOGPROC        glGetProgramInfoLog;
extern PFNGLLINKPROGRAMPROC              glLinkProgram;
extern PFNGLGETUNIFORMLOCATIONPROC       glGetUniformLocation;
extern PFNGLUNIFORM1FPROC                glUniform1f;
extern PFNGLUNIFORM1FVPROC               glUniform1fv;
extern PFNGLUNIFORM1IPROC                glUniform1i;
extern PFNGLUNIFORMMATRIX4FVPROC         glUniformMatrix4fv;
extern PFNGLGENBUFFERSPROC               glGenBuffers;
extern PFNGLISBUFFERPROC                 glIsBuffer;
extern PFNGLBINDBUFFERPROC               glBindBuffer;
extern PFNGLBUFFERDATAPROC               glBufferData;
extern PFNGLDELETEBUFFERSPROC            glDeleteBuffers;
extern PFNGLGENVERTEXARRAYSPROC          glGenVertexArrays;
extern PFNGLISVERTEXARRAYPROC            glIsVertexArray;
extern PFNGLBINDVERTEXARRAYPROC          glBindVertexArray;
extern PFNGLDELETEVERTEXARRAYSPROC       glDeleteVertexArrays;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
extern PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
extern PFNGLGENERATEMIPMAPPROC           glGenerateMipmap;

#endif


/* A struct to hold geometry data and send it off for rendering */
class TriangleSoup {

private:

    // All data members are private. They are accessed only by methods in the class.
    GLuint vao;          // Vertex array object, the main handle for geometry
    int nverts; // Number of vertices in the vertex array
    int ntris;  // Number of triangles in the index array (may be zero)
    GLuint vertexbuffer; // Buffer ID to bind to GL_ARRAY_BUFFER
    GLuint indexbuffer;  // Buffer ID to bind to GL_ELEMENT_ARRAY_BUFFER
    GLfloat *vertexarray; // Vertex array on interleaved format: x y z nx ny nz s t
    GLuint *indexarray;   // Element index array

public:

/* Constructor: initialize a triangleSoup object to all zeros */
TriangleSoup();

/* Destructor: clean up allocated data in a triangleSoup object */
~TriangleSoup();

/* Clean up allocated data in a triangleSoup object */
void clean();

/* Create a simple box geometry */
void createBox(float xsize, float ysize, float zsize);

/* Create a sphere (approximated by polygon segments) */
void createSphere(float radius, int segments);

/* Load geometry from an OBJ file */
void readOBJ(const char* filename);

/* Print data from a triangleSoup object, for debugging purposes */
void print();

/* Print information about a triangleSoup object (stats and extents) */
void printInfo();

/* Render the geometry in a triangleSoup object */
void render();

private:

void printError(const char *errtype, const char *errmsg);

};
