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

#ifndef ACORE_WAYPOINTMOVEMENTGENERATOR_H
#define ACORE_WAYPOINTMOVEMENTGENERATOR_H

/** PathMovementGenerator is used to generate movements
 * of waypoints and flight paths.  Each serves the purpose
 * of generate activities so that it generates updated
 * packets for the players.
 */

#include "MovementGenerator.h"
#include "Player.h"
#include "WaypointMgr.h"

#define FLIGHT_TRAVEL_UPDATE  100
#define TIMEDIFF_NEXT_WP      250

template<class T, class P>
class PathMovementBase
{
public:
    PathMovementBase() : i_path(), i_currentNode(0) {}
    PathMovementBase(P path) : i_path(path), i_currentNode(0) {}
    virtual ~PathMovementBase() {};

    uint32 GetCurrentNode() const { return i_currentNode; }

protected:
    P i_path;
    uint32 i_currentNode;
};

template<class T>
class WaypointMovementGenerator;

template<>
class WaypointMovementGenerator<Creature> : public MovementGeneratorMedium< Creature, WaypointMovementGenerator<Creature> >,
    public PathMovementBase<Creature, WaypointPath const*>
{
public:
    WaypointMovementGenerator(uint32 _path_id = 0, bool _repeating = true, bool _stalled = false)
        : PathMovementBase((WaypointPath const*)nullptr), i_nextMoveTime(0), m_isArrivalDone(false), path_id(_path_id), repeating(_repeating), stalled(_stalled)  {}
    ~WaypointMovementGenerator() { i_path = nullptr; }
    void DoInitialize(Creature*);
    void DoFinalize(Creature*);
    void DoReset(Creature*);
    bool DoUpdate(Creature*, uint32 diff);
    void Pause(uint32 timer = 0);
    void Resume(uint32 overrideTimer/* = 0*/);

    void MovementInform(Creature*);

    MovementGeneratorType GetMovementGeneratorType() { return WAYPOINT_MOTION_TYPE; }

    // now path movement implmementation
    void LoadPath(Creature*);

private:
    void Stop(int32 time) { i_nextMoveTime.Reset(time);}

    bool Stopped() { return !i_nextMoveTime.Passed();}

    bool CanMove(int32 diff)
    {
        i_nextMoveTime.Update(diff);
        return i_nextMoveTime.Passed();
    }

    void OnArrived(Creature*);
    bool StartMove(Creature*);

    void StartMoveNow(Creature* creature)
    {
        i_nextMoveTime.Reset(0);
        StartMove(creature);
    }

    TimeTrackerSmall i_nextMoveTime;
    bool m_isArrivalDone;
    uint32 path_id;
    bool repeating;
    bool stalled;
};

/** FlightPathMovementGenerator generates movement of the player for the paths
 * and hence generates ground and activities for the player.
 */
class FlightPathMovementGenerator : public MovementGeneratorMedium< Player, FlightPathMovementGenerator >,
    public PathMovementBase<Player, TaxiPathNodeList>
{
    public:
        explicit FlightPathMovementGenerator(uint32 startNode = 0)
        {
            i_currentNode = startNode;
            _endGridX = 0.0f;
            _endGridY = 0.0f;
            _endMapId = 0;
            _preloadTargetNode = 0;
        }
        void LoadPath(Player* player);
        void DoInitialize(Player*);
        void DoReset(Player*);
        void DoFinalize(Player*);
        bool DoUpdate(Player*, uint32);
        MovementGeneratorType GetMovementGeneratorType() override { return FLIGHT_MOTION_TYPE; }

        TaxiPathNodeList const& GetPath() { return i_path; }
        uint32 GetPathAtMapEnd() const;
        bool HasArrived() const { return (i_currentNode >= i_path.size()); }
        void SetCurrentNodeAfterTeleport();
        void SkipCurrentNode() { ++i_currentNode; }
        void DoEventIfAny(Player* player, TaxiPathNodeEntry const* node, bool departure);

        bool GetResetPos(Player*, float& x, float& y, float& z);

        void InitEndGridInfo();
        void PreloadEndGrid();

    private:

        float _endGridX;                            //! X coord of last node location
        float _endGridY;                            //! Y coord of last node location
        uint32 _endMapId;                           //! map Id of last node location
        uint32 _preloadTargetNode;                  //! node index where preloading starts

        struct TaxiNodeChangeInfo
        {
            uint32 PathIndex;
            int32 Cost;
        };

        std::deque<TaxiNodeChangeInfo> _pointsForPathSwitch;    //! node indexes and costs where TaxiPath changes
};

#endif
