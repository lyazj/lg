#include <glm/gtc/matrix_transform.hpp>

#include "GL3DApplication.h"
#include "GLBezierSurface.h"
#include "GLImage.h"
#include "GLProgram.h"
#include "GLTexture.h"
#include "GLTextureDecorator.h"
#include "Utils.h"

GL_DECLARE_CLASS(GLTextureDecorator)

using namespace std;

class GLExampleApplication final : public GL3DApplication {
public:
  using GL3DApplication::GL3DApplication;

  void Init() override;
  void Display() override;

private:
  GLRenderablePtr renderable;
  GLTextureDecoratorPtr flag;
  vector<vector<vec3d>> positions;
  vector<vector<vec3d>> velocities;

  inline static constexpr GLint stacks = 3, slices = 3;
  inline static constexpr GLint division = 6;
  inline static constexpr GLdouble length = 2.88, height = 1.92;         // m
  inline static constexpr GLdouble area = length * height;               // m^2
  inline static constexpr GLdouble unitArea = area / (stacks * slices);  // m^2
  inline static constexpr GLdouble width = 0.8e-3;                       // m
  inline static constexpr GLdouble rho = 0.5e2;                          // kg/m^3
  inline static constexpr GLdouble E = 1.0e6;                            // Pa
  inline static constexpr GLdouble g = 9.8;                              // m/s^2
  inline static constexpr GLdouble m = rho * width * unitArea;           // kg
  inline static constexpr GLdouble windSpeed3 = 4.4;                     // m/s
  inline static constexpr GLdouble windSpeed6 = 12.3;                    // m/s
  inline static constexpr GLdouble windSpeed9 = 22.6;                    // m/s
  inline static constexpr GLdouble rhoWind = 1.2;                        // kg/m^3
  inline static constexpr GLdouble velocityDamping = 0.96;
  inline static constexpr GLint nStepPerFrame = 100;
  inline static GLdouble windSpeed = windSpeed9;
  inline static glm::vec<3, GLdouble> windDirection = normalize(glm::vec<3, GLdouble>(1.0, 0.0, -0.01));

  void Frame(uint64_t t, uint64_t dt) override;

  static vec3d GetNormal(const vector<vector<vec3d>> &positions, GLint istack, GLint islice);
  static vec3d GetWindForce(vec3d v, vec3d vw, vec3d n);  // positive direction: -n
  static GLdouble GetL(bool isHorizontal);
  static GLdouble GetK(bool isHorizontal);
};

int main(int argc, char *argv[])
{
  static GLApplicationPtr application;
  application = make_shared<GLExampleApplication>(argc, argv);
  application->Run();
  return 0;
}

void GLExampleApplication::Init()
{
  GL3DApplication::Init();
  glDisable(GL_CULL_FACE);  // E.g., the back face of the bottom should be displayed.
  UseProgram(GLProgram::GetLightingTextureProgram());

  GLImage flagImage;
  flagImage.Load(GetTexturePath() / "flag.png");
  auto texture = make_shared<GLTexture>();
  texture->Texture(flagImage);
  flag = make_shared<GLTextureDecorator>(nullptr, nullptr, texture);
  renderable = flag;

  positions.assign(stacks + 1, vector<vec3d>(slices + 1));
  velocities.assign(stacks + 1, vector<vec3d>(slices + 1));
  for(GLint i = 0; i <= stacks; ++i) {
    for(GLint j = 0; j <= slices; ++j) {
      positions[i][j] = {
        length * (1.0 * (GLdouble)j / slices - 0.5),
        height * (0.5 - 1.0 * (GLdouble)i / stacks),
        0.0,
      };
    }
  }

  SetModel(glm::scale(mat4(1.0f), vec3(0.15f, 0.15f, 0.15f)));
}

void GLExampleApplication::Display()
{
  Clear();
  renderable->Draw(GetModel());
  Flush();
}

