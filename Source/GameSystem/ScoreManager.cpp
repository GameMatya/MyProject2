#include "ScoreManager.h"

void ScoreManager::SetPlayerHealthRate(const float& rate)
{
  healthRate = rate * 100.0f;
  healthScore = scorePoint * 0.5f * (1.0f - rate);
}

SCORE_RANK ScoreManager::GetScoreRank()
{
  finalPoint = scorePoint;       // ���ă|�C���g
  finalPoint -= healthScore;     // �̗͂̊����Ń|�C���g�𔼌�����

  // �����N�t��
  for (int i = 0; i < static_cast<int>(SCORE_RANK::MAX); ++i) {
    if (finalPoint >= RANK_BORDER[i])
      return static_cast<SCORE_RANK>(i);
  }

  return SCORE_RANK::D;
}
