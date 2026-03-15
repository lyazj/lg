#include "GLApplication.h"

#include <GL/glut.h>

#ifdef FREEGLUT
#include <GL/freeglut_ext.h>
#endif /* FREEGLUT */

#include <iostream>
#include <unordered_map>

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
#else  /* _WIN32 */
      width(1024),
      height(1024),
#endif /* _WIN32 */
      title(programShortName),
      windowId(-1),
      projection(1.0f),
      view(1.0f),
      model(1.0f),
      flush(glFlush),
      clearMask(GL_COLOR_BUFFER_BIT),
      frameRate(60),
      showFrameRateInterval(1000),
      frameCount(0),
      frameTime(0)
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
    p->SetUniform("u_texture0", 0);  // [XXX] This is duplicated.
    p->SetUniform("u_texture1", 1);  // [XXX] This is duplicated.
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

void GLApplication::PostRedisplay() const { glutPostRedisplay(); }

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

void GLApplication::SaveScreen() const
{
  fs::path path = programShortName + "_" + to_string(GetSystemTime()) + ".png";
  SaveScreen(path, GL_FRONT);
  clog << "Info: screen saved to: " << path << endl;
}

void GLApplication::PreInit()
{
  RandSeed((unsigned long long)time(0));
  GLImage::Init();
}

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
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glPointSize(1.0f);
#ifdef _WIN32
  glLineWidth(1.0f);
#else  /* _WIN32 */
  glLineWidth(2.0f);
#endif /* _WIN32 */
  GLProgram::SetDefaultVertexAttributes();

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  static unordered_map<int, Mouse> buttonMap = {
    { GLUT_LEFT_BUTTON, Mouse::LeftButton },
    { GLUT_MIDDLE_BUTTON, Mouse::MiddleButton },
    { GLUT_RIGHT_BUTTON, Mouse::RightButton },
    { 3, Mouse::WheelUp },
    { 4, Mouse::WheelDown },
    { 5, Mouse::WheelLeft },
    { 6, Mouse::WheelRight },
    { 7, Mouse::Backward },
    { 8, Mouse::Forward },
  };
  static unordered_map<int, SpecialKey> specialKeyMap = {
    { GLUT_KEY_F1, SpecialKey::F1 },
    { GLUT_KEY_F2, SpecialKey::F2 },
    { GLUT_KEY_F3, SpecialKey::F3 },
    { GLUT_KEY_F4, SpecialKey::F4 },
    { GLUT_KEY_F5, SpecialKey::F5 },
    { GLUT_KEY_F6, SpecialKey::F6 },
    { GLUT_KEY_F7, SpecialKey::F7 },
    { GLUT_KEY_F8, SpecialKey::F8 },
    { GLUT_KEY_F9, SpecialKey::F9 },
    { GLUT_KEY_F10, SpecialKey::F10 },
    { GLUT_KEY_F11, SpecialKey::F11 },
    { GLUT_KEY_F12, SpecialKey::F12 },
    { GLUT_KEY_LEFT, SpecialKey::Left },
    { GLUT_KEY_RIGHT, SpecialKey::Right },
    { GLUT_KEY_UP, SpecialKey::Up },
    { GLUT_KEY_DOWN, SpecialKey::Down },
    { GLUT_KEY_PAGE_UP, SpecialKey::PageUp },
    { GLUT_KEY_PAGE_DOWN, SpecialKey::PageDown },
    { GLUT_KEY_HOME, SpecialKey::Home },
    { GLUT_KEY_END, SpecialKey::End },
    { GLUT_KEY_INSERT, SpecialKey::Insert },
#ifdef GLUT_KEY_SHIFT_L
    { GLUT_KEY_SHIFT_L, SpecialKey::ShiftLeft },
#endif /* GLUT_KEY_SHIFT_L */
#ifdef GLUT_KEY_SHIFT_R
    { GLUT_KEY_SHIFT_R, SpecialKey::ShiftRight },
#endif /* GLUT_KEY_SHIFT_R */
#ifdef GLUT_KEY_CTRL_L
    { GLUT_KEY_CTRL_L, SpecialKey::CtrlLeft },
#endif /* GLUT_KEY_CTRL_L */
#ifdef GLUT_KEY_CTRL_R
    { GLUT_KEY_CTRL_R, SpecialKey::CtrlRight },
#endif /* GLUT_KEY_CTRL_R */
#ifdef GLUT_KEY_ALT_L
    { GLUT_KEY_ALT_L, SpecialKey::AltLeft },
#endif /* GLUT_KEY_ALT_L */
#ifdef GLUT_KEY_ALT_R
    { GLUT_KEY_ALT_R, SpecialKey::AltRight },
#endif /* GLUT_KEY_ALT_R */
#ifdef GLUT_KEY_SUPER_L
    { GLUT_KEY_SUPER_L, SpecialKey::SuperLeft },
#endif /* GLUT_KEY_SUPER_L */
#ifdef GLUT_KEY_SUPER_R
    { GLUT_KEY_SUPER_R, SpecialKey::SuperRight },
#endif /* GLUT_KEY_SUPER_R */
  };

  glutDisplayFunc([] { GLApplication::GetInstance()->Display(); });

  glutReshapeFunc([](int w, int h) { GLApplication::GetInstance()->Reshape(w, h); });

  glutTimerFunc(0, [](int v) { GLApplication::GetInstance()->FrameTimer(v); }, (int)NsToMs(frameTime));

  glutMouseFunc([](int b, int s, int x, int y) {
    auto it = buttonMap.find(b);
    if(it == buttonMap.end()) {
      cerr << "Warning: Unrecognized mouse button: " << b << endl;
      return;
    }
    switch(s) {
    case GLUT_DOWN: return GLApplication::GetInstance()->MouseDown(it->second, x, y);
    case GLUT_UP: return GLApplication::GetInstance()->MouseUp(it->second, x, y);
    default: abort();
    }
  });

  glutKeyboardFunc([](unsigned char k, int x, int y) { GLApplication::GetInstance()->KeyDown(k, x, y); });

  glutKeyboardUpFunc([](unsigned char k, int x, int y) { GLApplication::GetInstance()->KeyUp(k, x, y); });

  glutSpecialFunc([](int k, int x, int y) {
    auto it = specialKeyMap.find(k);
    if(it == specialKeyMap.end()) {
      cerr << "Warning: Unrecognized special key: " << k << endl;
      return;
    }
    GLApplication::GetInstance()->SpecialKeyDown(it->second, x, y);
  });

  glutSpecialUpFunc([](int k, int x, int y) {
    auto it = specialKeyMap.find(k);
    if(it == specialKeyMap.end()) {
      cerr << "Warning: Unrecognized special key: " << k << endl;
      return;
    }
    GLApplication::GetInstance()->SpecialKeyUp(it->second, x, y);
  });

  if(showFrameRateInterval) {
    glutTimerFunc(showFrameRateInterval, [](int) { GLApplication::GetInstance()->ShowFrameRate(); }, 0);
  }
}

