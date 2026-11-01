/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/

#include "precompiled.h"

CReGameHookchains g_ReGameHookchains;

void EXT_FUNC Regamedll_ChangeString_api(char *&dest, const char *source)
{
	size_t len = Q_strlen(source);
	if (dest == nullptr || Q_strlen(dest) != len) {
		delete [] dest;
		dest = new char [len + 1];
	}

	Q_strcpy(dest, source);
}

void EXT_FUNC RadiusDamage_api(Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType)
{
	RadiusDamage(vecSrc, pevInflictor, pevAttacker, flDamage, flRadius, iClassIgnore, bitsDamageType);
}

void EXT_FUNC ClearMultiDamage_api()
{
	ClearMultiDamage();
}

void EXT_FUNC ApplyMultiDamage_api(entvars_t *pevInflictor, entvars_t *pevAttacker)
{
	ApplyMultiDamage(pevInflictor, pevAttacker);
}

void EXT_FUNC AddMultiDamage_api(entvars_t *pevInflictor, CBaseEntity *pEntity, float flDamage, int bitsDamageType)
{
	AddMultiDamage(pevInflictor, pEntity, flDamage, bitsDamageType);
}

int EXT_FUNC Cmd_Argc_api()
{
	return CMD_ARGC_();
}

const char *EXT_FUNC Cmd_Argv_api(int i)
{
	return CMD_ARGV_(i);
}

CGrenade *EXT_FUNC PlantBomb_api(entvars_t *pevOwner, Vector &vecStart, Vector &vecVelocity)
{
	return CGrenade::ShootSatchelCharge(pevOwner, vecStart, vecVelocity);
}

CGib *EXT_FUNC SpawnHeadGib_api(entvars_t *pevVictim)
{
	return CGib::SpawnHeadGib(pevVictim);
}

void EXT_FUNC SpawnRandomGibs_api(entvars_t *pevVictim, int cGibs, int human)
{
	CGib::SpawnRandomGibs(pevVictim, cGibs, human);
}

void EXT_FUNC UTIL_RestartOther_api(const char *szClassname)
{
	UTIL_RestartOther(szClassname);
}

void EXT_FUNC UTIL_ResetEntities_api()
{
	UTIL_ResetEntities();
}

void EXT_FUNC UTIL_RemoveOther_api(const char *szClassname, int nCount)
{
	UTIL_RemoveOther(szClassname, nCount);
}

void EXT_FUNC UTIL_DecalTrace_api(TraceResult *pTrace, int decalNumber)
{
	UTIL_DecalTrace(pTrace, decalNumber);
}

void EXT_FUNC UTIL_Remove_api(CBaseEntity *pEntity)
{
	UTIL_Remove(pEntity);
}

int EXT_FUNC AddAmmoNameToAmmoRegistry_api(const char *szAmmoname)
{
	return AddAmmoNameToAmmoRegistry(szAmmoname);
}

void EXT_FUNC TextureTypePlaySound_api(TraceResult *ptr, Vector vecSrc, Vector vecEnd, int iBulletType)
{
	TEXTURETYPE_PlaySound(ptr, vecSrc, vecEnd, iBulletType);
}

CWeaponBox *EXT_FUNC CreateWeaponBox_api(CBasePlayerItem *pItem, CBasePlayer *pPlayerOwner, const char *modelName, Vector &origin, Vector &angles, Vector &velocity, float lifeTime, bool packAmmo)
{
	return CreateWeaponBox(pItem, pPlayerOwner, modelName, origin, angles, velocity, lifeTime < 0.0 ? CGameRules::GetItemKillDelay() : lifeTime, packAmmo);
}

CGrenade *EXT_FUNC SpawnGrenade_api(WeaponIdType weaponId, entvars_t *pevOwner, Vector &vecSrc, Vector &vecThrow, float time, int iTeam, unsigned short usEvent)
{
	switch (weaponId)
	{
		case WEAPON_HEGRENADE:
			return CGrenade::ShootTimed2(pevOwner, vecSrc, vecThrow, time, iTeam, usEvent);
		case WEAPON_FLASHBANG:
			return CGrenade::ShootTimed(pevOwner, vecSrc, vecThrow, time);
		case WEAPON_SMOKEGRENADE:
			return CGrenade::ShootSmokeGrenade(pevOwner, vecSrc, vecThrow, time, usEvent);
		case WEAPON_C4:
			return CGrenade::ShootSatchelCharge(pevOwner, vecSrc, vecThrow);
	}

	return nullptr;
}