void GLExampleApplication::Frame(uint64_t t [[maybe_unused]], uint64_t dt_in)
{
  GLdouble dt = (GLdouble)dt_in * 1e-9 / nStepPerFrame;  // s

  for(GLint s = 0; s < nStepPerFrame; ++s) {
    auto oldPositions(positions), newPositions(positions);
    auto oldVelocities(velocities), newVelocities(velocities);
    for(GLint i = 0; i < 2; ++i) {
      for(GLint istack = 0; istack <= stacks; ++istack) {
        for(GLint islice = 1; islice <= slices; ++islice) {  // The left-most column is fixed.
          vec3d v = velocities[istack][islice];
          vec3d vw = windSpeed * windDirection;
          vec3d n = GetNormal(positions, istack, islice);
          vec3d F = GetWindForce(v, vw, n);
          GLint dstack[4] = { -1, 1, 0, 0 }, dslice[4] = { 0, 0, -1, 1 };
          for(GLint id = 0; id < 4; ++id) {
            GLint jstack = istack + dstack[id], jslice = islice + dslice[id];
            if(jstack < 0 || jstack > stacks || jslice < 0 || jslice > slices) continue;
            GLdouble l = GetL(id >= 2), k = GetK(id >= 2);
            vec3d d = positions[jstack][jslice] - positions[istack][islice];
            F += k * (glm::length(d) - l) * normalize(d);
          }
          vec3d a = F / m + vec3d(0.0, -g, 0.0);
          newPositions[istack][islice] = oldPositions[istack][islice] + v * dt;
          newVelocities[istack][islice] = oldVelocities[istack][islice] + a * dt;
        }
      }
      if(i == 1) break;
      for(GLint istack = 0; istack <= stacks; ++istack) {    // Euler method.
        for(GLint islice = 1; islice <= slices; ++islice) {  // The left-most column is fixed.
          positions[istack][islice] = (oldPositions[istack][islice] + newPositions[istack][islice]) * 0.5;
          velocities[istack][islice] = (oldVelocities[istack][islice] + newVelocities[istack][islice]) * 0.5;
        }
      }
    }
    swap(positions, newPositions);
    swap(velocities, newVelocities);
  }

  static_assert(stacks == 3 && slices == 3);
  vector<vec3> controls(16);
  for(GLint istack = 0; istack <= stacks; ++istack) {
    for(GLint islice = 0; islice <= slices; ++islice) {
      controls[istack * (slices + 1) + islice] = positions[istack][islice];
      velocities[istack][islice] *= velocityDamping;
    }
  }
  flag->SetRenderable(make_shared<GLBezierSurface>(controls.data(), division));
  flag->SetVertexAttributes(true);
  flag->Buffer(true);
}

vec3d GLExampleApplication::GetWindForce(vec3d v, vec3d vw, vec3d n)
{
  vec3d vr = v - vw, vn = dot(vr, n) * n;
  return -0.5 * rhoWind * unitArea * glm::length(vn) * vn;
}

GLdouble GLExampleApplication::GetL(bool isHorizontal)
{
  GLdouble l;
  if(!isHorizontal) {
    l = height / stacks;
  } else {
    l = length / slices;
  }
  return l;
}

GLdouble GLExampleApplication::GetK(bool isHorizontal)
{
  GLdouble l, lp;
  if(!isHorizontal) {
    l = height / stacks;
    lp = length / slices;
  } else {
    l = length / slices;
    lp = height / stacks;
  }
  return E * lp * width / l;
}

vec3d GLExampleApplication::GetNormal(const vector<vector<vec3d>> &positions, GLint istack, GLint islice)
{
  vec3d tstack, tslice;
  if(istack == 0) {
    tstack = (positions[istack + 1][islice] - positions[istack][islice]) * 2.0;
  } else if(istack == stacks) {
    tstack = (positions[istack][islice] - positions[istack - 1][islice]) * 2.0;
  } else {
    tstack = positions[istack + 1][islice] - positions[istack - 1][islice];
  }
  if(islice == 0) {
    tslice = (positions[istack][islice + 1] - positions[istack][islice]) * 2.0;
  } else if(islice == slices) {
    tslice = (positions[istack][islice] - positions[istack][islice - 1]) * 2.0;
  } else {
    tslice = positions[istack][islice + 1] - positions[istack][islice - 1];
  }
  return normalize(cross(tstack, tslice));
}
