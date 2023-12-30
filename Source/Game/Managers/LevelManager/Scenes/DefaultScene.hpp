#pragma once

class DefaultScene final : public Scene
{
public:
	DefaultScene();
	~DefaultScene() override = default;

	 void Init() override;
	 void Update() override;
};