// ls2: orders for a bot from a plugin
static CCSBot *ApiBot(CBasePlayer *pPlayer)
{
	if (!pPlayer || !pPlayer->IsBot() || !pPlayer->IsAlive())
		return nullptr;

	CCSBot *pBot = static_cast<CCSBot *>(pPlayer);
	return pBot->GetLastKnownArea() ? pBot : nullptr;
}

bool EXT_FUNC CSBot_MoveTo_api(CBasePlayer *pPlayer, const Vector &pos, bool hurry)
{
	CCSBot *pBot = ApiBot(pPlayer);
	if (!pBot)
		return false;

	if (hurry)
		pBot->Run();

	pBot->MoveTo(&pos, hurry ? FASTEST_ROUTE : SAFEST_ROUTE);
	return true;
}

bool EXT_FUNC CSBot_HideNear_api(CBasePlayer *pPlayer, const Vector &pos, float duration, float range)
{
	CCSBot *pBot = ApiBot(pPlayer);
	if (!pBot)
		return false;

	Vector at = pos;
	CNavArea *area = TheNavAreaGrid.GetNearestNavArea(&at);
	return area && pBot->TryToHide(area, duration, range, false, false);
}

const char *EXT_FUNC CSBot_StateName_api(CBasePlayer *pPlayer)
{
	CCSBot *pBot = ApiBot(pPlayer);
	return pBot ? pBot->GetStateName() : "";
}

// Keep actual shortest distances for callers estimating item detours. The
// ordinary PathCost also includes tactical/crouch penalties and is not a length.
class BotTravelDistance
{
public:
	CCSBot *bot;
	float operator()(CNavArea *area, CNavArea *from, const CNavLadder *ladder)
	{
		if (from && !CSBot_CanTravel(bot, *from->GetCenter(), *area->GetCenter(), true))
			return -1.0f;
		return ShortestPathCost()(area, from, ladder);
	}
};

float EXT_FUNC CSBot_PathLength_api(CBasePlayer *pPlayer, const Vector &pos)
{
	CCSBot *pBot = ApiBot(pPlayer);
	if (!pBot)
		return -1.0f;

	Vector at = pos;
	CNavArea *goal = TheNavAreaGrid.GetNearestNavArea(&at);
	if (!goal)
		return -1.0f;

	BotTravelDistance cost{pBot};
	if (pBot->GetLastKnownArea() == goal)
	{
		Vector feet(pBot->pev->origin.x, pBot->pev->origin.y, pBot->GetFeetZ());
		if (!CSBot_CanTravel(pBot, feet, pos, true))
			return -1.0f;
	}
	return NavAreaTravelDistance(pBot->GetLastKnownArea(), goal, cost);
}

bool EXT_FUNC CSBot_Idle_api(CBasePlayer *pPlayer)
{
	CCSBot *pBot = ApiBot(pPlayer);
	if (!pBot)
		return false;

	pBot->Idle();
	return true;
}

bool EXT_FUNC CSBot_ShootAt_api(CBasePlayer *pPlayer, const Vector &pos, float duration)
{
	CCSBot *pBot = ApiBot(pPlayer);
	if (!pBot)
		return false;

	pBot->ShootAtSpot(pos, duration);
	return true;
}

bool EXT_FUNC CSBot_RetreatFrom_api(CBasePlayer *pPlayer, const Vector &from, float range, float duration)
{
	CCSBot *pBot = ApiBot(pPlayer);
	return pBot && pBot->RetreatFrom(from, range, duration);
}

void EXT_FUNC CSBot_MarkDanger_api(int team, const Vector &pos, float range, float amount)
{
	if ((team != TERRORIST && team != CT) || range <= 0.0f)
		return;

	for (auto area : TheNavAreaList)
	{
		const Vector *center = area->GetCenter();
		float dist = (*center - pos).Length();
		if (dist > range)
			continue;

		// the danger falls off to nothing at the range; raised up to it, never piled up
		float want = amount * (1.0f - dist / range);
		float have = area->GetDanger(team - 1);
		if (want <= have)
			continue;

		TraceResult result;
		UTIL_TraceLine(pos, *center + Vector(0, 0, HalfHumanHeight), ignore_monsters, nullptr, &result);
		if (result.flFraction < 1.0f)
			continue;

		area->IncreaseDanger(team - 1, want - have);
	}
}

