/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "BattlegroundNA.h"
#include "Player.h"
#include "WorldPacket.h"
#include "WorldStateDefines.h"

BattlegroundNA::BattlegroundNA()
{
    BgObjects.resize(BG_NA_OBJECT_MAX);
}

void BattlegroundNA::StartingEventCloseDoors()
{
    for (uint32 i = BG_NA_OBJECT_DOOR_1; i <= BG_NA_OBJECT_DOOR_4; ++i)
        SpawnBGObject(i, RESPAWN_IMMEDIATELY);
}

void BattlegroundNA::StartingEventOpenDoors()
{
    for (uint32 i = BG_NA_OBJECT_DOOR_1; i <= BG_NA_OBJECT_DOOR_2; ++i)
        DoorOpen(i);

    for (uint32 i = BG_NA_OBJECT_BUFF_1; i <= BG_NA_OBJECT_BUFF_2; ++i)
        SpawnBGObject(i, 90);
}

bool BattlegroundNA::HandlePlayerUnderMap(Player* player)
{
    player->NearTeleportTo(4055.504395f, 2919.660645f, 13.611241f, player->GetOrientation());
    return true;
}

void BattlegroundNA::HandleAreaTrigger(Player* player, uint32 trigger)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    switch (trigger)
    {
        case 4536:                                          // buff trigger?
        case 4537:                                          // buff trigger?
            break;
        // OUTSIDE OF ARENA, TELEPORT!
        case 4917:
        case 5006:
        case 5008:
            player->NearTeleportTo(4054.15f, 2923.7f, 13.4f, player->GetOrientation());
            break;
    }
}

void BattlegroundNA::FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& packet)
{
    packet.Worldstates.emplace_back(WORLD_STATE_BATTLEGROUND_NA_ARENA_SHOW, 1);
    Arena::FillInitialWorldStates(packet);
}

bool BattlegroundNA::SetupBattleground()
{
    // gates
    if (!AddObject(BG_NA_OBJECT_DOOR_1, BG_NA_OBJECT_TYPE_DOOR_1, 4031.854f, 2966.833f, 12.0462f,  -2.648788f, 0, 0, 0.9697962f, -0.2439165f, RESPAWN_IMMEDIATELY)
            || !AddObject(BG_NA_OBJECT_DOOR_2, BG_NA_OBJECT_TYPE_DOOR_2, 4081.179f, 2874.97f,  12.00171f, 0.4928045f, 0, 0, 0.2439165f, 0.9697962f,  RESPAWN_IMMEDIATELY)
            || !AddObject(BG_NA_OBJECT_DOOR_3, BG_NA_OBJECT_TYPE_DOOR_3, 4023.709f, 2981.777f, 10.70117f, -2.648788f, 0, 0, 0.9697962f, -0.2439165f, RESPAWN_IMMEDIATELY)
            || !AddObject(BG_NA_OBJECT_DOOR_4, BG_NA_OBJECT_TYPE_DOOR_4, 4090.064f, 2858.438f, 10.23631f, 0.4928045f, 0, 0, 0.2439165f,  0.9697962f, RESPAWN_IMMEDIATELY)
            // buffs
            || !AddObject(BG_NA_OBJECT_BUFF_1, BG_NA_OBJECT_TYPE_BUFF_1, 4009.189941f, 2895.250000f, 13.052700f, -1.448624f, 0, 0, 0.6626201f, -0.7489557f, 120)
            || !AddObject(BG_NA_OBJECT_BUFF_2, BG_NA_OBJECT_TYPE_BUFF_2, 4103.330078f, 2946.350098f, 13.051300f, -0.06981307f, 0, 0, 0.03489945f, -0.9993908f, 120)
            // Arena Ready Marker
            || !AddObject(BG_NA_OBJECT_READY_MARKER_1, ARENA_READY_MARKER_ENTRY, 4090.46f, 2875.43f, 12.16f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 300)
            || !AddObject(BG_NA_OBJECT_READY_MARKER_2, ARENA_READY_MARKER_ENTRY, 4022.82f, 2966.61f, 12.17f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 300))
    {
        LOG_ERROR("sql.sql", "BatteGroundNA: Failed to spawn some object!");
        return false;
    }

    return true;
}
