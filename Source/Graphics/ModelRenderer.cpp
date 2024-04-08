#include "ModelRenderer.h"
#include "Graphics.h"
#include <algorithm>

//	 �`�惂�f���̓o�^
void ModelRenderer::Register(CompModel* model)
{
#ifdef _DEBUG
  // �����I�u�W�F�N�g�����łɓo�^����Ă��Ȃ����m�F
  std::vector<CompModel*>::iterator it = std::find(models.begin(), models.end(), model);
  assert(it == models.end());
#endif // _DEBUG

  models.emplace_back(model);
}

// �`�惂�f���̓o�^����
void ModelRenderer::Remove(CompModel* model)
{
  // �C�e���[�^���擾����
  std::vector<CompModel*>::iterator it = std::find(models.begin(), models.end(), model);
  assert(it != models.end());

  models.erase(it);
}

// �`�惂�f���o�^�S����
void ModelRenderer::Clear()
{
  models.clear();
}

void ModelRenderer::RenderDynamic(ID3D11DeviceContext* dc, const RenderContext& rc, ModelShader* shader)
{
  // �`��̏���
  shader->Begin(dc, rc);

  // ���f����`��
  for (auto& model : models) {
    for (auto& mesh : model->GetResource()->GetMeshes()) {
      for (auto& subset : mesh.subsets) {
        //	 ���_�E�C���f�b�N�X�o�b�t�@���ݒ�
        shader->SetBuffers(dc, model->GetNodes(), mesh);

        //	 �T�u�Z�b�g�P�ʂŕ`��
        shader->DrawSubset(dc, subset);
      }
    }
  }

  //	�V�F�[�_�[�̏I������
  shader->End(dc);
}

void ModelRenderer::RenderStatic(ID3D11DeviceContext* dc, const RenderContext& rc, StaticModelShader* shader)
{
  staticRenderer.Render(dc, rc, shader);
}