void EXT_FUNC CSBot_SetObstacles_api(const float *boxes, int count)
{
	SetNavObstacles(boxes, count);
}

bool EXT_FUNC CSBot_Visible_api(CBasePlayer *player, const Vector &pos)
{
	// Sight does not require a previously visited nav area (a just-spawned
	// or paused bot still has eyes, even before its first navigation think).
	return player && player->IsBot() && player->IsAlive() &&
		static_cast<CCSBot *>(player)->IsVisible(&pos, true);
}

bool EXT_FUNC CSBot_Repath_api(CBasePlayer *player)
{
	CCSBot *bot = ApiBot(player);
	return bot && bot->Repath();
}

ReGameFuncs_t g_ReGameApiFuncs = {
	CREATE_NAMED_ENTITY,

	Regamedll_ChangeString_api,

	RadiusDamage_api,
	ClearMultiDamage_api,
	ApplyMultiDamage_api,
	AddMultiDamage_api,

	UTIL_FindEntityByString,

	AddEntityHashValue,
	RemoveEntityHashValue,

	Cmd_Argc_api,
	Cmd_Argv_api,

	PlantBomb_api,

	SpawnHeadGib_api,
	SpawnRandomGibs_api,

	UTIL_RestartOther_api,
	UTIL_ResetEntities_api,
	UTIL_RemoveOther_api,
	UTIL_DecalTrace_api,
	UTIL_Remove_api,

	AddAmmoNameToAmmoRegistry_api,
	TextureTypePlaySound_api,
	CreateWeaponBox_api,
	SpawnGrenade_api,

	CSBot_MoveTo_api,
	CSBot_HideNear_api,
	CSBot_StateName_api,
	CSBot_PathLength_api,

	CSBot_Idle_api,
	CSBot_ShootAt_api,
	CSBot_RetreatFrom_api,
	CSBot_MarkDanger_api,
	CSBot_SetObstacles_api,
	CSBot_Visible_api,
	CSBot_Repath_api,
};

GAMEHOOK_REGISTRY(CBasePlayer_Spawn);
GAMEHOOK_REGISTRY(CBasePlayer_Precache);
GAMEHOOK_REGISTRY(CBasePlayer_ObjectCaps);
GAMEHOOK_REGISTRY(CBasePlayer_Classify);
GAMEHOOK_REGISTRY(CBasePlayer_TraceAttack);
GAMEHOOK_REGISTRY(CBasePlayer_TakeDamage);
GAMEHOOK_REGISTRY(CBasePlayer_TakeHealth);
GAMEHOOK_REGISTRY(CBasePlayer_Killed);
GAMEHOOK_REGISTRY(CBasePlayer_AddPoints);
GAMEHOOK_REGISTRY(CBasePlayer_AddPointsToTeam);
GAMEHOOK_REGISTRY(CBasePlayer_AddPlayerItem);
GAMEHOOK_REGISTRY(CBasePlayer_RemovePlayerItem);
GAMEHOOK_REGISTRY(CBasePlayer_GiveAmmo);
GAMEHOOK_REGISTRY(CBasePlayer_ResetMaxSpeed);
GAMEHOOK_REGISTRY(CBasePlayer_Jump);
GAMEHOOK_REGISTRY(CBasePlayer_Duck);
GAMEHOOK_REGISTRY(CBasePlayer_PreThink);
GAMEHOOK_REGISTRY(CBasePlayer_PostThink);
GAMEHOOK_REGISTRY(CBasePlayer_UpdateClientData);
GAMEHOOK_REGISTRY(CBasePlayer_ImpulseCommands);
GAMEHOOK_REGISTRY(CBasePlayer_RoundRespawn);
GAMEHOOK_REGISTRY(CBasePlayer_Blind);

