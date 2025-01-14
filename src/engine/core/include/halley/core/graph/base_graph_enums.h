#pragma once

#include <cstdint>

namespace Halley {
 	using GraphNodeId = uint16_t;
	using GraphPinId = uint8_t;
	using GraphElementType = uint8_t;

	enum class GraphNodePinDirection : uint8_t {
		Input,
		Output
	};

	enum class GraphPinSide : uint8_t {
		Undefined,
		Left,
		Right,
		Top,
		Bottom
	};

	struct GraphNodePinType {
		GraphElementType type = 0;
		GraphNodePinDirection direction = GraphNodePinDirection::Input;
		bool isCancellable : 1;
		bool forceHorizontal : 1;
		
		GraphNodePinType(GraphElementType type = 0, GraphNodePinDirection direction = GraphNodePinDirection::Input, bool cancellable = false, bool forceHorizontal = false)
			: type(type)
			, direction(direction)
			, isCancellable(cancellable)
			, forceHorizontal(forceHorizontal)
		{}

		template <typename T>
		GraphNodePinType(T type = T(0), GraphNodePinDirection direction = GraphNodePinDirection::Input, bool cancellable = false, bool forceHorizontal = false)
			: type(static_cast<GraphElementType>(type))
			, direction(direction)
			, isCancellable(cancellable)
			, forceHorizontal(forceHorizontal)
		{}

		[[nodiscard]] bool operator==(const GraphNodePinType& other) const
		{
			return type == other.type && direction == other.direction && isCancellable == other.isCancellable && forceHorizontal == other.forceHorizontal;
		}

		[[nodiscard]] bool operator!=(const GraphNodePinType& other) const
		{
			return !(*this == other);
		}

		[[nodiscard]] GraphPinSide getSide() const
		{
			return direction == GraphNodePinDirection::Input ? GraphPinSide::Left : GraphPinSide::Right;
		}

		[[nodiscard]] bool canConnectTo(const GraphNodePinType& other) const
		{
			return type == other.type && direction != other.direction;
		}

		[[nodiscard]] GraphNodePinType getReverseDirection() const
		{
			return GraphNodePinType(type, direction == GraphNodePinDirection::Input ? GraphNodePinDirection::Output : GraphNodePinDirection::Input);
		}
	};
}