void GLApplication::Display() { }

void GLApplication::Reshape(int w, int h)
{
  glViewport(0, 0, w, h);
  width = w;
  height = h;
}

void GLApplication::FrameTimer(unsigned mt)  // mt: expected time in ms
{
  uint64_t t = GetElapsedTime();

  // Calculate the expected time span since the last frame, modulo 2^31 ms.
  int mdt = mt - (unsigned)NsToMs(frameTime);

  // Discard obsolete timer events to make sure the system responds.
  if(mdt >= 0) {
    //clog << "Info: GLApplication::FrameTimer(): frame: t = " << t << " ns, dt = " << mdt << " ns" << endl;
    Frame(t, t - frameTime);
    PostRedisplay();
  }

  // Calculate the delay since the expected time, modulo 2^31 ms.
  int mdelay = (unsigned)NsToMs(t) - mt;

  // Calculate the remaining time until the next expected frame, modulo 2^31 ms.
  int mremain = int(1000.0f / (GLfloat)frameRate - (float)mdelay + 0.5f);
  if(mremain < 0) mremain = 0;

  // Calcluate the expected time for the next frame, modulo 2^32 ms.
  mt = (unsigned)NsToMs(t) + mremain;

  // Schedule the next timer event.
  glutTimerFunc(mremain, [](int v) { GLApplication::GetInstance()->FrameTimer((unsigned)v); }, mt);
  ++frameCount;
  frameTime = t;
}

void GLApplication::Frame(uint64_t t [[maybe_unused]], uint64_t dt [[maybe_unused]]) { }

void GLApplication::MouseDown(Mouse button [[maybe_unused]], int x [[maybe_unused]], int y [[maybe_unused]]) { }

void GLApplication::MouseUp(Mouse button [[maybe_unused]], int x [[maybe_unused]], int y [[maybe_unused]]) { }

void GLApplication::KeyDown(unsigned char key, int x [[maybe_unused]], int y [[maybe_unused]])
{
  if(key == 022 /* Ctrl-R */) PostRedisplay();
  if(key == 023 /* Ctrl-S */) SaveScreen();
  if(key == 027 /* Ctrl-W */) exit(EXIT_SUCCESS);
}

void GLApplication::KeyUp(unsigned char key [[maybe_unused]], int x [[maybe_unused]], int y [[maybe_unused]]) { }

void GLApplication::SpecialKeyDown(SpecialKey key [[maybe_unused]], int x [[maybe_unused]], int y [[maybe_unused]]) { }

void GLApplication::SpecialKeyUp(SpecialKey key [[maybe_unused]], int x [[maybe_unused]], int y [[maybe_unused]]) { }

void GLApplication::Loop() { glutMainLoop(); }

void GLApplication::GetKeyModifiers(bool &shift, bool &ctrl, bool &alt, bool &super) const
{
  int modifiers = glutGetModifiers();
  shift = modifiers & GLUT_ACTIVE_SHIFT;
  ctrl = modifiers & GLUT_ACTIVE_CTRL;
  alt = modifiers & GLUT_ACTIVE_ALT;
#ifdef GLUT_ACTIVE_SUPER
  super = modifiers & GLUT_ACTIVE_SUPER;
#else  /* GLUT_ACTIVE_SUPER */
  super = false;
#endif /* GLUT_ACTIVE_SUPER */
}

void GLApplication::ShowFrameRate() const
{
  clog << "Info: frame rate: " << GetRealFrameRate() << " fps" << endl;
  if(showFrameRateInterval) {
    glutTimerFunc(showFrameRateInterval, [](int) { GLApplication::GetInstance()->ShowFrameRate(); }, 0);
  }
}