GAMEHOOK_REGISTRY(CBasePlayer_Observer_IsValidTarget);
GAMEHOOK_REGISTRY(CBasePlayer_SetAnimation);
GAMEHOOK_REGISTRY(CBasePlayer_GiveDefaultItems);
GAMEHOOK_REGISTRY(CBasePlayer_GiveNamedItem);
GAMEHOOK_REGISTRY(CBasePlayer_AddAccount);
GAMEHOOK_REGISTRY(CBasePlayer_GiveShield);
GAMEHOOK_REGISTRY(CBasePlayer_SetClientUserInfoModel);
GAMEHOOK_REGISTRY(CBasePlayer_SetClientUserInfoName);
GAMEHOOK_REGISTRY(CBasePlayer_HasRestrictItem);
GAMEHOOK_REGISTRY(CBasePlayer_DropPlayerItem);
GAMEHOOK_REGISTRY(CBasePlayer_DropShield);
GAMEHOOK_REGISTRY(CBasePlayer_OnSpawnEquip);
GAMEHOOK_REGISTRY(CBasePlayer_Radio);
GAMEHOOK_REGISTRY(CBasePlayer_Disappear);
GAMEHOOK_REGISTRY(CBasePlayer_MakeVIP);
GAMEHOOK_REGISTRY(CBasePlayer_MakeBomber);
GAMEHOOK_REGISTRY(CBasePlayer_StartObserver);
GAMEHOOK_REGISTRY(CBasePlayer_GetIntoGame);

GAMEHOOK_REGISTRY(CBaseAnimating_ResetSequenceInfo);

GAMEHOOK_REGISTRY(GetForceCamera);
GAMEHOOK_REGISTRY(PlayerBlind);
GAMEHOOK_REGISTRY(RadiusFlash_TraceLine);
GAMEHOOK_REGISTRY(RoundEnd);
GAMEHOOK_REGISTRY(InstallGameRules);
GAMEHOOK_REGISTRY(PM_Init);
GAMEHOOK_REGISTRY(PM_Move);
GAMEHOOK_REGISTRY(PM_AirMove);
GAMEHOOK_REGISTRY(HandleMenu_ChooseAppearance);
GAMEHOOK_REGISTRY(HandleMenu_ChooseTeam);
GAMEHOOK_REGISTRY(ShowMenu);
GAMEHOOK_REGISTRY(ShowVGUIMenu);
GAMEHOOK_REGISTRY(BuyGunAmmo);
GAMEHOOK_REGISTRY(BuyWeaponByWeaponID);
GAMEHOOK_REGISTRY(InternalCommand);

