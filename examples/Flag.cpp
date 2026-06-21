#include <glm/gtc/matrix_transform.hpp>

#include "GL3DApplication.h"
#include "GLProgram.h"
#include "GLQuadrilateralMesh.h"

GL_DECLARE_CLASS(GLQuadrilateralMesh)

using namespace std;

class GLExampleApplication final : public GL3DApplication {
public:
  using GL3DApplication::GL3DApplication;

  void Init() override;
  void Display() override;

private:
  GLRenderablePtr renderable;
  GLQuadrilateralMeshPtr flag;
  vector<vector<vec3>> initialPositions;
  vector<vector<vec3>> velocities;

  inline static constexpr GLfloat length = 2.88f, height = 1.92f;  // m
  inline static constexpr GLint stacks = 128, slices = 128;

  void Frame(uint64_t t, uint64_t dt) override;
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
  UseProgram(GLProgram::GetDefaultLightingProgram());

  flag = make_shared<GLQuadrilateralMesh>(stacks, slices);
  renderable = flag;
  initialPositions.assign(stacks + 1, vector<vec3>(slices + 1));
  velocities.assign(stacks + 1, vector<vec3>(slices + 1));
  for(GLint i = 0; i <= stacks; ++i) {
    for(GLint j = 0; j <= slices; ++j) {
      initialPositions[i][j] = {
        length * (1.0f * (GLfloat)j / slices - 0.5f),
        height * (0.5f - 1.0f * (GLfloat)i / stacks),
        0.0f,
      };
      (*flag)[i][j] = initialPositions[i][j];
    }
  }

  renderable->SetVertexAttributes(false);
  renderable->Buffer(false);

  SetModel(glm::scale(mat4(1.0f), vec3(0.2f, 0.2f, 0.2f)));
}

void GLExampleApplication::Display()
{
  Clear();
  renderable->Draw(GetModel());
  Flush();
}

void GLExampleApplication::Frame(uint64_t t [[maybe_unused]], uint64_t dt)
{
  // [TODO]

  flag->Buffer(true);
}
