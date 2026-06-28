#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "GL3DApplication.h"
#include "GLBufferedGeometry.h"
#include "GLProgram.h"
#include "Utils.h"

GL_DECLARE_CLASS(GLExampleParticleSystem)

using namespace std;

class GLExampleApplication final : public GL3DApplication {
public:
  using GL3DApplication::GL3DApplication;

  void Init() override;
  void Display() override;

  struct Particle {
    vec3 position;
    vec3 velocity;
    GLfloat life;
    GLfloat ttl;
    GLfloat color;

    static GLfloat GetRho(GLfloat theta);
    static GLfloat RandTheta();
    static GLfloat RandPhi();

    void Initialize(GLfloat theta, GLfloat phi);
    void Initialize() { Initialize(RandTheta(), RandPhi()); }
    void Propagate(GLfloat dt);
  };

private:
  GLRenderablePtr renderable;

  void Frame(uint64_t t, uint64_t dt) override;

  inline static constexpr GLint nParticle = 16384;
  inline static constexpr GLfloat r = 3.0f;  // cm
  inline static constexpr GLfloat v = 3.0f;  // cm/s
  inline static constexpr GLfloat lifeDeviation = 0.10f;

  vector<Particle> particles;
  GLExampleParticleSystemPtr particleSystem;
};

int main(int argc, char *argv[])
{
  static GLApplicationPtr application;
  application = make_shared<GLExampleApplication>(argc, argv);
  application->Run();
  return 0;
}

namespace {

class DropCurve {
public:
  DropCurve();
  GLdouble GetRho(GLdouble theta) const;

  inline static constexpr GLint k = 10;
  inline static constexpr GLint nDivision = 256;

private:
  vector<vec2d> points;  // (theta, rho); theta in [-pi/2, pi/2]
};

}  // namespace

GLfloat GLExampleApplication::Particle::GetRho(GLfloat theta)
{
  static DropCurve dropCurve;
  return (GLfloat)dropCurve.GetRho(theta);
}

GLfloat GLExampleApplication::Particle::RandTheta() { return asinf(1.0f - 2.0f * RandFloat()); }

GLfloat GLExampleApplication::Particle::RandPhi() { return RandFloat() * (2.0f * pi); }

void GLExampleApplication::Particle::Initialize(GLfloat theta, GLfloat phi)
{
  vec3 target;
  GLfloat rho = r * GetRho(theta);
  target.y = rho * sinf(theta);
  target.z = rho * cosf(theta) * cosf(phi);
  target.x = rho * cosf(theta) * sinf(phi);

  position = { 0.0f, 0.0f, 0.0f };
  velocity = v * normalize(target - position);
  ttl = life = length(target - position) / v * (1.0f + (2.0f * RandFloat() - 1.0f) * lifeDeviation);
  color = rho / r;
}

void GLExampleApplication::Particle::Propagate(GLfloat dt)
{
  ttl -= dt;
  if(ttl < 0) {  // Recycle and reuse it!
    Initialize();
  } else {
    position += velocity * dt;
  }
}

class GLExampleParticleSystem : public GL3DBufferedGeometry {
public:
  using Particle = GLExampleApplication::Particle;

  GLExampleParticleSystem(const vector<Particle> &particles);
  ~GLExampleParticleSystem() override = default;

  void Update();

protected:
  const vector<Particle> &particles;

  GLBuffer colorBuffer;
  std::vector<vec4> colors;

  void IssueSetVertexAttributes(bool force) const override;
  virtual void IssueBuffer() const override;
  virtual void IssueDraw() const override;

  inline static constexpr vec4 centerColor = { 0.0f, 0.0f, 0.5f, 0.0f };
  inline static constexpr vec4 innerColor = { 0.5f, 0.3f, 0.0f, 0.2f };
  inline static constexpr vec4 outerColor = { 1.0f, 1.0f, 0.0f, 0.0f };
  inline static constexpr GLfloat innerOuterBoundary = 0.2f;

  static vec4 GetColor(const Particle &particle);
};

void GLExampleApplication::Init()
{
  GL3DApplication::Init();
#ifdef _WIN32
  glPointSize(15.0f);
#else  /* _WIN32 */
  glPointSize(30.0f);
#endif /* _WIN32 */
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  EnableBlend();
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  UseProgram(GLProgram::GetDefaultProgram());

  particles.resize(nParticle);
  for(Particle &particle : particles) particle.Initialize();
  particleSystem = make_unique<GLExampleParticleSystem>(particles);
  renderable = particleSystem;
  renderable->SetVertexAttributes(false);

  SetModel(scale(mat4(1.0f), vec3(0.1f, 0.1f, 0.1f)));
}

void GLExampleApplication::Display()
{
  Clear();
  renderable->Draw(GetModel());
  Flush();
}