GAMEHOOK_REGISTRY(CSGameRules_FShouldSwitchWeapon);
GAMEHOOK_REGISTRY(CSGameRules_GetNextBestWeapon);
GAMEHOOK_REGISTRY(CSGameRules_FlPlayerFallDamage);
GAMEHOOK_REGISTRY(CSGameRules_FPlayerCanTakeDamage);
GAMEHOOK_REGISTRY(CSGameRules_PlayerSpawn);
GAMEHOOK_REGISTRY(CSGameRules_FPlayerCanRespawn);
GAMEHOOK_REGISTRY(CSGameRules_GetPlayerSpawnSpot);
GAMEHOOK_REGISTRY(CSGameRules_ClientUserInfoChanged);
GAMEHOOK_REGISTRY(CSGameRules_PlayerKilled);
GAMEHOOK_REGISTRY(CSGameRules_DeathNotice);
GAMEHOOK_REGISTRY(CSGameRules_CanHavePlayerItem);
GAMEHOOK_REGISTRY(CSGameRules_DeadPlayerWeapons);
GAMEHOOK_REGISTRY(CSGameRules_ServerDeactivate);
GAMEHOOK_REGISTRY(CSGameRules_CheckMapConditions);
GAMEHOOK_REGISTRY(CSGameRules_CleanUpMap);
GAMEHOOK_REGISTRY(CSGameRules_RestartRound);
GAMEHOOK_REGISTRY(CSGameRules_CheckWinConditions);
GAMEHOOK_REGISTRY(CSGameRules_RemoveGuns);
GAMEHOOK_REGISTRY(CSGameRules_GiveC4);
GAMEHOOK_REGISTRY(CSGameRules_ChangeLevel);
GAMEHOOK_REGISTRY(CSGameRules_GoToIntermission);
GAMEHOOK_REGISTRY(CSGameRules_BalanceTeams);
GAMEHOOK_REGISTRY(CSGameRules_OnRoundFreezeEnd);
GAMEHOOK_REGISTRY(PM_UpdateStepSound);
GAMEHOOK_REGISTRY(CBasePlayer_StartDeathCam);
GAMEHOOK_REGISTRY(CBasePlayer_SwitchTeam);
GAMEHOOK_REGISTRY(CBasePlayer_CanSwitchTeam);
GAMEHOOK_REGISTRY(CBasePlayer_ThrowGrenade);
GAMEHOOK_REGISTRY(CSGameRules_CanPlayerHearPlayer);
GAMEHOOK_REGISTRY(CWeaponBox_SetModel);
GAMEHOOK_REGISTRY(CGrenade_DefuseBombStart);
GAMEHOOK_REGISTRY(CGrenade_DefuseBombEnd);
GAMEHOOK_REGISTRY(CGrenade_ExplodeHeGrenade);
GAMEHOOK_REGISTRY(CGrenade_ExplodeFlashbang);
GAMEHOOK_REGISTRY(CGrenade_ExplodeSmokeGrenade);
GAMEHOOK_REGISTRY(CGrenade_ExplodeBomb);
GAMEHOOK_REGISTRY(ThrowHeGrenade);
GAMEHOOK_REGISTRY(ThrowFlashbang);
GAMEHOOK_REGISTRY(ThrowSmokeGrenade);
GAMEHOOK_REGISTRY(PlantBomb);
GAMEHOOK_REGISTRY(CBasePlayer_SetSpawnProtection);
GAMEHOOK_REGISTRY(CBasePlayer_RemoveSpawnProtection);
GAMEHOOK_REGISTRY(IsPenetrableEntity);
GAMEHOOK_REGISTRY(CBasePlayer_HintMessageEx);
GAMEHOOK_REGISTRY(CBasePlayer_UseEmpty);
GAMEHOOK_REGISTRY(CBasePlayerWeapon_CanDeploy);
GAMEHOOK_REGISTRY(CBasePlayerWeapon_DefaultDeploy);
GAMEHOOK_REGISTRY(CBasePlayerWeapon_DefaultReload);
GAMEHOOK_REGISTRY(CBasePlayerWeapon_DefaultShotgunReload);
GAMEHOOK_REGISTRY(CBasePlayer_DropIdlePlayer);

GAMEHOOK_REGISTRY(CreateWeaponBox);

GAMEHOOK_REGISTRY(SpawnHeadGib);
GAMEHOOK_REGISTRY(SpawnRandomGibs);
GAMEHOOK_REGISTRY(CGib_Spawn);
GAMEHOOK_REGISTRY(CGib_BounceGibTouch);
GAMEHOOK_REGISTRY(CGib_WaitTillLand);

GAMEHOOK_REGISTRY(CBaseEntity_FireBullets);
GAMEHOOK_REGISTRY(CBaseEntity_FireBuckshots);
GAMEHOOK_REGISTRY(CBaseEntity_FireBullets3);

GAMEHOOK_REGISTRY(CBasePlayer_Observer_SetMode);
GAMEHOOK_REGISTRY(CBasePlayer_Observer_FindNextPlayer);

GAMEHOOK_REGISTRY(CBasePlayer_Pain);
GAMEHOOK_REGISTRY(CBasePlayer_DeathSound);
GAMEHOOK_REGISTRY(CBasePlayer_JoiningThink);

