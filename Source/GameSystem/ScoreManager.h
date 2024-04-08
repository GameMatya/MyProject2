#pragma once

// ヘリ = 300point
const float SCORE_HELICOPTER = 300.0f;
// 敵AC = 2000point
const float SCORE_ENEMY_AC = 2000.0f;

// プレイヤーのHP = 割合によってポイント半減

// ポイントによるスコアランク
enum class SCORE_RANK {
  S = 0,
  A,
  B,
  C,
  D,

  MAX
};

class ScoreManager
{
private:
  ScoreManager() {}

public:
  static ScoreManager& Instance() {
    static ScoreManager instance;
    return instance;
  }

  void Initialize() {
    killCount = 0.0f;
    scorePoint = 0.0f;
  }

  void AddPoint(const float& point) {
    killCount++;
    scorePoint += point;
  }

  void SetPlayerHealthRate(const float& rate);

  SCORE_RANK GetScoreRank();

  const float& GetFinalScore() const { return finalPoint; }
  const float& GetScorePoint() const { return scorePoint; }

  const float& GetHealthRate() const { return healthRate; }
  const float& GetHealthScore() const { return healthScore; }

  const float& GetKillCount() const { return killCount; }

private:
  // スコアランクの条件
  const float RANK_BORDER[(int)SCORE_RANK::MAX] = {
    6000,
    5000,
    3500,
    1000,
    0
  };

  float killCount = 0;
  float scorePoint = 0;
  float finalPoint = 0;
  float healthRate = 0.0f;
  float healthScore = 0.0f;

};