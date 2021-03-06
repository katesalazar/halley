#pragma once

#include "asset_editor.h"
#include "src/ui/scroll_background.h"

namespace Halley {
	class GraphEditor : public AssetEditor {
	public:
		GraphEditor(UIFactory& factory, Resources& resources, Project& project, AssetType type);

		void onMakeUI() override;

	protected:
		std::shared_ptr<ScrollBackground> scrollBg;
	};
}
