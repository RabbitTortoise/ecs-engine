#pragma once
#include <vector>
#include <assert.h>
#include <string>



namespace AStar
{
	enum class PathFindingState
	{
		GoalFound = 1,
		GoalNotFound = 2,
		GoalCannotBeReached = 3

	};


	template<typename State, typename ValueType>
	struct AStarNode
	{
		AStarNode() {};
		AStarNode(AStarNode* p, State s, ValueType g, ValueType h, ValueType f) 
		{
			parent = p;
			state = s;
			gValue = g;
			hValue = h;
			fValue = f;
		}

		AStarNode* parent = nullptr;

		State state;
		ValueType gValue;
		ValueType hValue;
		ValueType fValue;

		int lastActionID = -1;
	};


	template<typename NodeType>
	NodeType* FindFromNodeList(std::vector<NodeType*>& nodeList, NodeType* node)
	{
		bool found = true;
		for (auto nodeFromList : nodeList)
		{
			found = true;
			for (int index = 0; index < nodeFromList->state.size(); index++)
			{
				if (nodeFromList->state[index] != node->state[index])
				{
					found = false;
					break;
				}
			}
			if (found)
			{
				return nodeFromList;
			}
		}
		return nullptr;
	}


	template<typename State, typename NodeType>
	bool FindPossibleMoves(
		std::vector<NodeType*>& openList,
		std::vector<NodeType*>& closedList,
		NodeType* currentNode,
		State goalState,
		float k,
		std::vector<State>& actions,
		std::vector<std::vector<bool>>* walkableMap,
		float (*gCalculation)(NodeType*, State, float),
		float (*hCalculation)(NodeType*, State, float),
		bool (*IsValidAction)(State state, State action, std::vector<std::vector<bool>>* walkableMap))
	{
		int actionID = -1;
		for (auto& action : actions)
		{
			actionID++;

			bool legalAction = IsValidAction(currentNode->state, action, walkableMap);

			if (legalAction)
			{
				NodeType* newNode = new NodeType();
				newNode->state = currentNode->state;
				for (size_t i = 0; i < currentNode->state.size(); ++i) {

					newNode->state[i] += action[i];

				}
				newNode->parent = currentNode;

				CalculateValues(newNode, goalState, k, gCalculation, hCalculation);
				NodeType* sameStateInOpen = FindFromNodeList(openList, newNode);
				NodeType* sameStateInClosed = FindFromNodeList(closedList, newNode);

				if (sameStateInOpen != nullptr) {
					//Update new values for the open node
					if (newNode->gValue < sameStateInOpen->gValue)
					{
						sameStateInOpen->parent = currentNode;
						CalculateValues(sameStateInOpen, goalState, k, gCalculation, hCalculation);
					}
					delete newNode;
					continue;

				}
				if (sameStateInClosed != nullptr)
				{
					//Node already closed
					delete newNode;
					continue;
				}

				newNode->parent = currentNode;
				openList.push_back(newNode);

			}
		}
		return true;
	}



	template<typename State, typename NodeType>
	bool CalculateValues(NodeType* node, State goalState, float k,
		float (*gCalculation)(NodeType*, State, float),
		float (*hCalculation)(NodeType*, State, float)) {

		if (node->parent == nullptr) //First Node
		{
			node->gValue = 0;
		}
		else
		{
			node->gValue = gCalculation(node, goalState, k);
		}

		node->hValue = hCalculation(node, goalState, k);
		node->fValue = node->gValue + node->hValue;
		return true;
	}

	/*template<typename State, typename NodeType>
	bool FindPossibleMoves(
		std::vector<NodeType*>& openList, 
		std::vector<NodeType*>& closedList, 
		NodeType* currentNode, 
		State goalState,
		std::vector<State> actions,
		float (*gCalculation)(NodeType*, State, float),
		float (*hCalculation)(NodeType*, State, float))
	{
		return false;
	}*/

	template<typename State, typename NodeType>
	PathFindingState PathFindStep(
		std::vector<NodeType*>& openList,
		std::vector<NodeType*>& closedList,
		NodeType*& currentNode,
		State goalState,
		float k,
		std::vector<State>& actions,
		int& takenActionOut,
		std::vector<std::vector<bool>>* walkableMap,
		float (*gCalculation)(NodeType*, State, float),
		float (*hCalculation)(NodeType*, State, float),
		bool (*IsValidAction)(State state, State action, std::vector<std::vector<bool>>* walkableMap))
	{
		
		//First Step
		//Erase current node from open nodes if it is there
		bool found = true;
		for (auto openNode = openList.begin(); openNode != openList.end(); openNode++)
		{
			found = true;
			for (size_t i = 0; i < currentNode->state.size(); ++i) {
				if ((*openNode)->state[i] != currentNode->state[i])
				{
					found = false;
					break;
				}
			}
			if (found)
			{
				openList.erase(openNode);
				break;
			}
		}

		//Add current node to closed list
		closedList.push_back(currentNode);

		//Check if we are at goal
		found = true;
		for (size_t i = 0; i < currentNode->state.size(); ++i) {
			if (goalState[i] != currentNode->state[i])
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			return PathFindingState::GoalFound;
		}
		
		//Check if there are no further open nodes
		if (openList.size() == 0)
		{
			return PathFindingState::GoalCannotBeReached;

		}

		//Get the node with smallest fValue
		NodeType* bestNewNode = openList.front();
		for (auto openNode : openList)
		{
			if (openNode->fValue < bestNewNode->fValue)
			{
				bestNewNode = openNode;
			}
		}

		State takenAction;
		for (size_t i = 0; i < currentNode->state.size(); ++i) {
			takenAction[i] = bestNewNode->state[i] - currentNode->state[i];
		}

		currentNode = bestNewNode;


		//Second Step
		//Find all next moves and put them to open list
		FindPossibleMoves(openList, closedList, currentNode, goalState, k, actions, walkableMap, gCalculation, hCalculation, IsValidAction);
		


		int actionID = -1;
		for (auto action : actions)
		{
			found = true;
			for (size_t i = 0; i < currentNode->state.size(); ++i) {
				if (action[i] != currentNode->state[i])
				{
					found = false;
					break;
				}
			}
			actionID++;
			if (found)
			{
				break;
			}
			
		}
		currentNode->lastActionID = actionID;
		takenActionOut = actionID;


		return PathFindingState::GoalNotFound;
	}

