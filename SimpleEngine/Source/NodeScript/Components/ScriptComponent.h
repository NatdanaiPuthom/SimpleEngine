#pragma once
#include "Components/Base/Component.h"
#include <memory>

namespace SCRIPT
{
	class Script;
}

enum class InteractionResult
{
	NoInteraction,
	MoveOntoTile,
	Stay,
};


class ScriptComponent final : public Component
{
public:
	InteractionResult myResult;
public:
	virtual ~ScriptComponent() override;

	virtual void Init(const GameInitContext& aContext) override;
	virtual void Update(const GameUpdateContext& aContext) override;
	InteractionResult Interact(const GameUpdateContext& aContext, GameObject* aGameObject);

	void SetScript(SCRIPT::Script* aScript);
	static void RegisterInternalScriptNodes();

private:
	SCRIPT::Script* myScript;
};