#pragma once
#include "../entity.h"
#include "script_graph.h"
#include "script_state.h"
#include "script_node_enums.h"
#include "halley/core/graphics/text/text_renderer.h"
#include "halley/time/halleytime.h"

namespace Halley {
	class World;
	class ScriptEnvironment;
	class ScriptGraph;
    class ScriptState;
	class BaseGraphNode;

	class IGraphNodeType {
	public:
		struct SettingType {
			String name;
			String type;
			Vector<String> defaultValue;
		};

		constexpr static Colour4f parameterColour = Colour4f(0.97f, 0.35f, 0.35f);
		using PinType = GraphNodePinType;

		virtual ~IGraphNodeType() = default;

		virtual String getId() const = 0;
		virtual String getName() const = 0;

		virtual Vector<SettingType> getSettingTypes() const;
		virtual void updateSettings(ScriptGraphNode& node, const ScriptGraph& graph, Resources& resources) const;

		virtual gsl::span<const PinType> getPinConfiguration(const ScriptGraphNode& node) const = 0;
        PinType getPin(const ScriptGraphNode& node, size_t n) const;

		virtual std::pair<String, Vector<ColourOverride>> getNodeDescription(const ScriptGraphNode& node, const World* world, const ScriptGraph& graph) const;
		virtual std::pair<String, Vector<ColourOverride>> getPinDescription(const ScriptGraphNode& node, PinType elementType, GraphPinId elementIdx) const;
		
		virtual bool canAdd() const { return true; }
        virtual bool canDelete() const { return true; }

		virtual std::optional<Vector2f> getNodeSize(const BaseGraphNode& node, float curZoom) const { return std::nullopt; }
	};
	
	class IScriptNodeType : public IGraphNodeType {
	public:		
        struct Result {
        	ScriptNodeExecutionState state = ScriptNodeExecutionState::Done;
        	uint8_t outputsActive = 1;
			uint8_t outputsCancelled = 0;
			GraphNodeId nodeRef = 0;
	        Time timeElapsed = 0;

        	[[nodiscard]] Result() = default;
        	[[nodiscard]] Result(ScriptNodeExecutionState state, Time timeElapsed = 0, uint8_t outputsActive = 1, uint8_t outputsCancelled = 0, GraphNodeId nodeRef = 0)
        		: state(state), outputsActive(outputsActive), outputsCancelled(outputsCancelled), nodeRef(nodeRef), timeElapsed(timeElapsed)
        	{}
        };

		virtual ~IScriptNodeType() = default;

		virtual ScriptNodeClassification getClassification() const = 0;

		virtual std::pair<String, Vector<ColourOverride>> getDescription(const ScriptGraphNode& node, const World* world, PinType elementType, GraphPinId elementIdx, const ScriptGraph& graph) const;
		virtual String getShortDescription(const World* world, const ScriptGraphNode& node, const ScriptGraph& graph, GraphPinId elementIdx) const;
		virtual String getLargeLabel(const ScriptGraphNode& node) const;
		virtual String getLabel(const ScriptGraphNode& node) const;
		virtual String getIconName(const ScriptGraphNode& node) const;

        virtual bool canKeepData() const { return false; }
		virtual bool hasDestructor(const ScriptGraphNode& node) const { return false; }
		virtual bool showDestructor() const { return true; }

		virtual std::unique_ptr<IScriptStateData> makeData() const { return {}; }
        virtual void initData(IScriptStateData& data, const ScriptGraphNode& node, const EntitySerializationContext& context, const ConfigNode& nodeData) const {}

		virtual Result update(ScriptEnvironment& environment, Time time, const ScriptGraphNode& node, IScriptStateData* curData) const = 0;
		virtual void destructor(ScriptEnvironment& environment, const ScriptGraphNode& node, IScriptStateData* curData) const = 0;
		virtual bool isStackRollbackPoint(ScriptEnvironment& environment, const ScriptGraphNode& node, GraphPinId outPin, IScriptStateData* curData) const = 0;

