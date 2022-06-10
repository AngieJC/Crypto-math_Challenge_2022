#pragma once
#include "1Round.h"					// 1. 利用确定的中间变量直接恢复密钥
#include "2Rounds.h"				// 2. 精确差分攻击
#include "3-4Rounds.h"				// 3. 立方攻击后遍历密钥，效果并不好，弃用
#include "CubeAttack.h"				// 4. 立方攻击后将超级多项式作为约束添加到MILP中
#include "CubeAttackAccurate.h"		// 5. 在4的基础上多次MILP求解得到唯一密钥
#include "MITM.h"					// 6. 理论上可以攻击到8轮，实际6轮，7轮的复杂度为2^40，8轮的复杂度为2^43，8轮以上不行