#pragma once

namespace Scenes
{
	class NavmeshSceneTest final : public Simple::Scene, public Simple::Observer<Simple::eEvent>
	{
	public:
		NavmeshSceneTest();
		~NavmeshSceneTest();

		void Init() override;
		void Update() override;
		void Render() override;
	private:
		void ReceiveMessage(const Simple::Message<Simple::eEvent>& aMessage) override;
	};
}