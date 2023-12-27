#pragma once

class SpotlightScene final : public Scene
{
public:
	SpotlightScene();
	~SpotlightScene() override = default;

	virtual void Init() override;
	virtual void Update() override;
};