		virtual ConfigNode getData(ScriptEnvironment& environment, const ScriptGraphNode& node, size_t pinN, IScriptStateData* curData) const = 0;
		virtual void setData(ScriptEnvironment& environment, const ScriptGraphNode& node, size_t pinN, ConfigNode data, IScriptStateData* curData) const = 0;
        virtual EntityId getEntityId(ScriptEnvironment& environment, const ScriptGraphNode& node, GraphPinId pinN, IScriptStateData* curData) const = 0;
		virtual ConfigNode getDevConData(ScriptEnvironment& environment, const ScriptGraphNode& node, IScriptStateData* curData) const = 0;

		ConfigNode readDataPin(ScriptEnvironment& environment, const ScriptGraphNode& node, size_t pinN) const;
		void writeDataPin(ScriptEnvironment& environment, const ScriptGraphNode& node, size_t pinN, ConfigNode data) const;
		EntityId readEntityId(ScriptEnvironment& environment, const ScriptGraphNode& node, size_t idx) const;
		EntityId readRawEntityId(ScriptEnvironment& environment, const ScriptGraphNode& node, size_t idx) const;
		String getConnectedNodeName(const World* world, const ScriptGraphNode& node, const ScriptGraph& graph, size_t pinN) const;

		struct OutputNode {
			OptionalLite<GraphNodeId> dstNode;
			GraphPinId outputPin;
			GraphPinId inputPin;
		};
		std::array<OutputNode, 8> getOutputNodes(const ScriptGraphNode& node, uint8_t outputActiveMask) const;
		GraphPinId getNthOutputPinIdx(const ScriptGraphNode& node, size_t n) const;

        static String addParentheses(String str);
	};

	template <typename DataType, typename EnvironmentType = ScriptEnvironment>
	class ScriptNodeTypeBase : public IScriptNodeType {
	public:
		static_assert(std::is_base_of_v<IScriptStateData, DataType>);
		static_assert(std::is_base_of_v<ScriptEnvironment, EnvironmentType>);
		
		virtual Result doUpdate(EnvironmentType& environment, Time time, const ScriptGraphNode& node, DataType& curData) const { return Result(ScriptNodeExecutionState::Done); }
		virtual void doDestructor(EnvironmentType& environment, const ScriptGraphNode& node, DataType& curData) const {}
		virtual bool doIsStackRollbackPoint(EnvironmentType& environment, const ScriptGraphNode& node, GraphPinId outPin, DataType& curData) const { return false; }
		virtual void doInitData(DataType& data, const ScriptGraphNode& node, const EntitySerializationContext& context, const ConfigNode& nodeData) const = 0;
		virtual ConfigNode doGetData(EnvironmentType& environment, const ScriptGraphNode& node, size_t pinN, DataType& curData) const { return ConfigNode(); }
		virtual void doSetData(EnvironmentType& environment, const ScriptGraphNode& node, size_t pinN, ConfigNode data, DataType& curData) const {}
		virtual EntityId doGetEntityId(EnvironmentType& environment, const ScriptGraphNode& node, GraphPinId pinN, DataType& curData) const { return EntityId(); }
		virtual ConfigNode doGetDevConData(EnvironmentType& environment, const ScriptGraphNode& node, DataType& curData) const { return {}; }

		std::unique_ptr<IScriptStateData> makeData() const override { return std::make_unique<DataType>(); }
		void initData(IScriptStateData& data, const ScriptGraphNode& node, const EntitySerializationContext& context, const ConfigNode& nodeData) const override { doInitData(dynamic_cast<DataType&>(data), node, context, nodeData); }

		Result update(ScriptEnvironment& environment, Time time, const ScriptGraphNode& node, IScriptStateData* curData) const final override { return doUpdate(dynamic_cast<EnvironmentType&>(environment), time, node, *dynamic_cast<DataType*>(curData)); }
		void destructor(ScriptEnvironment& environment, const ScriptGraphNode& node, IScriptStateData* curData) const final override { return doDestructor(dynamic_cast<EnvironmentType&>(environment), node, *dynamic_cast<DataType*>(curData)); }
		bool isStackRollbackPoint(ScriptEnvironment& environment, const ScriptGraphNode& node, GraphPinId outPin, IScriptStateData* curData) const final override { return doIsStackRollbackPoint(dynamic_cast<EnvironmentType&>(environment), node, outPin, *dynamic_cast<DataType*>(curData)); }
		ConfigNode getData(ScriptEnvironment& environment, const ScriptGraphNode& node, size_t pinN, IScriptStateData* curData) const final override { return doGetData(dynamic_cast<EnvironmentType&>(environment), node, pinN, *dynamic_cast<DataType*>(curData)); }
		void setData(ScriptEnvironment& environment, const ScriptGraphNode& node, size_t pinN, ConfigNode data, IScriptStateData* curData) const final override { doSetData(dynamic_cast<EnvironmentType&>(environment), node, pinN, std::move(data), *dynamic_cast<DataType*>(curData)); }
		EntityId getEntityId(ScriptEnvironment& environment, const ScriptGraphNode& node, GraphPinId pinN, IScriptStateData* curData) const final override { return doGetEntityId(dynamic_cast<EnvironmentType&>(environment), node, pinN, *dynamic_cast<DataType*>(curData)); }
		ConfigNode getDevConData(ScriptEnvironment& environment, const ScriptGraphNode& node, IScriptStateData* curData) const override { return doGetDevConData(dynamic_cast<EnvironmentType&>(environment), node, *dynamic_cast<DataType*>(curData)); }
	};