	template<typename State, typename NodeType>
	bool InitializePathFinding(
		std::vector<NodeType*>& openList,
		std::vector<NodeType*>& closedList,
		NodeType* currentNode,
		State goalState,
		float k,
		std::vector<State>& actions,
		std::vector<std::vector<bool>>* walkableMap,
		float (*gCalculation)(NodeType*, State, float),
		float (*hCalculation)(NodeType*, State, float),
		bool (*IsValidAction)(State state, State action, std::vector<std::vector<bool>>* walkableMap))
	{
		currentNode->lastActionID = -1;
		CalculateValues(currentNode, goalState, k, gCalculation, hCalculation);
		openList.push_back(currentNode);
		return FindPossibleMoves(openList, closedList, currentNode, goalState, k, actions, walkableMap, gCalculation, hCalculation, IsValidAction);
	}
}


namespace AICourseAStar
{


	struct State
	{
		State()
		{
			GridX = 0;
			GridY = 0;
		}

		State(int x, int y)
		{
			GridX = x;
			GridY = y;
		}

		void Set(glm::vec2 states)
		{
			GridX = (int)states[0];
			GridY = (int)states[1];
		}

		glm::vec2 Get()
		{
			return glm::vec2{ GridX, GridY };
		}

		constexpr int& operator[](std::size_t i)
		{
			if (i == 0)
			{
				return GridX;
			}
			if (i == 1)
			{
				return GridY;
			}
			else
			{
				assert(1 == 1);
			}

		}



		std::string StateToString()
		{
			std::string result = "";
			result += std::to_string(GridX) + ", " + std::to_string(GridY) + ": " + (ValidState ? "True" : "False");
			return result;
		}

		size_t size()
		{
			return 2;
		}

		int GridX = 0;
		int GridY = 0;

		bool ValidState = false;
		int GridDimensions = 0;


		const static bool IsValidState(const State& state)
		{
			if (state.GridX < 0 || state.GridX >= state.GridDimensions || state.GridY < 0 || state.GridY >= state.GridDimensions)
			{
				return false;
			}
			return true;
		}
	};

	static std::vector<State> Actions =
	{
		{0,1},
		{1,0},
		{1,1},
		{0,-1},
		{-1,0},
		{-1,-1},
	};

	static float CalculateHValue(AStar::AStarNode<State, float>* currentNode, State goalState, float k)
	{
		return (float)(k * sqrt(pow(goalState[0] - currentNode->state[0], 2) + pow(goalState[1] - currentNode->state[1], 2) * 1.0f));
		
	}
	static float CalculateGValue(AStar::AStarNode<State, float>* currentNode, State goalState, float k)
	{
		if (currentNode->parent == nullptr) //First Node
		{
			return 0;
		}
		else
		{
			int d = abs(currentNode->state[0] - currentNode->parent->state[0]) + abs(currentNode->state[1] - currentNode->parent->state[1]);
			if (d == 2) //Walk diagonally
			{
				return currentNode->parent->gValue + 1.41421356237309504880168872420f;
			}
			else //Walk to side
			{
				return currentNode->parent->gValue + 1;
			}
		}
	}

	static bool IsValidAction(State currentState, State Action, std::vector<std::vector<bool>>* walkableMap)
	{
		State newState = currentState;
		newState[0] = currentState[0] + Action[0];
		newState[1] = currentState[1] + Action[1];
		bool valid = newState.IsValidState(newState);
		if (valid)
		{
			valid = (*walkableMap)[newState[0]][newState[1]];


			int d = abs(Action[0]) + abs(Action[1]);
			if (d == 2) //Walk diagonally
			{
				State corner1 = currentState;
				State corner2 = currentState;
				if (Action[0] == 1 && Action[1] == 1)
				{
					corner1[0] = currentState[0] + 1;
					corner2[1] = currentState[1] + 1;
					if (!(*walkableMap)[corner1[0]][corner1[1]] || !(*walkableMap)[corner2[0]][corner2[1]])
					{
						valid = false;
					}
				}
				else if (Action[0] == 1 && Action[1] == -1)
				{
					corner1[0] = currentState[0] + 1;
					corner2[1] = currentState[1] - 1;
					if (!(*walkableMap)[corner1[0]][corner1[1]] || !(*walkableMap)[corner2[0]][corner2[1]])
					{
						valid = false;
					}
				}
				else if (Action[0] == -1 && Action[1] == 1)
				{
					corner1[0] = currentState[0] - 1;
					corner2[1] = currentState[1] + 1;
					if (!(*walkableMap)[corner1[0]][corner1[1]] || !(*walkableMap)[corner2[0]][corner2[1]])
					{
						valid = false;
					}
				}
				else if (Action[0] == -1 && Action[1] == -1)
				{
					corner1[0] = currentState[0] - 1;
					corner2[1] = currentState[1] - 1;
					if (!(*walkableMap)[corner1[0]][corner1[1]] || !(*walkableMap)[corner2[0]][corner2[1]])
					{
						valid = false;
					}
				}
			}
		}
		return valid;
	}
}