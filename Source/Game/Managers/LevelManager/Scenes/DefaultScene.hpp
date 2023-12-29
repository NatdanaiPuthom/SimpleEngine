#pragma once

class DefaultScene final : public Scene
{
public:
	DefaultScene();
	~DefaultScene() override = default;

	virtual void Init() override;
	virtual void Update() override;
};