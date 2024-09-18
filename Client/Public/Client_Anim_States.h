#pragma once

namespace Client
{
	enum PLAYER_STATE {
		// ���̵�, ��ȭ, �ȱ�, �ٱ�
		STATE_IDLE, STATE_IDLE_CARRY, STATE_TALK_BACK, STATE_TALK, STATE_WALK, STATE_RUN,

		// ���и� ������ �ִ� �ִϸ��̼�
		STATE_SHIELD, STATE_SHIELD_F, STATE_SHIELD_B, STATE_SHIELD_L, STATE_SHIELD_R, STATE_SHIELD_HIT,

		// Į�� ������ �ִ� �ִϸ��̼�
		STATE_SLASH, STATE_SLASH_LP, STATE_SLASH_F, STATE_SLASH_B, STATE_SLASH_L, STATE_SLASH_R, STATE_NORMAL_SLASH_ED, STATE_SPECIAL_SLASH_ED,

		// Į�� ���� �Ѵ� �ִ� �ִϸ��̼�
		STATE_SLASH_SHIELD, STATE_SLASH_SHIELD_F, STATE_SLASH_SHIELD_B, STATE_SLASH_SHIELD_L, STATE_SLASH_SHIELD_R,

		// �� �а� ����
		STATE_PUSH, STATE_PULL,

		// ������ �������
		STATE_ITEM_ST, STATE_Lv25_SWRODGET_ST,

		// ����
		STATE_JUMP,

		// �ڷ� ����
		STATE_DASH_CRASH,

		// ������ �Ծ�����
		STATE_DMG_ELEC, STATE_DMG_QUAKE,

		// ����, ��Ȱ
		STATE_DEATH, STATE_RESURRECTION,

		// �¿� ��ư, Ű ����
		STATE_BUTTON_L, STATE_BUTTON_R, STATE_KEY_OPEN,

		// �ʻ��
		STATE_DASH_ST, STATE_DASH_LP, STATE_DASH_ED,

		// ���
		STATE_CARRY, STATE_LAND_CARRY, STATE_JUMP_CARRY, STATE_THROW,

		STATE_END
	};

	enum POTDEMON_STATE {

		// Pot Demon
		POT_WAIT, POT_WALK, POT_STUN, POT_STUN_WAIT, POT_GETUP, POT_POP, POT_ATTACK_FIRE, POT_ATTACK_JUGGLING, POT_DEPOP, POT_POP_WAIT,

		// Demon King
		KING_POP, KING_WAIT, KING_ATTACK_JUGGLING, KING_ATTACK_FIRE, KING_DEPOP,

		// ������ 2 ��Ÿ����
		KING_PHASE_2_ST,

		// ������ 2 ���̵� ����
		KING_PHASE_2_WAIT,

		// ������� & ��Ÿ����
		KING_PHASE_2_ED, KING_CLONING_APPEAR,

		// �ָ� ���� �� �������
		KING_CLONING_ATK_PUNCH, KING_CLONING_ATK_PUNCH_ED,

		// ������� ������ ���� �Ϸ��� ����
		KING_CLONING_ED_WAIT, KING_CLONING_ED_THROW, KING_CLONING_ED_WAIT_2,

		// ������ �ޱ�, ����
		KING_CLONING_DAMAGE, KING_CLONING_ULT_DAMAGE, KING_ATK_ULT, KING_CLONING_DEAD,

		POTDEMON_STATE_END = -1
	};

	
}

using namespace std;
using namespace Client;