	template <typename EnvironmentType>
	class ScriptNodeTypeBase<void, EnvironmentType> : public IScriptNodeType {
	public:
		static_assert(std::is_base_of_v<ScriptEnvironment, EnvironmentType>);
		
		virtual Result doUpdate(EnvironmentType& environment, Time time, const ScriptGraphNode& node) const { return Result(ScriptNodeExecutionState::Done); }
		virtual void doDestructor(EnvironmentType& environment, const ScriptGraphNode& node) const {}
		virtual bool doIsStackRollbackPoint(EnvironmentType& environment, const ScriptGraphNode& node, GraphPinId outPin) const { return false; }
		virtual ConfigNode doGetData(EnvironmentType& environment, const ScriptGraphNode& node, size_t pinN) const { return ConfigNode(); }
		virtual void doSetData(EnvironmentType& environment, const ScriptGraphNode& node, size_t pinN, ConfigNode data) const {}
		virtual EntityId doGetEntityId(EnvironmentType& environment, const ScriptGraphNode& node, GraphPinId pinN) const { return EntityId(); }
		virtual ConfigNode doGetDevConData(EnvironmentType& environment, const ScriptGraphNode& node) const { return {}; }

		Result update(ScriptEnvironment& environment, Time time, const ScriptGraphNode& node, IScriptStateData*) const final override { return doUpdate(dynamic_cast<EnvironmentType&>(environment), time, node); }
		void destructor(ScriptEnvironment& environment, const ScriptGraphNode& node, IScriptStateData*) const final override { return doDestructor(dynamic_cast<EnvironmentType&>(environment), node); }
		bool isStackRollbackPoint(ScriptEnvironment& environment, const ScriptGraphNode& node, GraphPinId outPin, IScriptStateData*) const final override { return doIsStackRollbackPoint(dynamic_cast<EnvironmentType&>(environment), node, outPin); }
		ConfigNode getData(ScriptEnvironment& environment, const ScriptGraphNode& node, size_t pinN, IScriptStateData*) const final override { return doGetData(dynamic_cast<EnvironmentType&>(environment), node, pinN); }
		void setData(ScriptEnvironment& environment, const ScriptGraphNode& node, size_t pinN, ConfigNode data, IScriptStateData*) const final override { doSetData(dynamic_cast<EnvironmentType&>(environment), node, pinN, std::move(data)); }
		EntityId getEntityId(ScriptEnvironment& environment, const ScriptGraphNode& node, GraphPinId pinN, IScriptStateData*) const final override { return doGetEntityId(dynamic_cast<EnvironmentType&>(environment), node, pinN); }
		ConfigNode getDevConData(ScriptEnvironment& environment, const ScriptGraphNode& node, IScriptStateData*) const override { return doGetDevConData(dynamic_cast<EnvironmentType&>(environment), node); }
	};

	class ScriptNodeTypeCollection {
	public:
		ScriptNodeTypeCollection();
    	void addScriptNode(std::unique_ptr<IScriptNodeType> nodeType);

		const IScriptNodeType* tryGetNodeType(const String& typeId) const;
		Vector<String> getTypes(bool includeNonAddable) const;
		Vector<String> getNames(bool includeNonAddable) const;

	private:
    	std::map<String, std::unique_ptr<IScriptNodeType>> nodeTypes;

		void addBasicScriptNodes();
	};
}
