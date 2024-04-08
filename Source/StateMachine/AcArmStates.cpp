#include "AcArmStates.h"
#include "Component/CompArmoredCore.h"
#include "Component/CompGun.h"

AcArmShot::AcArmShot(CompArmoredCore* owner, const ModelAnimator::ANIM_AREA& area) :State(owner), armArea(area), animator(&owner->GetModel()->animator)
{
  // �E�肩���肩�𔻒�
  if (armArea == ModelAnimator::ANIM_AREA::ARM_RIGHT)
    gun = dynamic_cast<CompGun*>(owner->GetRightWeapon());
  else
    gun = dynamic_cast<CompGun*>(owner->GetLeftWeapon());
}

void AcArmShot::Enter()
{
  // �e�������[�h���Ȃ牽�����Ȃ�
  if (gun->CanExecute() == false) {
    // �E�r�̏ꍇ
    if (armArea == ModelAnimator::ANIM_AREA::ARM_RIGHT)
      owner->GetRightArmSM().ChangeState(CompArmoredCore::ARM_STATE::NONE);

    // ���r�̏ꍇ
    if (armArea == ModelAnimator::ANIM_AREA::ARM_LEFT)
      owner->GetLeftArmSM().ChangeState(CompArmoredCore::ARM_STATE::NONE);

    return;
  }

  animator->PlayAnimation(armArea, animator->FindAnimationId("Shot_Enter"), false);
}

void AcArmShot::Execute(const float& elapsedTime)
{
  WeakObject target = owner->GetAttackTarget();

  // �G�̕����Ɍ���
  if (target.expired() == false) {
    // �x�N�g����XZ���ʂɉ��H
    DirectX::XMFLOAT3 targetVecNorm = owner->GetAttackTargetVec();
    targetVecNorm.y = 0.0f;
    targetVecNorm = Mathf::Normalize(targetVecNorm); // ���K��

    owner->Turn(elapsedTime, targetVecNorm, CompArmoredCore::ABRUPT_TURN_SPEED);
  }

  // �A�j���[�V������؂�ւ��邩�m�F
  bool changeAnimation = false;
  AnimeChangeCheck(changeAnimation);

  // �A�j���[�V�����̕ύX
  if (changeAnimation == true) {
    UpdateAnimeState();
  }

  // �Đ����̃A�j���[�V���������[�v��������
  else if (animationState == ANIMATION_STATE::LOOP)
  {
    // �ˌ��ς݂͉������Ȃ�
    if (isShot == true)return;

    // �A�j���[�V��������U���̃^�C�~���O���擾����
    if (animator->CheckEvent(armArea, ANIMATION_EVENT::ATTACK)) {
      isShot = true;
      gun->Execute(target.lock().get());
    }
  }
}

void AcArmShot::Exit()
{
  animationState = 0;
}

void AcArmShot::AnimeChangeCheck(bool& changeAnime)
{
  // �A�j���[�V�����I��
  if (animator->IsPlayAnimation(armArea) == false) {
    changeAnime = true;
  }
  else if (animationState == ANIMATION_STATE::LOOP) {
    // �}�K�W���̒e����0�ɂȂ�����A�ˌ��I��
    if (gun->CanExecute() == false) {
      changeAnime = true;
      return;
    }

    // �A�j���[�V�����C�x���g�̔���
    if (animator->CheckEvent(armArea, ANIMATION_EVENT::CANCEL_ACCEPT)) {
      isShot = false;

      // �U�����͂�����������ˌ��I��
      // �E�r�̏ꍇ
      if (armArea == ModelAnimator::ANIM_AREA::ARM_RIGHT)
        changeAnime = !owner->InputRightHand();

      // ���r�̏ꍇ
      if (armArea == ModelAnimator::ANIM_AREA::ARM_LEFT)
        changeAnime = !owner->InputLeftHand();
    }
  }
}

void AcArmShot::UpdateAnimeState()
{
  switch (animationState)
  {
  case ANIMATION_STATE::ENTER: // �ˌ��A�j���[�V���� ( ���[�v�Đ� )
    animator->PlayAnimation(armArea, animator->FindAnimationId("Shot_Loop"), true);
    animator->SetAnimationSpeed(armArea, gun->GetAnimeSpeed());

    animationState = ANIMATION_STATE::LOOP;
    break;
  case ANIMATION_STATE::LOOP: // �ˌ��I���A�j���[�V����
    animator->PlayAnimation(armArea, animator->FindAnimationId("Shot_End"), false);

    animationState = ANIMATION_STATE::END;
    break;
  case ANIMATION_STATE::END: // �X�e�[�g�𔲂���
    // �E�r�̏ꍇ
    if (armArea == ModelAnimator::ANIM_AREA::ARM_RIGHT)
      owner->GetRightArmSM().ChangeState(CompArmoredCore::ARM_STATE::NONE);

    // ���r�̏ꍇ
    if (armArea == ModelAnimator::ANIM_AREA::ARM_LEFT)
      owner->GetLeftArmSM().ChangeState(CompArmoredCore::ARM_STATE::NONE);

    break;
  default: // ��O����
    assert(!"Arm Animation State is Error !!!");
    break;
  }
}
