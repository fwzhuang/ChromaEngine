#ifndef _CHROMA_UICOMPONENT_H
#define _CHROMA_UICOMPONENT_H

#include <component/IComponent.h>

namespace Chroma
{
	class UIComponent :	public IComponent
	{
	public:
		// Functions
		void Init();
		void OnUpdate() {};
		void Destroy();
		void Serialize(ISerializer*& serializer) override;

		virtual void Draw() {};

		UIComponent();
		virtual ~UIComponent();

	protected:
	};
}
#endif