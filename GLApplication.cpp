#include "GLApplication.h"

#include "GLImage.h"
#include "GLProgram.h"
#include "Utils.h"

GL_DEFINE_WRAPPER(ReadPixels)

using namespace std;

GLApplication *GLApplication::gInstance;

GLApplication::GLApplication(int &ac, char *av[])
    : argc(ac),
      argv(av),
      programName(argv[0]),
      programShortName(fs::path(programName).stem().string()),
      displayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH),  // negligible overhead
#ifdef _WIN32
      width(512),
      height(512),
#else /* _WIN32 */
      width(1024),
      height(1024),
#endif /* _WIN32 */
      title(programShortName),
      windowId(-1),
      projection(1.0),
      view(1.0),
      model(1.0),
      flush(glFlush),
      clearMask(GL_COLOR_BUFFER_BIT)
{
  if(gInstance) abort();
  gInstance = this;
}

GLApplication::~GLApplication() { gInstance = nullptr; }

void GLApplication::Run()
{
  PreInit();
  InitGL();
  Init();
  Loop();
}

void GLApplication::SetDisplayMode(unsigned m)
{
  if(windowId >= 0) abort();
  displayMode = m;
}

void GLApplication::SetWindowSize(int w, int h)
{
  if(windowId >= 0) glutReshapeWindow(w, h);
  width = w;
  height = h;
}

void GLApplication::SetTitle(const std::string &t)
{
  if(windowId >= 0) glutSetWindowTitle(t.c_str());
  title = t;
}

void GLApplication::EnableDoubleBuffer()
{
  if(windowId >= 0) abort();
  displayMode &= ~GLUT_SINGLE;
  displayMode |= GLUT_DOUBLE;
  flush = glutSwapBuffers;
}

void GLApplication::DisableDoubleBuffer()
{
  if(windowId >= 0) abort();
  displayMode &= ~GLUT_DOUBLE;
  displayMode |= GLUT_SINGLE;
  flush = glFlush;
}

void GLApplication::UseProgram(const GLProgramPtr &p)
{
  if(p) {
    p->Use();
    p->SetUniform("u_projection", projection);
    p->SetUniform("u_view", view);
    p->SetUniform("u_model", model);
  }
  program = p;
}

void GLApplication::SetProjection(const mat4 &p)
{
  if(program) program->SetUniform("u_projection", p);
  projection = p;
}

void GLApplication::SetView(const mat4 &v)
{
  if(program) program->SetUniform("u_view", v);
  view = v;
}

void GLApplication::SetModel(const mat4 &m)
{
  if(program) program->SetUniform("u_model", m);
  model = m;
}

void GLApplication::EnableBlend()
{
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GLApplication::DisableBlend() { glDisable(GL_BLEND); }

void GLApplication::EnableDepthTest()
{
  glEnable(GL_DEPTH_TEST);
  clearMask |= GL_DEPTH_BUFFER_BIT;
}

void GLApplication::DisableDepthTest()
{
  glDisable(GL_DEPTH_TEST);
  clearMask &= ~GL_DEPTH_BUFFER_BIT;
}

void GLApplication::Clear() const { glClear(clearMask); }

void GLApplication::SaveScreen(const fs::path &path, GLenum mode) const
{
  vector<byte> pixels(width * height * 4);
  glFinish();
  glReadBuffer(mode);
  GLReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

  GLImage image;
  image.SetWidth(width);
  image.SetHeight(height);
  image.SetData(std::move(pixels));
  image.SetType(GLImageType::RGBA);
  image.Save(path);
}

void GLApplication::PreInit() { RandSeed((unsigned long long)time(0)); }

void GLApplication::InitGL()
{
  glutInit(&argc, argv);
  glutInitDisplayMode(displayMode);
  glutInitWindowSize(width, height);
  windowId = glutCreateWindow(title.c_str());
  if(windowId < 0) abort();
  if(glewInit() != GLEW_OK) abort();
}

void GLApplication::Init()
{
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glPointSize(1.0);
#ifdef _WIN32
  glLineWidth(1.0);
#else /* _WIN32 */
  glLineWidth(2.0);
#endif /* _WIN32 */
  GLProgram::SetDefaultVertexAttributes();

  glutDisplayFunc([] { GLApplication::GetInstance()->Display(); });
  glutReshapeFunc([](int w, int h) { GLApplication::GetInstance()->Reshape(w, h); });
}

void GLApplication::Reshape(int w, int h)
{
  glViewport(0, 0, w, h);
  width = w;
  height = h;
}