void GLExampleApplication::Frame(uint64_t t [[maybe_unused]], uint64_t dt_in)
{
  GLfloat dt = (GLfloat)dt_in * 1e-9f;  // s
  for(Particle &particle : particles) particle.Propagate(dt);
  particleSystem->Update();
  renderable->Buffer(true);
}

GLExampleParticleSystem::GLExampleParticleSystem(const vector<Particle> &p) : particles(p)
{
  vertices.resize(particles.size());
  colors.resize(particles.size());
  vertexBuffer.SetUsage(GL_DYNAMIC_DRAW);
  colorBuffer.SetUsage(GL_DYNAMIC_DRAW);
}

void GLExampleParticleSystem::Update()
{
  vertices.resize(particles.size());
  colors.resize(particles.size());
  for(size_t i = 0; i < particles.size(); ++i) {
    const Particle &particle = particles[i];
    vertices[i] = particle.position;
    colors[i] = GetColor(particle);
  }
}

vec4 GLExampleParticleSystem::GetColor(const Particle &particle)
{
  GLfloat color = particle.color * (1.0f - particle.ttl / particle.life);
  if(color < innerOuterBoundary) {
    return mix(centerColor, innerColor, color / innerOuterBoundary);
  } else {
    return mix(innerColor, outerColor, (color - innerOuterBoundary) / (1.0f - innerOuterBoundary));
  }
}

void GLExampleParticleSystem::IssueSetVertexAttributes(bool force) const
{
  GL3DBufferedGeometry::IssueSetVertexAttributes(force);
  colorBuffer.Bind();
  GLProgram::SetVertexAttributePointer("a_color", 4);
}

void GLExampleParticleSystem::IssueBuffer() const
{
  GL3DBufferedGeometry::IssueBuffer();
  colorBuffer.Buffer(colors);
}

void GLExampleParticleSystem::IssueDraw() const { GLDrawArrays(GL_POINTS, 0, (GLsizei)vertices.size()); }

DropCurve::DropCurve()
{
  auto GetRhoTheta = [](GLdouble alpha) {
    GLdouble p = pow((1.0 - sin(alpha)) * 0.5, k);
    GLdouble x = p * cos(alpha), y = p * sin(alpha) + 0.9;
    return make_pair(hypot(x, y), atan2(y, x));
  };

  // Start point.
  GLdouble alpha0 = -pid / 2.0, rho0, theta0;
  tie(rho0, theta0) = GetRhoTheta(alpha0);
  points.emplace_back(theta0, rho0);

  const GLdouble dtheta = pid / nDivision;
  GLdouble dalpha = dtheta / 256.0;  // Initial conservative guess.

  // Add points.
  for(;;) {
    GLdouble alpha = alpha0 + dalpha, rho, theta;
    tie(rho, theta) = GetRhoTheta(alpha);

    // Adjust dalpha to match dtheta.
    if(theta - theta0 > dtheta) {
      for(;;) {
        GLdouble dalpha1 = dalpha * 0.5, alpha1 = alpha0 + dalpha1;
        auto [rho1, theta1] = GetRhoTheta(alpha1);
        if(theta1 - theta0 < dtheta) break;
        tie(dalpha, alpha, rho, theta) = make_tuple(dalpha1, alpha1, rho1, theta1);
      }
    } else if(theta - theta0 < dtheta) {
      for(;;) {
        GLdouble dalpha1 = dalpha * 2.0, alpha1 = alpha0 + dalpha1;
        auto [rho1, theta1] = GetRhoTheta(alpha1);
        if(theta1 - theta0 > dtheta) break;
        tie(dalpha, alpha, rho, theta) = make_tuple(dalpha1, alpha1, rho1, theta1);
      }
    }
    if(alpha > pid / 2.0) {
      alpha = pid / 2.0, dalpha = alpha - alpha0;
      tie(rho, theta) = GetRhoTheta(alpha);
    }

    points.emplace_back(theta, rho);
    if(alpha >= pid / 2.0) break;
    tie(alpha0, rho0, theta0) = make_tuple(alpha, rho, theta);
  }
}

GLdouble DropCurve::GetRho(GLdouble theta) const
{
  auto it2 = upper_bound(points.begin(), points.end(), theta, [](GLdouble t, const vec2d &p) { return t < p.x; });
  if(it2 == points.begin()) it2 = next(it2);
  if(it2 == points.end()) it2 = prev(it2);
  auto it1 = prev(it2);  // Linear interpolation.
  GLdouble theta1 = it1->x, rho1 = it1->y;
  GLdouble theta2 = it2->x, rho2 = it2->y;
  return rho1 + (theta - theta1) / (theta2 - theta1) * (rho2 - rho1);
}
