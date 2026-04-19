#include "GLApplication.h"

#include <GL/glut.h>

#include <glm/gtc/matrix_transform.hpp>

#ifdef FREEGLUT
#include <GL/freeglut_ext.h>
#endif /* FREEGLUT */

#include <iostream>
#include <unordered_map>

#include "GLImage.h"
#include "GLProgram.h"
#include "Utils.h"

#ifdef _WIN32
#define USE_WINDOWS_WINDOWING
#include <windows.h>
#else /* _WIN32  */
#ifdef __APPLE__
#define USE_MAC_WINDOWING
#else /* __APPLE__ */
#define USE_X11_WINDOWING
#include <GL/glx.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#endif /* __APPLE__ */
#endif /* _WIN32 */

GL_DEFINE_WRAPPER(ReadPixels)

using namespace std;

GLApplication *GLApplication::gInstance;

GLApplication::GLApplication(int &ac, char *av[])
    : argc(ac),
      argv(av),
      programName(L"GLApplication"),
      programShortName(L"GLApplication"),
      displayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE),  // negligible overhead for GLUT_DEPTH
#ifdef _WIN32
      width(512),
      height(512),
#else  /* _WIN32 */
      width(1024),
      height(1024),
#endif /* _WIN32 */
      title(L"GL Application"),
      windowId(-1),
      projection(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f)),
      view(1.0f),
      model(1.0f),
      clearMask(GL_COLOR_BUFFER_BIT),
      frameRate(60),
      showFrameRateInterval(1000),
      frameCount(0),
      frameTime(0),
      pressedSpecialKeys(0),
      mouseX(0),
      mouseY(0),
      pressedMouseButtons(0)
{
  if(gInstance) abort();
  gInstance = this;
}

GLApplication::~GLApplication()
{
  gInstance = nullptr;
  clog << "Info: GLApplication deleted: " << programShortName << endl;
}

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
  if(w <= 0 || h <= 0) abort();
  if(windowId >= 0) glutReshapeWindow(w, h);
  width = w;
  height = h;
}

#ifndef USE_MAC_WINDOWING
void GLApplication::MaximizeWindow() const
{
#ifdef USE_WINDOWS_WINDOWING
  ShowWindow(GetActiveWindow(), SW_MAXIMIZE);
  return;
#endif /* USE_WINDOWS_WINDOWING */

#ifdef USE_X11_WINDOWING
  auto display = XOpenDisplay(nullptr);
  if(!display) return;
  XEvent event = {};
  event.xclient.type = ClientMessage;
  event.xclient.window = glXGetCurrentDrawable();
  event.xclient.message_type = XInternAtom(display, "_NET_WM_STATE", False);
  event.xclient.format = 32;
  event.xclient.data.l[0] = 1;  // _NET_WM_STATE_ADD
  event.xclient.data.l[1] = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
  event.xclient.data.l[2] = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", False);
  event.xclient.data.l[3] = 0;
  event.xclient.data.l[4] = 0;
  XSendEvent(display, DefaultRootWindow(display), False, SubstructureRedirectMask | SubstructureNotifyMask, &event);
  XFlush(display);
  XCloseDisplay(display);
  return;
#endif /* USE_X11_WINDOWING */
}
#endif /* USE_MAC_WINDOWING */

GLfloat GLApplication::GetAspect() const
{
  if(height == 0) abort();
  return (GLfloat)width / (GLfloat)height;
}

void GLApplication::SetTitle(const wstring &t)
{
  if(windowId >= 0) glutSetWindowTitle(ToLocaleString(t).c_str());
  title = t;
}

wstring GLApplication::GetDefaultTitle() const
{
  // Insert a space when: lower -> UPPER.
  wstring shortName = GetProgramShortName();
  wstring t;
  for(size_t i = 0; i < shortName.size(); ++i) {
    if(i > 0 && iswlower(shortName[i - 1]) && iswupper(shortName[i])) t += L' ';
    t += shortName[i];
  }
  return t;
}

void GLApplication::EnableDoubleBuffer()
{
  if(windowId >= 0) abort();
  displayMode &= ~GLUT_SINGLE;
  displayMode |= GLUT_DOUBLE;
}

void GLApplication::DisableDoubleBuffer()
{
  if(windowId >= 0) abort();
  displayMode &= ~GLUT_DOUBLE;
  displayMode |= GLUT_SINGLE;
}

void GLApplication::Flush() const
{
  if(displayMode & GLUT_DOUBLE) {
    glutSwapBuffers();
  } else {
    glFlush();
  }
}

