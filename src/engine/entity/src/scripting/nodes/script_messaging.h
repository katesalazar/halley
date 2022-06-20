#pragma once
#include "scripting/script_environment.h"

namespace Halley {
	class ScriptSendMessage final : public ScriptNodeTypeBase<void> {
	public:
		String getId() const override { return "sendMessage"; }
		String getName() const override { return "Message Send"; }
		String getIconName(const ScriptGraphNode& node) const override { return "script_icons/send_message.png"; }
		ScriptNodeClassification getClassification() const override { return ScriptNodeClassification::Action; }

		Vector<SettingType> getSettingTypes() const override;
		gsl::span<const PinType> getPinConfiguration(const ScriptGraphNode& node) const override;
		std::pair<String, Vector<ColourOverride>> getNodeDescription(const ScriptGraphNode& node, const World* world, const ScriptGraph& graph) const override;
		std::pair<String, Vector<ColourOverride>> getPinDescription(const ScriptGraphNode& node, PinType elementType, ScriptPinId elementIdx) const override;
		
		Result doUpdate(ScriptEnvironment& environment, Time time, const ScriptGraphNode& node) const override;
	};

	class ScriptReceiveMessageData final : public ScriptStateData<ScriptReceiveMessageData> {
	public:
		ConfigNode curArgs;
		bool hasMessageActive = false;

		ConfigNode toConfigNode(const EntitySerializationContext& context) override;
	};

	class ScriptReceiveMessage final : public ScriptNodeTypeBase<ScriptReceiveMessageData> {
	public:
		String getId() const override { return "receiveMessage"; }
		String getName() const override { return "Message Receive"; }
		String getIconName(const ScriptGraphNode& node) const override { return "script_icons/receive_message.png"; }
		ScriptNodeClassification getClassification() const override { return ScriptNodeClassification::Terminator; }

		Vector<SettingType> getSettingTypes() const override;
		gsl::span<const PinType> getPinConfiguration(const ScriptGraphNode& node) const override;
		std::pair<String, Vector<ColourOverride>> getNodeDescription(const ScriptGraphNode& node, const World* world, const ScriptGraph& graph) const override;
		std::pair<String, Vector<ColourOverride>> getPinDescription(const ScriptGraphNode& node, PinType elementType, ScriptPinId elementIdx) const override;
		bool hasDestructor() const override;
		bool showDestructor() const override;

		void doInitData(ScriptReceiveMessageData& data, const ScriptGraphNode& node, const EntitySerializationContext& context,	const ConfigNode& nodeData) const override;
		Result doUpdate(ScriptEnvironment& environment, Time time, const ScriptGraphNode& node, ScriptReceiveMessageData& data) const override;
		ConfigNode doGetData(ScriptEnvironment& environment, const ScriptGraphNode& node, size_t pinN, ScriptReceiveMessageData& data) const override;
		void doDestructor(ScriptEnvironment& environment, const ScriptGraphNode& node, ScriptReceiveMessageData& data) const override;

		bool canReceiveMessage(const ScriptGraphNode& node, const String& messageId, bool requiresSpawningScript) const;
		bool tryReceiveMessage(const ScriptGraphNode& node, ScriptReceiveMessageData& data, ScriptMessage& msg) const;
		std::pair<String, int> getMessageIdAndParams(const ScriptGraphNode& node) const;
	};

	class ScriptSendSystemMessage final : public ScriptNodeTypeBase<void> {
	public:
		String getId() const override { return "sendSystemMessage"; }
		String getName() const override { return "System Msg. Send"; }
		String getIconName(const ScriptGraphNode& node) const override { return "script_icons/send_system_message.png"; }
		ScriptNodeClassification getClassification() const override { return ScriptNodeClassification::Action; }

		Vector<SettingType> getSettingTypes() const override;
		gsl::span<const PinType> getPinConfiguration(const ScriptGraphNode& node) const override;
		std::pair<String, Vector<ColourOverride>> getNodeDescription(const ScriptGraphNode& node, const World* world, const ScriptGraph& graph) const override;
		
		Result doUpdate(ScriptEnvironment& environment, Time time, const ScriptGraphNode& node) const override;
	};
}
