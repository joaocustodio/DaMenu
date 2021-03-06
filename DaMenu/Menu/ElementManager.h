#pragma once
#include <vector>
class ElementManager
{
public:
	~ElementManager();
	ElementManager(InputManagerInterface* InputManager,RenderInterface* Renderer);
	uint32_t AddElement(MenuElement* Element);
	void Render();
	void UpdateInput();
	void ProcessKeyboardMessage(const KeyboardMessage& Msg);
	void ProcessMouseMessage(const MouseMessage& Msg);

	template<typename T>
	T* GetElementById(const uint32_t Id);
private:
	std::vector<MenuElement*> m_Elements;
	InputManagerInterface* m_InputManager;
	RenderInterface* m_Renderer;
};

ElementManager::~ElementManager()
{
	for (MenuElement* Element : m_Elements)
	{
		delete Element;
	}
	m_Elements.clear();
}

ElementManager::ElementManager(InputManagerInterface* InputManager,RenderInterface* Renderer)
{
	m_InputManager = InputManager;
	m_Renderer = Renderer;
	m_Renderer->Init();
	m_InputManager->EventKeyboardMessage() += 
		std::bind(&ElementManager::ProcessKeyboardMessage, this,
			std::placeholders::_1);
	m_InputManager->EventMouseMessage() +=
		std::bind(&ElementManager::ProcessMouseMessage,this,
			std::placeholders::_1);
}

uint32_t ElementManager::AddElement(MenuElement* Element)
{
	for (MenuElement* Element2 : m_Elements)
	{
		if (Element2->GetId() == Element->GetId())
			return Element2->GetId();
	}
	m_Elements.push_back(Element);
	return Element->GetId();
}

void ElementManager::Render()
{
	m_Renderer->PreFrame();
	for (MenuElement* Element : m_Elements)
	{
		Element->Draw(*m_Renderer);
	}
	m_Renderer->Present();
}

void ElementManager::ProcessMouseMessage(const MouseMessage& Msg)
{
	switch (Msg.GetEvent())
	{
	case MouseMessage::MouseEvent::Move:
		for (MenuElement* Element : m_Elements)
		{
			if(!Element->IsPointInControl(Msg.GetLocation()) &&
				Element->IsCursorInElement())
			{
				Element->OnMouseLeave(Msg);
			}else if(Element->IsPointInControl(Msg.GetLocation()) &&
				!Element->IsCursorInElement())
			{
				Element->OnMouseEnter(Msg);
			}

			if (Element->IsCursorInElement())
			{
				Element->OnMouseMove(Msg);
			}
		}
		break;
	case MouseMessage::MouseEvent::BtnDown:
		for (MenuElement* Element : m_Elements)
		{
			if(!Element->IsPointInMouseDownZone(Msg.GetLocation()))
				continue;

			Element->OnMouseDown(Msg);
		}
		break;
	case MouseMessage::MouseEvent::BtnUp:
		for (MenuElement* Element : m_Elements)
		{
			//if (!Element->IsPointInMouseDownZone(Msg.GetLocation()))
			//	continue;

			Element->OnMouseUp(Msg);
		}
		break;
	default:
		break;
	}
}

void ElementManager::ProcessKeyboardMessage(const KeyboardMessage& Msg)
{

}

void ElementManager::UpdateInput()
{
	m_InputManager->PollKeyboard();
	m_InputManager->PollMouse();
}

template<typename T>
T* ElementManager::GetElementById(const uint32_t Id)
{
	for (MenuElement* Element : m_Elements)
	{
		if (Element->GetId() == Id)
			return dynamic_cast<T*>(Element);

		//Search all windows for their elements also
		if (Element->GetType() == ElementType::Window)
		{
			WindowElement* WindowElem = dynamic_cast<WindowElement*>(Element);
			if (WindowElem != nullptr)
				return WindowElem->GetElementById<T*>(Id);
		}
	}
}