void GLApplication::UseProgram(const GLProgramPtr &p)
{
  if(program == p) return;
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

vec4 GLApplication::GetViewerPosition() const
{
  vec3 v = -transpose(mat3(view)) * view[3];
  return vec4(v, projection[2][3] != 0.0f);  // 0 for orthographic and 1 for perspective
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

void GLApplication::SetFrameRate(GLint fr)
{
  std::swap(frameRate, fr);
  if(windowId >= 0 && frameRate && fr == 0) {
    glutTimerFunc(0, [](int v) { GLApplication::GetInstance()->FrameTimer(v); }, (int)NsToMs(GetElapsedTime()));
  }
}

void GLApplication::SetShowFrameRateInterval(GLint i)
{
  std::swap(showFrameRateInterval, i);
  if(windowId >= 0 && showFrameRateInterval && i == 0) {
    glutTimerFunc(showFrameRateInterval, [](int) { GLApplication::GetInstance()->ShowFrameRate(); }, 0);
  }
}

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
  fs::path path = GetProgramShortName();
  path += "_" + to_string(GetSystemTime()) + ".png";
  SaveScreen(path, GL_FRONT);
  clog << "Info: screen saved to: " << path << endl;
}

void GLApplication::PreInit()
{
  ios_base::sync_with_stdio(false);
  SetDefaultLocale();
  programName = FromLocaleString(argv[0]);
  programShortName = fs::path(programName).stem().wstring();
  title = GetDefaultTitle();
  clog << "Info: GLApplication name: " << programShortName << endl;

  RandSeed((unsigned long long)time(0));
  GLImage::Init();
}

void GLApplication::InitGL()
{
  glutInit(&argc, argv);
  glutInitDisplayMode(displayMode);
  glutInitWindowSize(width, height);
  windowId = glutCreateWindow(ToLocaleString(title).c_str());
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

  glEnable(GL_CULL_FACE);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_FRAMEBUFFER_SRGB);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  static unordered_map<int, MouseButton> buttonMap = {
    { GLUT_LEFT_BUTTON, MouseButton::LeftButton },
    { GLUT_MIDDLE_BUTTON, MouseButton::MiddleButton },
    { GLUT_RIGHT_BUTTON, MouseButton::RightButton },
    { 3, MouseButton::WheelUp },
    { 4, MouseButton::WheelDown },
    { 5, MouseButton::WheelLeft },
    { 6, MouseButton::WheelRight },
    { 7, MouseButton::Backward },
    { 8, MouseButton::Forward },
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

  glutMouseFunc([](int b, int s, int x, int y) {
    GLApplication *app = GLApplication::GetInstance();
    app->mouseX = x, app->mouseY = y;
    auto it = buttonMap.find(b);
    if(it == buttonMap.end()) {
      cerr << "Warning: unrecognized mouse button: " << b << endl;
      return;
    }
    switch(s) {
    case GLUT_DOWN: app->pressedMouseButtons |= it->second; return app->MouseDown(it->second, x, y);
    case GLUT_UP: app->pressedMouseButtons &= ~it->second; return app->MouseUp(it->second, x, y);
    default: abort();
    }
  });

  auto mouseMove = [](int x, int y) {
    GLApplication *app = GLApplication::GetInstance();
    int dx = x - app->mouseX, dy = y - app->mouseY;
    app->mouseX = x, app->mouseY = y;
    app->MouseMove(x, y, dx, dy);
  };
  glutMotionFunc(mouseMove);
  glutPassiveMotionFunc(mouseMove);

  glutKeyboardFunc([](unsigned char k, int x, int y) { GLApplication::GetInstance()->KeyDown(k, x, y); });

  glutKeyboardUpFunc([](unsigned char k, int x, int y) { GLApplication::GetInstance()->KeyUp(k, x, y); });

  glutSpecialFunc([](int k, int x, int y) {
    GLApplication *app = GLApplication::GetInstance();
    auto it = specialKeyMap.find(k);
    if(it == specialKeyMap.end()) {
      cerr << "Warning: unrecognized special key: " << k << endl;
      return;
    }
    app->pressedSpecialKeys |= it->second;
    app->SpecialKeyDown(it->second, x, y);
  });

  glutSpecialUpFunc([](int k, int x, int y) {
    GLApplication *app = GLApplication::GetInstance();
    auto it = specialKeyMap.find(k);
    if(it == specialKeyMap.end()) {
      cerr << "Warning: unrecognized special key: " << k << endl;
      return;
    }
    app->pressedSpecialKeys &= ~it->second;
    app->SpecialKeyUp(it->second, x, y);
  });

  if(frameRate) {
    glutTimerFunc(0, [](int v) { GLApplication::GetInstance()->FrameTimer(v); }, (int)NsToMs(frameTime));
  }

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

  if(frameRate) {
    // Calculate the delay since the expected time, modulo 2^31 ms.
    int mdelay = (unsigned)NsToMs(t) - mt;

    // Calculate the remaining time until the next expected frame, modulo 2^31 ms.
    int mremain = int(1000.0f / (GLfloat)frameRate - (float)mdelay + 0.5f);
    if(mremain < 0) mremain = 0;

    // Calcluate the expected time for the next frame, modulo 2^32 ms.
    mt = (unsigned)NsToMs(t) + mremain;

    // Schedule the next timer event.
    glutTimerFunc(mremain, [](int v) { GLApplication::GetInstance()->FrameTimer((unsigned)v); }, mt);
  }

  ++frameCount;
  frameTime = t;
}

void GLApplication::Frame(uint64_t t [[maybe_unused]], uint64_t dt [[maybe_unused]]) { }

void GLApplication::MouseDown(MouseButton button [[maybe_unused]], int x [[maybe_unused]], int y [[maybe_unused]]) { }

void GLApplication::MouseUp(MouseButton button [[maybe_unused]], int x [[maybe_unused]], int y [[maybe_unused]]) { }

void GLApplication::MouseMove(
    int x [[maybe_unused]], int y [[maybe_unused]], int dx [[maybe_unused]], int dy [[maybe_unused]])
{
  // empty
}

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

void GLApplication::ShowFrameRate() const
{
  clog << "Info: frame rate: " << GetRealFrameRate() << " fps" << endl;
  if(showFrameRateInterval) {
    glutTimerFunc(showFrameRateInterval, [](int) { GLApplication::GetInstance()->ShowFrameRate(); }, 0);
  }
}

GLProgramGuard::GLProgramGuard(const GLProgramPtr &p) : program(GLApplication::GetInstance()->GetProgram())
{
  GLApplication::GetInstance()->UseProgram(p);
}

GLProgramGuard::~GLProgramGuard() { GLApplication::GetInstance()->UseProgram(program); }