GAMEHOOK_REGISTRY(FreeGameRules);
GAMEHOOK_REGISTRY(PM_LadderMove);
GAMEHOOK_REGISTRY(PM_WaterJump);
GAMEHOOK_REGISTRY(PM_CheckWaterJump);
GAMEHOOK_REGISTRY(PM_Jump);
GAMEHOOK_REGISTRY(PM_Duck);
GAMEHOOK_REGISTRY(PM_UnDuck);
GAMEHOOK_REGISTRY(PM_PlayStepSound);
GAMEHOOK_REGISTRY(PM_AirAccelerate);
GAMEHOOK_REGISTRY(ClearMultiDamage);
GAMEHOOK_REGISTRY(AddMultiDamage);
GAMEHOOK_REGISTRY(ApplyMultiDamage);
GAMEHOOK_REGISTRY(BuyItem);
GAMEHOOK_REGISTRY(CSGameRules_Think);
GAMEHOOK_REGISTRY(CSGameRules_TeamFull);
GAMEHOOK_REGISTRY(CSGameRules_TeamStacked);
GAMEHOOK_REGISTRY(CSGameRules_PlayerGotWeapon);
GAMEHOOK_REGISTRY(CBotManager_OnEvent);
GAMEHOOK_REGISTRY(CBasePlayer_CheckTimeBasedDamage);
GAMEHOOK_REGISTRY(CBasePlayer_EntSelectSpawnPoint);
GAMEHOOK_REGISTRY(CBasePlayerWeapon_ItemPostFrame);
GAMEHOOK_REGISTRY(CBasePlayerWeapon_KickBack);
GAMEHOOK_REGISTRY(CBasePlayerWeapon_SendWeaponAnim);
GAMEHOOK_REGISTRY(CSGameRules_SendDeathMessage);

GAMEHOOK_REGISTRY(CBasePlayer_PlayerDeathThink);
GAMEHOOK_REGISTRY(CBasePlayer_Observer_Think);
GAMEHOOK_REGISTRY(CBasePlayer_RemoveAllItems);
GAMEHOOK_REGISTRY(CBasePlayer_UpdateStatusBar);
GAMEHOOK_REGISTRY(CBasePlayer_TakeDamageImpulse);

GAMEHOOK_REGISTRY(SendSayMessage);
GAMEHOOK_REGISTRY(CSBot_SeesPlayer);
GAMEHOOK_REGISTRY(CSBot_CanTravel);

int CReGameApi::GetMajorVersion() {
	return REGAMEDLL_API_VERSION_MAJOR;
}

int CReGameApi::GetMinorVersion() {
	return REGAMEDLL_API_VERSION_MINOR;
}

const ReGameFuncs_t *CReGameApi::GetFuncs() {
	return &g_ReGameApiFuncs;
}

IReGameHookchains *CReGameApi::GetHookchains() {
	return &g_ReGameHookchains;
}

CGameRules *CReGameApi::GetGameRules() {
	return g_pGameRules;
}

WeaponInfoStruct *CReGameApi::GetWeaponInfo(int weaponID) {
	return ::GetWeaponInfo(weaponID);
}

WeaponInfoStruct *CReGameApi::GetWeaponInfo(const char *weaponName) {
	return ::GetWeaponInfo(weaponName);
}

playermove_t *CReGameApi::GetPlayerMove() {
	return pmove;
}

WeaponSlotInfo *CReGameApi::GetWeaponSlot(WeaponIdType weaponID) { return ::GetWeaponSlot(weaponID); }
WeaponSlotInfo *CReGameApi::GetWeaponSlot(const char *weaponName) { return ::GetWeaponSlot(weaponName); }

ItemInfo *CReGameApi::GetItemInfo(WeaponIdType weaponID) { return &CBasePlayerItem::m_ItemInfoArray[weaponID]; }
AmmoInfo *CReGameApi::GetAmmoInfo(AmmoType ammoID) { return &CBasePlayerItem::m_AmmoInfoArray[ammoID]; }

AmmoInfoStruct *CReGameApi::GetAmmoInfoEx(AmmoType ammoID) { return ::GetAmmoInfo(ammoID); }
AmmoInfoStruct *CReGameApi::GetAmmoInfoEx(const char *ammoName) { return ::GetAmmoInfo(ammoName); }

bool CReGameApi::BGetICSEntity(const char *pchVersion) const
{
	if (!Q_stricmp(pchVersion, CSENTITY_API_INTERFACE_VERSION))
	{
		return true;
	}

	return false;
}

bool CReGameApi::BGetIGameRules(const char *pchVersion) const
{
	if (!Q_stricmp(pchVersion, GAMERULES_API_INTERFACE_VERSION))
	{
		return true;
	}

	return false;
}

EXPOSE_SINGLE_INTERFACE(CReGameApi, IReGameApi, VRE_GAMEDLL_API_VERSION);
