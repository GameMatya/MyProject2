#pragma once

// é¿çsîªíË
template<class Owner>
class JudgmentBase
{
public:
	JudgmentBase(Owner* owner) :owner(owner) {}
	virtual bool Judgment() = 0;

protected:
	Owner* owner;

};